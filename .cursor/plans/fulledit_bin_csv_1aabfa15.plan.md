---
name: FullEdit Bin CSV
overview: 研发调试旁路：Make Bin/FineTune 旁增加 FullEdit Bin（全温区 CSV 差分写回 + 坏光路 DAC 总结）。不得改动 Read/Calibrate/Write BIN/Burn 主流程语义。
todos:
  - id: spec-doc
    content: 落盘 design spec（含受众=研发、主流程隔离、校验/命名/错误码/光路总结）
    status: completed
  - id: path-impact
    content: 实现 PathDacImpact（CH→4×FineTuneAddress）+ CrossPeakTest
    status: completed
  - id: dac-io-mcs
    content: FullEditDacIo：MCS 全温区 export/diff/import + 危险行解锁
    status: completed
  - id: dac-io-mems
    content: FullEditDacIo：1x64 MEMS 三温 CH/MID CSV 对称实现
    status: completed
  - id: fulledit-dlg
    content: CM576FullEditDlg：Export / Unlock / Validate&Write / Path Impact 面板
    status: completed
  - id: main-wire
    content: 主窗仅加按钮/生命周期（与 FineTune 同级）；禁止改 Run Path/Merge/Burn 逻辑
    status: completed
  - id: docs-invariants
    content: 使用说明标注「研发调试」+ INVARIANTS 主流程隔离条目
    status: completed
isProject: false
---

# FullEdit Bin + 坏光路 DAC 总结

> **For agentic workers:** Use subagent-driven-development or executing-plans; implement task-by-task. Spec + plan live under `docs/superpowers/` after approval.

**Goal:** 为**研发工程师**提供与 FineTune / Make Bin 同级的**额外调试调节**能力：Excel 编辑 10 路 burn bin 的 LOW/ROOM/HIGH DAC 并差分写回；按拓扑坏光路（CH）汇总受影响 DAC。不面向产线主流程自动化。

**Architecture:** 无 MFC 的 `FullEditDacIo` + `PathDacImpact` 放在 App 层旁路模块；`CM576FullEditDlg` 模型ess，镜像 FineTune。主窗只接线按钮与 Busy 门闩，**零改动** `Run Path` / RECAL / Merge / `GenerateStandardBinFiles` 业务语义。

**Tech Stack:** MFC VS2022、现有 Z4671Core bin writers、UTF-8-SIG CSV。

## Global Constraints

### 受众与主流程隔离（硬约束）

- **受众**：研发调试 / 坏路急救；与 **FineTune**、**Make Bin** 同一类开放能力，**不是**产线标准 SOP 步骤。
- **不得影响主流程**：`Read Bin` → `Calibrate (Run Path)` → `Write BIN` → `Burn Flash` 的顺序、门限、Merge/INV、会话 LUT 写入路径**禁止**为 FullEdit 改语义或插副作用。
- FullEdit **只读写磁盘上的** `{SN}_backup.bin` / `{SN}_standard.bin`（及 `fulledit/` CSV）；**不**自动 Burn；**不**改路径 CSV；**不**改 RECAL 参数默认值。
- **默认不写主窗 session LUT/Mems**：写回成功后**不要**默认调用类似 `OnFineTuneBinPatched` 去覆盖内存定标结果（避免 Recal 会话被旁路污染）。若需与主窗对齐，仅可选显式勾选「同步到会话内存」，默认关闭。
- UI 文案 / 使用说明须标明 **「研发调试」**；按钮放在 Make Bin / FineTune 邻近，勿插入产线主按钮序列中间造成误导。
- 主流程 Busy 时 FullEdit 可拒绝 Export/Write（`FE_BUSY`），但 FullEdit **不得**延长或改写主流程 Busy 语义。

### 数据与安全

- DAC 为有符号 int16（`M576_RECAL_DAC_MIN`..`MAX`）；写回必须走现有 Writer（CRC 正确）。
- 危险行默认不可写；须对话框显式解锁，否则 **整次写回失败**（禁止静默丢改）。
- 相对 **导出基线** 只 patch 有 diff 的行/文件。
- MCS：`[0]=dacY` `[1]=dacX`；1x64：`sDACx`/`sDACy` 与 FineTune 读法一致（写回时勿搞反轴）。
- 1x64 CSV **只暴露** `stCalibDAC[0/1/2]`（LOW/ROOM/HIGH）；不暴露第 4 温槽。
- 坏光路输入：**产品 CH 1..576**（与 IL Test 相同）；界面展示推导出的 MPO + 四级地址。

