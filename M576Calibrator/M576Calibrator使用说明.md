# M576Calibrator 校准工具使用说明

## 1. 工具用途

**M576Calibrator** 用于 M576 / 1310 波长相关校准流程中的上位机辅助，主要能力包括：

- **整机单串口拓扑**：上位机 **只连接 439F 控制板** 串口；**不**再单独连接 MCS 串口。
- **439F 侧**：通过同一串口发送 **ASCII** 文本指令（如 `RECAL 1 …`），按路径表逐步执行并记录回传内容。
- **MCS / 1×64 定标读与烧录**：读 Flash 与烧录前由上位机发 **`trans <n>`** 进入透传、结束时发 **`$$`** 取消透传；其间隔内仍按 Z4671 **二进制帧**（`GetLogFileData` 读、`StartFWUpdate` → 分片 → `FWUpdateEnd` 写）。透传目标与通道列表在源码 **`CalibConstants.h`**（`g_m576FlashReadTransChannels` / `g_m576FlashBurnTransChannels`）中配置，默认 **trans 1～4**。
- **1310 低温槽**：支持在生成 BIN 时，将内存中的 LUT 数据 **仅合并到低温温度槽（`IDX_TEMP_LOW`）**，其余温度点可从备份 BIN 保留。

具体命令格式、点数、超时等请以 **固件版本说明** 与 **《需求Spec PRD》** 为准；本工具对 RECAL 响应提供日志与简易数值解析，便于联调迭代。

---

## 2. 运行环境与启动

- **系统**：Windows（与工程一致，建议 Win32 程序在 64 位系统上正常运行）。
- **工程**：Visual Studio 2015（v140）、**多字节字符集（MBCS）**、MFC 动态库；编译生成可执行文件后，在本机直接运行。
- **硬件**：PC 具备 **一个** 连接至 **439F** 的串口（或 USB 转串口），波特率程序内默认为 **115200，8N1**（与代码一致；若现场不同需改源码或重新编译）。

---

## 3. 主界面区域说明

| 区域 | 说明 |
|------|------|
| **串口 (439F)** | 下拉选择 COM（COM1～COM32），对应整机 439F 调试口。 |
| **打开串口** | 打开上述端口；失败时查看下方日志。 |
| **Path CSV** | **固化**为 `CalibConstants.h` 中默认的 `output\pm_*.csv`（PM）或 `output\pd_*.csv`（PD），界面仅 **一行简要说明**；切换 **Mode** 时更新说明；**Run path** 前会再次套用默认路径。 |
| **Mode** | **PM (RECAL 1)**：功率计方案，Command A + B；**PD (RECAL 2)**：板载 PD 方案，Command C（无 **RECAL 0**）。 |
| **delay ms / DAC range / DAC step** | 用于 **`RECAL 3` / `RECAL 5`** 扫点及超时估算；**不**随 `RECAL 0` 下发（固件定稿：`RECAL 0` 仅波长，默认 1310 nm）。 |
| **Backup BIN** | （可选）**基路径**（如 `output\mcs_lut_backup.bin`）。读 Flash 后在同目录生成 **`基名_t1.bin` … `基名_t4.bin`**。「写 BIN」时按 **trans 1～4** 分别读取对应的 `*_tN.bin`（若存在）与内存中 **该路** 定标结果合并；仅 trans1 可回退读 **单一旧文件**（无 `_t1` 时）。 |
| **Output BIN base** | **基路径**（如 `output\standard.bin`）。**写 BIN** 写出 **`基名_t1.bin` … `基名_t4.bin`** 共四个文件；**烧录** 按 `CalibConstants.h` 中的通道列表，对每个 trans 烧录磁盘上对应的 **`_tN.bin`**（缺省或空文件则跳过该路并打日志）。 |
| **Bundle SN** | 写入 Bundle 头用的序列号；为空时程序内有默认占位，正式使用请填写真实 SN。 |
| **进度条** | 「跑路径」时按步数更新；「烧录定标」时按分包进度更新。 |
| **日志窗口** | 显示操作结果、报错及 RECAL 回传摘要。 |

---

## 4. 推荐操作流程

### 4.1 连接串口

1. 将 USB/串口线连接至 **439F** 控制板，在设备管理器中确认 COM 号。  
2. 在界面 **串口 (439F)** 中选择对应端口。  
3. 点击 **打开串口**。成功则日志提示端口已打开。

### 4.2 路径 CSV（程序内置）

