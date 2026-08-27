---
name: Path Impact Six Stages
overview: 升级 FullEdit Path Impact：CH 下拉（含 MPO）选中即刷新；按 1×64 叶子级联展开为六级（两侧上级+叶子 + 两 MCS）；ListCtrl + 中文摘要 + 六格横向卡片；并补 CrossPeakTest。
todos:
  - id: path-six-logic
    content: PathDacImpact：ParentFromLeaf + 固定 6 slots + 中文摘要 API
    status: completed
  - id: path-six-tests
    content: CrossPeakTest：直通/级联/ParentFromLeaf 用例
    status: completed
  - id: path-six-ui
    content: FullEditDlg：CH Combo、摘要、六格卡片、ListCtrl、Copy/Export
    status: completed
  - id: path-six-docs
    content: 更新 fulledit design spec Path Impact 节
    status: completed
isProject: false
---

# FullEdit Path Impact 六级形象展示

**Goal:** 把 FullEdit 的 Path Impact 从「文本框 CH + 四段乱文本」改成：Combo 选 CH、六级开关可视化（与 [设备开关图.png](doc/设备开关图.png) / PRD `1x64 Switch_1/2` 一致）。

**Default (直通口):** 固定 **6 行**。叶子 `SW_x==1` 时上级=叶子同一槽，叶子行角色标 **「直通（无级联）」**（不隐藏行、不缩成 4 行）。

## 拓扑推导（核心）

现有 [PathDacImpact.cpp](M576Calibrator/M576CalibratorApp/PathDacImpact.cpp) 每侧 1×64 只取 Mapping **叶子**一行。补 **上级**：

```text
leaf = Mapping 唯一行 (c1=opticalPort, c4=peer)
if leaf.SW == 1: parent = leaf          // 直通 CH01–14 类
if leaf.SW == 2: parent = {SW=1, CH_y=8} // SW2-COM
if leaf.SW == 3: parent = {SW=1, CH_y=9} // SW3-COM
if leaf.SW == 4: parent = {SW=1, CH_y=10}// SW4-COM
```

与仓库 [pm_1x64_1Mapping.csv](M576Calibrator/output/pm_1x64_1Mapping.csv) 中 SW1 的 CH_y=8/9/10（口 15/32/49）一致。

六级顺序（光走向）：

```mermaid
flowchart LR
  A1["1x64_1 parent"] --> A2["1x64_1 leaf"]
  A2 --> M1["MCS1"]
  M1 --> M2["MCS2"]
  M2 --> B1["1x64_2 parent"]
  B1 --> B2["1x64_2 leaf"]
```

| # | 角色中文 | 地址来源 |
|---|----------|----------|
| 1 | 1#1×64 上级 | parent from map1 |
| 2 | 1#1×64 叶子 | leaf map1（直通则同上级并标注） |
| 3 | 1#MCS | block/ch 公式不变 |
| 4 | 2#MCS | 同上 |
| 5 | 2#1×64 上级 | parent from map2 |
| 6 | 2#1×64 叶子 | leaf map2 |

`burnIndex` / CSV 文件名仍用 `FineTuneBurnIndex` + `M576SnForBurnFileIndex`（上级与叶子常对应 **不同** `{SN}_dac.csv`）。

## 数据层改动

文件：[PathDacImpact.h](M576Calibrator/M576CalibratorApp/PathDacImpact.h) / [.cpp](M576Calibrator/M576CalibratorApp/PathDacImpact.cpp)

- `PathDacImpactSlot` 增加：`roleZh`、`stageKind`（Parent/Leaf/Mcs）、`isDirectPass`、`csvFileHint`
- `slots` 固定 **size==6**
- 新增 `PathDacImpactParentFromLeaf(leafSw, leafChY, …)`（纯函数，可单测）
- `PathDacImpactFormatSummaryZh(r)` → 顶部一行中文（CH、MPO、口对、是否级联）
- 保留/弱化纯文本 `FormatText`（导出 impact 文件仍可用结构化摘要+六行表格式）

## UI（[M576FullEditDlg](M576Calibrator/M576CalibratorApp/M576FullEditDlg.cpp) + .rc）

- **CH：** `IDC_FE_EDIT_CH` 改为 `CComboBox`（`CBS_DROPDOWNLIST`）；Init 填 576 项：`CH{n}  {MPO}`（`IlTestChannelToMpoPath`）；`CBN_SELCHANGE` → 自动 Resolve（可去掉强制点 Resolve，按钮改「刷新 DAC」）
- **摘要：** 新增只读 `IDC_FE_STATIC_SUMMARY`（或单行 EDIT）显示 `FormatSummaryZh`
- **ListCtrl：** 替换大段 `IDC_FE_EDIT_IMPACT` 为 `LVS_REPORT` 六行列：`级 | 角色 | 设备 | CSV/SN | 槽位 | LOW Y | LOW X`；从当前 Role bin `FineTuneReadCurrentDac` 填 Y/X
- **六格卡片：** 摘要下方一排 6 个小静态区（或 6×`GROUPBOX` 缩略）：角色名 + `SW/ch` + `Y/X`；与 List 同步，选 List 行时高亮对应卡片（`SetWindowText` / 粗体静态即可，不自绘拓扑图）
- **Copy keys / Export impact：** 改为导出「摘要 + 六行 TSV/文本」（含上级/叶子），逻辑仍写 `path_impact\CH{nnn}_impact.txt`

布局：Path Impact 组加高以容纳 Combo + 摘要 + 卡片条 + List；Local log 下移。

## 测试

[CrossPeakTest/main.cpp](M576Calibrator/CrossPeakTest/main.cpp) 扩展：

- `ParentFromLeaf`：SW2→(1,8)，SW3→(1,9)，SW4→(1,10)，SW1→自身
- CH1：六槽；两侧 1×64 parent==leaf 且 `isDirectPass`
- CH70（口 4 / 36）：若 Mapping 叶子非 SW1，则 parent 为 SW1 CH_y 8/9/10；`slots.size()==6`

## 文档

短更 [2026-08-26-fulledit-bin-dac-design.md](docs/superpowers/specs/2026-08-26-fulledit-bin-dac-design.md) Path Impact 节：四→六、上级推导规则、直通标注。

## 非目标

- 不改 Run Path / Merge / Burn
- 不改 FullEdit CSV export/write
- 不嵌入 `物理拓扑图.png` 位图高亮