---

## 1. 规格：交互与文件命名

### 1.1 UI 入口（研发工具区）

- 主对话框在 [M576CalibratorApp.rc](M576Calibrator/M576CalibratorApp/M576CalibratorApp.rc) **Make Bin** / **FineTune** 邻近增加 **FullEdit Bin**（同属研发工具区，勿插入 Read/Calibrate/Write BIN/Burn 主按钮链）。
- 按钮旁或对话框标题带简短提示：**研发调试**。
- 打开模型ess `CM576FullEditDlg`（模式对齐 [M576FineTuneDlg](M576Calibrator/M576CalibratorApp/M576FineTuneDlg.h)）：Busy 检查、`ValidateSnBeforeBinOp`、**本地 log**（不刷主窗产线 Log）、不关主窗。

### 1.2 对话框分区

```mermaid
flowchart LR
  role[Select Backup or Standard]
  export[Export 10 CSV plus baseline]
  edit[User edits working CSV in Excel]
  path[CH 1..576 Path Impact panel]
  apply[Validate and Write diffs]
  role --> export --> edit --> apply
  role --> path
  path -.->|"lists FineTuneAddress keys"| edit
```

| 区 | 行为 |
|----|------|
| Role | Backup / Standard → `M576BinFileRole` + `M576BuildBurnFilePaths` |
| Export | 写 `working/` + `baseline/` 各 10 CSV；打开文件夹；提示用 Excel 改 working |
| Unlock | 勾选后才允许危险行进入 patch（见 §3） |
| Path Impact | 输入 CH → 列出 4 个设备地址、burn index、CSV 行键、可选当前三温 DAC |
| Validate & Write | working vs baseline diff → 校验 → 只写有改动的 bin |

### 1.3 目录与命名

根目录（`output\latest` 旁或之下）：

```text
{outDir}/fulledit/{backup|standard}/
  working/
    {SN}_dac.csv          # 10 个，SN = M576SnForBurnFileIndex 0..9
  baseline/
    {SN}_dac.csv          # 导出时只读快照；写回比对用
  path_impact/
    CH{nnn}_impact.txt    # 可选：点「导出光路总结」时生成
```

Burn index ↔ 文件（与 FineTune 一致）：

| idx | 设备 | CSV kind |
|-----|------|----------|
| 0–1 | MCS1 / MCS2 | `schema=mcs` |
| 2–5 | 1x64_1 SW1–4 | `schema=mems` |
| 6–9 | 1x64_2 SW1–4 | `schema=mems` |

---

## 2. 规格：CSV Schema（同行可比较）

### 2.1 MCS（34×32 行）

表头（写回 **忽略** `delta_*` 与只读注释列以外的计算列；`temp_point_*` 若改动则写回 `wTemperaturePoint`）：

```text
schema,burn_index,sn_label,sw_lut_idx,optical_block,ch_idx,ch_kind,
temp_point_LOW_0p1C,temp_point_ROOM_0p1C,temp_point_HIGH_0p1C,
LOW_dac_y,LOW_dac_x,ROOM_dac_y,ROOM_dac_x,HIGH_dac_y,HIGH_dac_x,
delta_ROOM_LOW_y,delta_ROOM_LOW_x,delta_HIGH_LOW_y,delta_HIGH_LOW_x,
delta_HIGH_ROOM_y,delta_HIGH_ROOM_x
```

- 对齐现有 [export_mcs_bin_dac.py](M576Calibrator/CrossPeakTest/export_mcs_bin_dac.py)，并加 `schema,burn_index`。
- 行键：`(sw_lut_idx, ch_idx)`。

### 2.2 1×64 MEMS（每 burn 文件一张）

```text
schema,burn_index,sn_label,sw1to4,row_kind,row_idx,
temp_centi_LOW,temp_centi_ROOM,temp_centi_HIGH,
LOW_dac_y,LOW_dac_x,ROOM_dac_y,ROOM_dac_x,HIGH_dac_y,HIGH_dac_x,
delta_ROOM_LOW_y,...
```