- 四路 PM / 四路 PD 的文件名与相对目录写死在 **`CalibConstants.h`**（`g_m576DefaultPmCsvRel` / `g_m576DefaultPdCsvRel`），构建时 PostBuild 拷贝到 **`程序目录\output\`**。  
- 工艺上仍可将 `standard_pm.csv` / `standard_pd.csv` **按 target 拆成四份**覆盖上述文件；也可用 **`tools\split_path_csv_eight.ps1`** 生成。  
- 某路文件 **缺失** 或 **0 行**：该槽 **跳过**（Warn）。若要改路径或文件名，需改常量并重新编译。

### 4.3 跑路径（RECAL）

1. 确保串口已打开（若未打开，点击 **Run path** 时会尝试自动打开）。  
2. 选择 **Mode**，确认当前模式下的 **四路 CSV**。  
3. 点击 **Run path (RECAL)**。  
4. **PM**：清零四份内存 LUT；若已填 **Backup BIN** 基路径且磁盘上存在对应 **`基名_tN.bin`**，则先 **预载** 到 `m_lutByTrans`（再跑路径时在之上更新）；随后发 **一次** `RECAL 0`，再按 **trans 1→4** 依次加载 PM 四文件并跑 `RECAL 1` + `RECAL 3`，寻峰结果写入 **`m_lutByTrans[槽位]`**；进度为四文件 **总行数**。  
5. **PD**：同上（含可选预载），**无 RECAL 0**，顺序为 `RECAL 2` + `RECAL 5`，写入对应 **`m_lutByTrans[槽位]`**。  
6. 需要中断时点 **Stop**。

**PD 与功率计差异（Z4744）**

- **功率计**：`RECAL 1`，目标 **1～6**；四路文件与 **1#MCS / 2#MCS / 1#1×64 / 2#1×64** 对应关系见 **`TransLutRoute.cpp`（PM）**。  
- **PD**：`RECAL 2`，目标 **1～4**；槽位映射见 **`TransLutRoute.cpp`（PD）**，与 `LutPeakApply` 写 LUT 约定一致。  
- 扫参在 **`RECAL 3` / `RECAL 5`**；列数：**5** / **3**。

### 4.4 生成 BIN（写 BIN）

1. 设置 **Output BIN base**（如 `output\standard.bin`）。  
2. 可选：**Backup BIN** 基路径；写 BIN 时读取 **`基名_t1.bin`…`基名_t4.bin`**（若存在）与各路内存 LUT 做 **1310 低温槽** 合并。  
3. 点击 **Write BIN**，生成 **四个** `基名_tN.bin`。无某路备份时该路仅写内存 LUT。

### 4.4.1 读 Flash 备份（439F `trans`）

1. 在 **Backup BIN** 填写 **基路径**（例如 `output\mcs_lut_backup.bin`）。  
2. 打开串口后点击 **Read Flash backup**。  
3. 程序对每个配置的 **trans 通道** 依次：`$$`（容错）→ `trans N` → 按 Z4671 读整包 LUT → `$$`，并在基路径上生成 **`基名_tN.bin`**（如 `mcs_lut_backup_t1.bin` …）。通道含义与列表以 **`CalibConstants.h`** 为准。

### 4.5 烧录定标（经 439F `trans` 至各下游）

1. 确认已执行 **写 BIN**，且 **Output BIN base** 同目录下存在至少一个 **非空的 `基名_tN.bin`**。  
2. 确保 **439F 串口** 已打开（未打开时点击烧录会尝试 **打开串口**）。  
3. 点击 **烧录定标**。  
4. 程序对每个 **烧录通道**（默认 trans 1～4，见 **`CalibConstants.h`**）使用 **该通道对应的 `_tN.bin` 全路径** 上传；缺失或 **0 字节** 的文件 **跳过** 该 trans。进度条为各通道分包总和。烧录结束后固件侧等待与既有实现一致。

---

## 5. 路径 CSV 格式（功率计 / `RECAL 1`）

与 **Z4744 Command B** 一致：串口 `RECAL 1` 在 `target_switch_index` 之后只有 **四个** 光路参数（四段各一个 **通道号**），无单独的「块」字段。

每行 **5 个整数**，逗号分隔：

```text
target_index, ch1, ch2, ch3, ch4
```

- **target_index**：目标校准开关索引（PRD：**1～6**，对应 1#1x64 Stage1/2、1#/2# MCS、2#1x64 Stage1/2）。  
- **ch1**：1#1×64 通道 **1～64**，  
- **ch2**：1#MCS 通道 **1～18**，  
- **ch3**：2#MCS 通道 **1～18**，  
- **ch4**：2#1×64 通道 **1～64**，  

顺序与拓扑 **1#1×64 → 1#MCS → 2#MCS → 2#1×64** 一致。

**PM（功率计）模式**完整定标需遍历 **1286 步**（与 PRD 一致）：  

`64 + 3 + 32×18 + 32×18 + 64 + 3 = 1286`  

**PD 模式**路径为 **1219 步**（仅经 2#1×64 与单侧 MCS，见下文「5.1 PD 路径 CSV」）：  

`64 + 3 + 32×18 + 32×18 = 1219`  

| 段 | 行数 | 含义（与 `output\standard.csv` 生成脚本一致） |
|----|------|---------------------------------------------|
| 64 | 64 | `target_index=1`，扫 1#1x64 前级通道 1…64 |
| 3 | 3 | `target_index=2`，边界点（ch 1 / 32 / 64） |
| 576 | 576 | `target_index=3`，1# MCS 全网格 32×18 |
| 576 | 576 | `target_index=4`，2# MCS 全网格 32×18 |
| 64 | 64 | `target_index=5`，扫 2#1x64 通道 1…64 |
| 3 | 3 | `target_index=6`，边界点（ch 1 / 32 / 64） |

默认八路 CSV 见 **`CalibConstants.h`**（`g_m576DefaultPmCsvRel` / `g_m576DefaultPdCsvRel`）；编译后 PostBuild 会拷贝到 **`程序目录\output\`**。写 BIN / 烧录使用 **同一套基路径** 生成与查找 **`_t1.bin`…`_t4.bin`**。具体 `target_index` 与四路文件的对应以 **`TransLutRoute.cpp`** 为准，需与 **固件 / 现场 path** 对齐。

**Backup BIN**：多通道 **Read Flash backup** 后得到各 `*_tN.bin`；写 BIN 时按路读取并与该路内存 LUT 合并。未读备份时可留空，则写出路径仅含本次 Run path 写入的各 `m_lutByTrans`。

可选：表头行以 **`target_index`** 开头会被自动跳过；以 **`#`** 开头的注释行会被忽略。

