# M576 上位机：1×64 MEMS 校准数据写入地址说明

本文档供与固件同事会议对齐使用，说明 **1310 nm PM 定标流程中，1×64 寻峰得到的 DAC 写入 bin/内存 的寻址规则**。实现以源码为准，路径均相对于 `M576Calibrator` 工程。

---

## 1. 范围与前提

| 项目 | 说明 |
|------|------|
| 数据内容 | 二维寻峰后的 **DAC X / DAC Y**（经 int16 钳位后以 `short` / 16 位有符号模式落盘） |
| 温度槽 | **仅低温档**：`stCalibDAC[0]`（与 MCS LUT 的 `IDX_TEMP_LOW == 0` 对应） |
| 协议侧 RECAL | PM 使用 `RECAL 1`；路径 CSV 中 `target_index` + `ch1..ch4` 与 Z4744 一致 |
| 结构体布局 | 与 126S `tagMemsSwCoef` 对齐，见 `Z4671Core/M576OneX64Coef.h` |

**不在本文展开**：透传读 Flash、XMODEM 烧录命令序列；仅描述 **逻辑端口 / 映射表 → 结构体字段 → 字节偏移** 的对应关系。

---

## 2. 物理与软件分层（便于会议对齐术语）

- 一块 **1×64 trans**（槽位 3 或 4）在内存/备份中对应 **`stM576OneX64MemsSwCoef` 数组 `[4]`**：即 **4 片 MEMS 开关系数**（常称 sw1～sw4），每片最多 **36** 个 `stChnDAC` 槽位（宏 `M576_1X64_MAX_CHANNEL_NUM`），PM 映射表当前使用 **CH_y 1～17**。
- 每片在磁盘上通常为 **独立 bin**：`sizeof(stM576OneX64MemsSwCoef) == 2208`（160 B `BUNDLEHEADER` + 2048 B body）。
- 固件 **MEM 读 8 KB** 时，多为 **4×2048 B body 连续拼接**（再合成带头的 4 个 2208 B 文件），与下文 **「8K 内偏移」** 一致。

---

## 3. 核心规则：谁决定写哪一扇区

### 3a. PM（功率计）当前实现：固件映射表

**Trans 槽 3/4** 跑 `pm_1x64_*.csv` 时，MEMS 低温写址由 **`pm_1x64_*Mapping.csv`** 决定（与 PM CSV **同行序**；前五列必须与 PM 行完全一致）。

| 项目 | 说明 |
|------|------|
| 映射路径 | 由 PM CSV 推导：`xxx\pm_1x64_1.csv` → `xxx\pm_1x64_1Mapping.csv`（在扩展名前插入 `Mapping`） |
| 加载与校验 | `Pm1x64Mapping.cpp`：`LoadPm1x64MappingCsv`、`ValidatePmStepsAgainstMapping` |
| 写 MEMS | `WriteMems1x64LowTempDacPair`：`block = SW_x - 1`，`inBlk = CH_y - 1`（表中 `CH_y` 为 1-based） |
| 统计 CSV 偏移 | `CalibBuildStatRowPmMemsMapped`（内部 `Mems1x64OffsetsIn8k(block, inBlk)`） |

映射缺失或与 PM 前五列不一致时，**跳过该槽整段 PM**（不打静默错误 bin）。

### 3b. 遗留规则（`ApplyRecalPeakToMems1x64`）

`ApplyRecalPeakToMems1x64` 仍保留在 `LutPeakApply.cpp`（按 `ch1`/`ch4` 做 `/16`、`%16`）。**PM trans3/4 已不再调用**；**PD** 仍使用 `ApplyRecalPeakToMems1x64Pd`（按端口推导，本轮未引入 PD 映射表）。

| `target_index` | 遗留 MEMS 写址所用的逻辑端口 |
|----------------|------------------------------|
| **1、2** | **`ch1`** |
| **5、6** | **`ch4`** |
| **3、4** | 走 MCS LUT，不写本 MEMS 结构 |

---

## 4. 遗留公式：端口 1～64 → div16 → `stChnDAC`

对选定的 `ch`（`ch1` 或 `ch4`）：

```text
ch0   = ch - 1          // 0..63
block = ch0 / 16        // 0..3
inBlk = ch0 % 16        // 0..15
```

**运行时写入位置**（低温）：

```text
m_mems1x64[transIndex][block].stCalibDAC[0].stChnDAC[inBlk].sDACx
m_mems1x64[transIndex][block].stCalibDAC[0].stChnDAC[inBlk].sDACy
```

- `transIndex`：`fileSlot - 2`。PM 下 target 1、2 → trans 槽位索引 2；target 5、6 → 槽位 3（见 `TransLutRoute.cpp`）。
- **X/Y**：寻峰 **Y → `sDACx`**，**X → `sDACy`**。
- **`wValid`**：每片 switch、低温槽 `stCalibDAC[0]` 一个 `WORD`；`WriteMems1x64DacPair` 在 `calibSlot==0` 写 DAC 后置 **1**（与 126S/另一上位机一致）。单文件 body 内偏移 **1024**，文件绝对偏移 **1184 (0x4A0)**，小端 `01 00`。
- **Write BIN 合并**：`MergeMems1310LowTempSlot` 仅把会话中 **`stChnDAC[0..16]`**（CH_y 1..17）写入备份；**`stChnDAC[17]`（第 18 路）及 `stChnDAC[18..35]`、`stMidDAC[]` 始终保留备份 bin**，不会被会话中的 0 覆盖。

---

## 5. 字节偏移（统计 CSV / 与固件对 hex）

`CalibWriteMeta.cpp` → `Mems1x64OffsetsIn8k(block, inBlk, oX, oY)`：计算 **8K body 拼接布局** 下 `sDACx`/`sDACy` 的字节偏移。

PM 映射模式下，`CalibBuildStatRowPmMemsMapped` 使用映射给出的 `block`/`inBlk`（即 `CH_y-1`）。

---

## 6. 源码索引

| 内容 | 文件 |
|------|------|
| 映射 CSV | `M576CalibratorApp/Pm1x64Mapping.cpp` |
| 按槽写 MEMS | `M576CalibratorApp/LutPeakApply.cpp`（`WriteMems1x64LowTempDacPair`） |
| PM 路径集成 | `M576CalibratorApp/M576CalibratorDlg.cpp`（`RunPathPowerMeterFile`，`fileSlot >= 2`） |
| 统计行 | `M576CalibratorApp/CalibWriteMeta.cpp` |
| 结构体 | `Z4671Core/M576OneX64Coef.h` |

---

## 7. 修订记录

| 日期 | 说明 |
|------|------|
| 2026-05-01 | PM：`pm_1x64_*Mapping.csv` + `WriteMems1x64LowTempDacPair`；PD 仍按端口推导。 |