- `row_kind`=`CH`：`row_idx` 0..35（`stChnDAC`）；`MID`：0..24（`stMidDAC`）。
- 三温 ↔ `stCalibDAC[0/1/2]`；轴与 FineTune `ReadMemsDacPair` 一致（文件里 `sDACx`/`sDACy` 命名以现有读写为准，CSV 列统一叫 `*_dac_y/*_dac_x` 并在 docs 写清映射）。
- 行键：`(row_kind, row_idx)`。

---

## 3. 规格：危险行、差分、校验、错误码

### 3.1 危险行（默认不可写）

| 设备 | 危险条件 |
|------|----------|
| MCS | `sw_lut_idx` ∈ {32,33}（SN33/34）；`ch_idx` ≥ 18（含 PORT18–19 与 MID） |
| 1×64 | `row_kind=CH` 且 `row_idx` ≥ 17；或 `row_kind=MID` |

解锁勾选（建议拆开）：`Unlock SN33/34`、`Unlock MCS ch≥18`、`Unlock 1x64 CH≥18/MID`。任一危险 diff 存在且未解锁 → 失败码 `FE_DANGEROUS_LOCKED`。

### 3.2 差分写回规则

1. 每个 burn 文件：parse working + baseline；行键对齐。
2. 仅当某行任一 **可写列**（三温 DAC 六列 + 可选 temp_point/temp_centi）相对 baseline 变化 → 计入 patch。
3. 文件零 diff → 跳过该 bin。
4. working 多行/缺行/键重复 → 失败（见错误码）。
5. Patch 后：`CLutBinWriter::Write` / `CMems1x64LutBinWriter::WriteSingleSwitch`。**默认不同步**主窗 session LUT/Mems（与 FineTune 的自动 sync 区分，避免污染定标会话）。仅当用户勾选「同步到会话内存」时才回调主窗。

### 3.3 校验清单（Validate）

- [ ] Role 目录下 10 对 working/baseline 文件名与 `M576BuildBurnFilePaths` SN 一致（允许「仅改过的文件存在」时：缺失 working 视为该文件无改动并跳过；**禁止**只有 working 无 baseline）。
- [ ] CSV UTF-8-SIG；`schema` 与 burn_index 匹配。
- [ ] 所有 DAC / temp 字段可解析为整数；DAC ∈ int16。
- [ ] MCS：`sw` 0..33，`ch` 0..31；MEMS：`row_idx` 范围内。
- [ ] 危险 diff ↔ 解锁状态。
- [ ] 写盘后 Read 回校验已 patch 的槽位值一致（抽样或全量 patch 行）。

### 3.4 错误码（日志/状态栏机读前缀）

| Code | 含义 |
|------|------|
| `FE_OK` | 成功（可带 `files=N rows=M`） |
| `FE_NO_SN` | SN 未读全 / 路径无法构建 |
| `FE_BIN_MISSING` | 对应 role 的 bin 不存在 |
| `FE_EXPORT_FAIL` | 导出读写失败 |
| `FE_BASELINE_MISSING` | working 有改但无 baseline |
| `FE_CSV_PARSE` | 列/类型/schema 错误 |
| `FE_CSV_KEY` | 行键重复或非法 |
| `FE_DAC_RANGE` | 超出 int16 |
| `FE_DANGEROUS_LOCKED` | 危险行有 diff 未解锁 |
| `FE_DIFF_EMPTY` | 无任何 diff（提示非错误可选） |
| `FE_WRITE_FAIL` | Writer/CRC/磁盘失败 |
| `FE_VERIFY_FAIL` | 写后读回不一致 |
| `FE_PATH_CH` | 光路 CH 非法或 Mapping 歧义/缺失 |
| `FE_BUSY` | 主流程 Busy |

---

## 4. 规格：坏光路 → DAC 总结（能做到）

**结论：能做到。** IL Test 已有 CH→四级开关；缺的是合成 **CH → 四个 `FineTuneAddress`**。

### 4.1 映射算法（新建 `PathDacImpact`）

输入：`ch1to576`。

1. 复用 [IlTestMath.cpp](M576Calibrator/M576CalibratorApp/IlTestMath.cpp)：  
   `sw=(ch-1)/18+1`，`mcsCh=(ch-1)%18+1`，`pb=sw+32`；MPO 用 `IlTestChannelToMpoPath`。