程序内会对 **target_index** 与 **ch1/ch4（1～64）、ch2/ch3（1～18）** 做粗校验。

示例表头与一行：

```csv
target_index,ch1,ch2,ch3,ch4
1,1,1,1,32
```

### 5.1 PD 路径 CSV（`RECAL 2`，3 列）

固件线格式为 **`RECAL 2 <target> <2#1x64 ch> <MCS ch>`**，每行 **3 个整数**（不含表头注释）：

```text
target_index, ch1, ch2
```

- **target_index**：**1～4**（1 = 2#1×64 Stage_1；2 = Stage_2；3 = 1# MCS；4 = 2# MCS）。  
- **ch1**：2#1×64 通道 **1～64**。经物理拓扑，**ch1 1～32** 走 **1# MCS**，**ch1 33～64** 走 **2# MCS**（由 ch1 隐式决定，不再单独传 bank）。  
- **ch2**：MCS 通道 **1～18**。  
- **校验**：`target_index=3` 时要求 **ch1 为 1～32**；`target_index=4` 时要求 **ch1 为 33～64**。

**步数 1219**（与拓扑一致：`64 + 3 + 576 + 576`，其中 576 = 32×18 单侧 MCS 全网格）：

| 段 | 行数 | 含义 |
|----|------|------|
| 64 | 64 | target=1，扫 2#1×64 ch 1…64 |
| 3 | 3 | target=2，边界点 ch 1 / 32 / 64 |
| 576 | 576 | target=3，1# MCS 32×18 |
| 576 | 576 | target=4，2# MCS 32×18（ch1 = 33…64） |

示例与默认表见 **`output\standard_pd.csv`**（可用 `tools\generate_sample_path_pd_1219.py` 重新生成）。

---

## 6. 注意事项

1. **仅需一路** 与 439F 相连的串口；无需在 PC 上再选 MCS COM。  
2. **烧录有风险**，务必先备份现场 BIN，并在工艺允许时再执行「烧录定标」。失败时按现场流程恢复备份或重新烧录。  
3. **RECAL 语法与响应** 以固件为准；若超时或乱码，检查接线、波特率、线序及固件版本。  
4. 定标 BIN 烧录依赖 **439F 显式 `trans`/`$$` 与 Z4671 二进制帧**；`trans` 文本与应答格式以固件为准，必要时调整 **`Board439fTransTunnel`** 与 **`CalibConstants.h`** 中的超时。  
5. 本工具 LUT 帧格式按 **Z4671**；若某 `trans` 下游非 Z4671 分区，请在 **`g_m576FlashBurnTransChannels`** / **`g_m576FlashReadTransChannels`** 中排除该通道，避免误烧/误读。

---

## 7. 相关文档与工程路径

- 需求与协议摘要：`需求Spec PRD.md`  
- 解决方案：`M576Calibrator\M576Calibrator.sln`  
- 编译与字符集说明：`M576Calibrator\TESTING.txt`  

---

*文档版本随软件迭代更新；若界面文案与程序不一致，以实际程序为准。*
