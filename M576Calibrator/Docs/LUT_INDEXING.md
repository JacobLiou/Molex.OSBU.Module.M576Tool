# MCS LUT 下标与命名对照（LUT_INDEXING）

产线、固件、Z4671 老工具与 M576 上位机混用 **SN / sw / ch / 块号** 时，以本文为准对表。结构体定义见 `Z4671Core/Z4767StructDefine.h` 中 `stLutSettingZ4671::wCalibPtrDAC`。

```cpp
WORD wCalibPtrDAC[34][TEMP_CALIB_NUM][PORT_MAX_COUNT + MID_MAX_COUNT][2];
// 即 [34][温区][32通道][Y/X 或 0/1]
```

- `PORT_MAX_COUNT = 20`
- `MID_MAX_COUNT = 12`
- 通道维合计 **32** = 20 端口 + 12 中转（Mid）

温区（与 Z4671 一致）：

| 宏 | 值 | 说明 |
|----|-----|------|
| `IDX_TEMP_LOW` | 0 | 低温档；**1310 定标只写此槽** |
| `IDX_TEMP_ROOM` | 1 | 常温 |
| `IDX_TEMP_HIGH` | 2 | 高温 |

---

## 1. 第一维：SN 与 `sw`（0..33）

| 对外/Excel 说法 | `wCalibPtrDAC` 下标 `sw` | 1310 是否 Merge | 说明 |
|-----------------|--------------------------|-----------------|------|
| SN1 .. SN32 | **0 .. 31** | **是** | MCS 开关块；见下文块号映射 |
| SN33 | **32** | **否** | 1×64 相关镜像槽，保留 backup |
| SN34 | **33** | **否** | 同上 |

宏：`M576_MCS_LUT_SW_MERGE_COUNT == 32`（`CalibConstants.h`）。

**历史问题**：若 Merge 循环写成 `sw < 34`，session 中的零会覆盖 SN33/34 的低温 DAC，导致与固件扯皮。当前 `MergeLut1310LowTempSlot` 仅 `sw < 32`。

---

## 2. MCS 块号 1..32 → `sw` 0..31

路径/日志里的 **MCS 块号 1..32** 不是简单的 `sw = 块号 - 1`，而是固件约定重排：

```cpp
// CalibConstants.h — M576McsBlock1To32ToLutSwIdx0(block1to32)
// block 1..16  → sw 16..31
// block 17..32 → sw 0..15
```

| MCS 块号 (1..32) | LUT `sw` |
|------------------|----------|
| 1 | 16 |
| 2 | 17 |
| … | … |
| 16 | 31 |
| 17 | 0 |
| 18 | 1 |
| … | … |
| 32 | 15 |

对表 Z4671 CSV 时：先确认 CSV 列是 **SN 序号** 还是 **sw 下标**，再查上表。

---

## 3. 第二维：路径通道与 LUT `ch`（0..31）

| 对外说法 | LUT `ch` | 路径 CSV | 1310 Run Path |
|----------|----------|----------|---------------|
| CH1 .. CH18 | **0 .. 17** | `c2`/`c3` 或 PD `ch2`：**1..18** | **会定标并应写入** |
| CH19 .. CH32 | **18 .. 31** | 无默认路径步 | **不应被未校准 session 覆盖** |

校验：`PathCsvDriver.cpp` — `MCS channel out of 1..18`。

通道语义（结构体层面）：

- **ch 0 .. 19**：`PORT_MAX_COUNT` 端口通道  
- **ch 20 .. 31**：Mid 中转点（`MID_MAX_COUNT`）

产线口语「CH19–CH32」= LUT **ch 18–31**。

---

## 4. DAC 对与 CSV 列

每个 `(sw, 温区, ch)` 有两个 WORD，存 **有符号 int16** DAC：

| 下标 | 常见含义 |
|------|----------|
| `[0]` | dacY |
| `[1]` | dacX |

1310 会话写 LUT 时通常只填 **LOW** 槽（见 `M576CalibratorDlg` 中 `ParseLowTemp1310DacCsv` / Run Path 写表逻辑）。

---

## 5. 四路 trans 与 MCS 实例

| trans 槽 | 典型设备 | 内存 |
|----------|----------|------|
| 0, 1 | 1# / 2# MCS | `m_lutByTrans[0/1]` → 各自 `stLutSettingZ4671` |
| 2, 3 | 1# / 2# 1×64 | `m_memsByTrans[2/3]` → `stM576OneX64MemsSwCoef`（非 `wCalibPtrDAC`） |

1310 MCS Merge 在 **Write BIN / MakeBin** 时按 trans 分别：`MergeLut1310LowTempSlot(merged, m_lutByTrans[i])`。

1×64 低温：`MergeMems1310LowTempSlot` 只拷 `stCalibDAC[0]`，与 MCS 的 SN33/34 槽无关。

---

## 6. Read / Write / 对表流程

```text
Read Bin  → backup 进 m_lutInitial / 文件
Calibrate → session 写入 m_lutByTrans（可能仅部分 sw/ch）
Write Bin → merged = backup + Merge(session) → 落盘
Burn      → 透传烧录
```

对表争议时：

1. 比 **同一文件** 的 backup vs merged，不要混 Initial 与 Read 时间点的不同文件。  
2. 先查 **sw 32/33、ch 18..31** 是否被 INV-02 / INV-03 违反。  
3. Read 仅验证 INV-20；Write 查 Merge 实现。

---

## 7. 快速查表（扯皮常用）

| 现象 | 优先查 |
|------|--------|
| SN33/34 LOW 全 0，ROOM 有值 | INV-02、设备历史；非 Read 单独清零 |
| CH19–32 变 0，只跑了 1..18 路径 | INV-03、INV-06 稀疏 merge |
| 负 DAC 变 0 | INV-04、INV-13 |
| 平扫 -999999 仍一直重试 | INV-10 |
| Read 与 Z4671 整包不一致 | INV-21 Flash 基址 |

---

## 8. 相关文件

| 主题 | 文件 |
|------|------|
| Merge 1310 | `LutMerge1310.cpp` |
| Merge 计数宏 | `CalibConstants.h` |
| Write BIN 编排 | `M576CalibratorDlg.cpp` |
| Read LUT | `McsFwTransport.cpp` |
| 不变量 | [`INVARIANTS.md`](INVARIANTS.md) |