2. **MCS1 / MCS2**：`FineTuneAddress{Mcs1|Mcs2, block=sw, ch=mcsCh}` → burn 0/1；LUT `swIdx=M576McsBlock1To32ToLutSwIdx0(block)`，`chIdx=mcsCh-1`。
3. **1x64_1 / 1x64_2**：在 `pm_1x64_1Mapping.csv` / `pm_1x64_2Mapping.csv` 中查找 **唯一** 行，使 path 端口对为 `(c1=sw, c4=pb)`（与 SmallRange/校准 path 约定一致）；取该行 `SW_x`/`CH_y` → `FineTuneAddress`；burn = `2+(SW-1)` 或 `6+(SW-1)`。  
   0 行 / 多行 → `FE_PATH_CH`。
4. UI 列表每行：`device, burn_index, {SN}_dac.csv, row key, LOW/ROOM/HIGH Y/X`（从当前 Role 的 bin 只读）。

### 4.2 与 FullEdit 的结合

- Path Impact **不自动改 CSV**；提供 **Copy keys** / **Export impact txt**，方便在 Excel 里定位那 4 个文件中的对应行。
- 文档写明：一条产品光路同时穿过 **两端 1×64 + 两侧 MCS 同 block/ch**；调坏路时通常要对照这 4 组 DAC（再按实测决定改哪一端）。

样例（与现网 Mapping 一致时）：CH1 → MCS1/2 block=1 ch=1；1x64_1 与 1x64_2 各一档 SW/CH_y（由 Mapping 决定）。

---

## 5. 实现落点（文件）

| 单元 | 职责 |
|------|------|
| `docs/superpowers/specs/2026-08-26-fulledit-bin-dac-design.md` | 本文规格正文 |
| `docs/superpowers/plans/2026-08-26-fulledit-bin-dac.md` | 分步实现计划（checkbox） |
| `FullEditDacTypes.h` / `FullEditDacIo.h|.cpp` | MCS/MEMS export、diff、import、危险行判定 |
| `PathDacImpact.h|.cpp` | CH→4×FineTuneAddress + Mapping 查找 |
| `M576FullEditDlg.h|.cpp` + `.rc` / `resource.h` | UI |
| `M576CalibratorDlg` | **仅**按钮、Busy 门闩、SN、打开/关闭 dlg；不改 Run Path/Merge/Burn 实现 |
| `CrossPeakTest`（可选） | PathDacImpact + CSV diff 单测；不必跑硬件 |

复用：`FineTuneBurnIndex`、`M576BuildBurnFilePaths`、`CLutBinWriter`、`CMems1x64LutBinWriter`、`IlTestMath`、`Pm1x64Mapping` / Mapping 加载路径（与 FineTune 相同）。

**不**把逻辑放进 Z4671Core（无 MFC/CSV 编排）；Writer 仍只负责 bin/CRC。

---

## 6. 实现任务顺序

1. **Spec 落盘** — 将本章写入 `docs/superpowers/specs/...`。
2. **PathDacImpact + CrossPeakTest** — CH1/CH70 固定期望（Mapping 用仓库内 csv）；歧义失败用例。
3. **FullEditDacIo MCS** — export/diff/import 单文件；危险行锁定；int16；写后校验。
4. **FullEditDacIo MEMS** — 同上，三温 CH+MID。
5. **CM576FullEditDlg** — Role、Export、Unlock、Validate&Write、Open folder、本地 log。
6. **Path Impact 面板** — CH 输入、列表、读当前 bin DAC、Copy/Export。
7. **主窗按钮 + 接线** — 放在 Make Bin/FineTune 旁；Busy/SN；**禁止**改动主流程函数体；session sync 默认关。
8. **用户文档短节** — 使用说明单独一节 **「研发调试工具（FineTune / Make Bin / FullEdit）」**；`INVARIANTS` 增：FullEdit 为旁路、可写全温区但危险行须解锁、**不得改变主流程 Merge/RECAL 行为**。

---

## 7. 验证

- CrossPeakTest：PathDacImpact、MCS/MEMS diff 锁定/解锁、DAC 越界。
- 手工：Read Bin → FullEdit Export → 改某 MCS LOW → Write → 仅该 `{SN}_*.bin` 变；未解锁改 SN33 → `FE_DANGEROUS_LOCKED`。
- Path：输入 CH1，列表 4 地址与 IL Test / Mapping 一致。
- **主流程回归**：未开 FullEdit 时，Run Path / Write BIN / Burn 行为与改前一致（代码 diff 审查：主流程文件无逻辑变更，仅新增按钮接线）。
- FullEdit 内无 Burn；默认不同步 session。
