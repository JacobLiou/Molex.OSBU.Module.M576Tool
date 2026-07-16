# 1310 寻峰 / 定标算法优化 TODO（讨论稿）

> **状态**：讨论中，**尚未立项实施**。  
> **背景**：1310 校准固件以 Flash 内 DAC（`RECAL` 中 `base=9999`）为扫频起点；产线大量测试表明部分通道 IL 仍不理想。当前上位机 RECAL 3/5 + `PeakFinder2D` 交叉寻峰与 `Peak1DSweepRecenter` 重试策略已较完整，继续微调三阶拟合公式对「初值偏远 / 弱耦合」类问题的收益可能有限。  
> **相关文档**：[`INVARIANTS.md`](INVARIANTS.md)（INV-10..19）、[`DEVELOPMENT_AND_CODE_GUIDE.md`](DEVELOPMENT_AND_CODE_GUIDE.md)、[`LUT_INDEXING.md`](LUT_INDEXING.md)  
> **相关代码**：`PeakFinder2D.cpp`、`Peak1DSweepRecenter.cpp`、`M576Peak1DConstants.h`、`M576CalibratorDlg.cpp`（`RunRecal1DSweepWithPeakRecenterRetry`）、`CrossPeakTest/`、`dataAnalysis/analyze_il_channels.py`

---

## 1. 问题 framing（讨论共识前提）

### 1.1 当前主路径不是「真 2D 格点搜索」

PM/PD Run Path 实际是：

1. `RECAL 0` 配置光源 / PM 档  
2. `RECAL 1` 路由  
3. `RECAL 3`（或 `5`）**mode 0**：定 X、扫 Y  
4. **mode 1**：定 Y@peak、扫 X  
5. `PeakCrossFrom1DScans`：两轴各自三阶 LSQ + 单峰校验，得到交叉 `(row, col)` → DAC  

因此「Peak2D 拐点算法」的疑问需拆成两类：

| 现象 | 更可能主因 | 上位机侧重 |
|------|------------|------------|
| 日志无峰 / `flat` / `StrictInc` / skip | Flash 初值偏、扫频窗口未盖住真峰 | recenter、扩 offset、显式 base |
| 有 `-> peak row=...` 但 IL 差 | 弱峰、旁瓣、低 Y-span、非全局最优交叉点 | 扫频密度、验收门限、难通道配置 |
| 重试打满仍差 | 物理耦合差或初值/窗口根本不对 | backup base、两趟定标、固件协同 |

### 1.2 已有上位机能力（改动前勿重复造轮）

- Flat / 单调贴边 / 扩 offset / base 平移（INV-10、INV-11）  
- Y 预扫 OK、交叉 Y 失败 → 重炉 RECAL 3/0（INV-19）  
- 最多 12 次 attempt（`M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS`）  
- PM：`RECAL 0` 后 `opm` 挡位核对（INV-17）  
- 离线回归：`CrossPeakTest`；comm 分析：`dataAnalysis/analyze_il_channels.py`

### 1.3 产线 comm 初步分型（待扩大样本）

参考 `comm_2026-06-17` 等设备 K6528198 分析：

- **寻峰失败型**：如 Step 565（`no peak` + `flat` + `StrictInc`）  
- **寻峰成功、IL 差型**：如 Step 568（有峰但 Y-span 极低，~9948 vs 正常 ~81875）  
- **高发区域**：如 2#MCS slot2（Step 187、565 等）——是否与光路/映射相关待讨论  

**待讨论**：坏通道中「失败 vs 成功但 IL 差」各占多少？决定优先投入 recenter 还是验收门限。

---

## 2. 优化项 backlog（按建议优先级）

每项标注：**状态**、**层级**（上位机 / 固件 / 产线流程）、**风险**、**依赖**。

### P0 — 数据分析与分型（零代码或脚本增强）

| ID | 事项 | 状态 | 说明 |
|----|------|------|------|
| **ALG-P0-01** | 坏通道自动分型报告 | 讨论中 | 扩展 `analyze_il_channels.py`：按步输出 `validate_code`、`trend`、`Y-span`、峰位贴边、retry 次数、最终 base/offset，并与 IL 关联 |
| **ALG-P0-02** | comm → CrossPeakTest 回放闭环 | 部分完成 | 上位机 Run Path 已实时写 `comm_*_recal_sweeps.csv`（含全部 retry）；`tools/extract_recal_sweep_csv.py` 仍用于**旧 comm 日志**回放；`CrossPeakTest --export-peak-csv` / `--mock-sweeps` 兼容新 `path` 列 |
| **ALG-P0-03** | 难通道清单维护 | 讨论中 | 产线维护「高发 step / slot / MCS 块」表，作为后续配置驱动与验收的输入 |

**讨论点**：分型维度是否足够？是否需要按 `(fileSlot, sw, ch)` 而不仅是 path step index？

---

### P1 — 扫频参数与 UI 默认值（低风险，可小范围实机试）

| ID | 事项 | 状态 | 说明 |
|----|------|------|------|
| **ALG-P1-01** | 难通道 `dacRange` / `dacStep` / `delayMs` 矩阵实验 | 讨论中 | 对固定坏 step 做小矩阵扫描；注意 `M576_MAX_RECAL_SWEEP_READ_MS` 超时 |
| **ALG-P1-02** | 默认 UI 参数产线建议值文档化 | 讨论中 | 在通过 P0 分析后，写入用户手册或本 TODO 附录 |
| **ALG-P1-03** | 精扫点数与三阶拟合稳定性 | 讨论中 | `dacStep` 减小 → 点数增多；与 `M576_PEAK1D_CUBIC_MIN_SAMPLES`、fine refine 关系 |

**讨论点**：是否允许「全局 UI 参数」与「难通道覆盖」并存？见 P2-02。

---

### P2 — 显式 base 与通道级配置（中等改动，收益预期高）

| ID | 事项 | 状态 | 说明 |
|----|------|------|------|
| **ALG-P2-01** | Retry 时用 Backup LUT DAC 作显式 `baseY`/`baseX` | 讨论中 | Read Bin 后从 `backupAll1310DAC.csv` / session LUT 取 `(sw,ch)` DAC；失败重试时不用仅 `9999`，减少依赖 Flash 初值 |
| **ALG-P2-02** | 难通道配置表（CSV/JSON） | 讨论中 | 列示例：`step_index, coarse_range, fine_range, delay_ms, force_base_y, force_base_x, max_extra_retry`；Run Path 读表覆盖 UI |
| **ALG-P2-03** | 邻道 / 同 SW 插值初值（慎用） | 讨论中 | 初值来自相邻已校准通道；须符合 INV-06 稀疏 merge，避免污染未校准槽位 |

**讨论点**：

- P2-01 首轮是否仍用 `9999`，还是坏通道列表内首轮即用 backup？  
- P2-02 配置谁维护（工艺 / 软件）？是否进版本管理？  
- P2-03 是否接受「借用邻道」带来的系统性风险？

**入口代码（实施时）**：`RunRecal1DSweepWithPeakRecenterRetry` 的 `initialMovingBase`；PM/PD 主循环中 `M576_RECAL_FW_READ_BASE_DAC` 的使用处。

---

### P3 — 算法门限微调（须 CrossPeakTest + INV 同步）

| ID | 事项 | 状态 | 说明 |
|----|------|------|------|
| **ALG-P3-00** | PeakFinder 峰突出度 + 对称拟合窗 + INI 可配 `MinProminenceDb` | **已实施** | `M576_PEAK1D_MIN_PROMINENCE_DB`、`Peak1DGetMinProminenceDb`、`M576Calibrator.ini`；CrossPeakTest 回归全绿 |
| **ALG-P3-01** | 弱峰通道：`M576_PEAK1D_MIN_SPAN_DB` / `FLAT_REL_SPAN_FRAC` | 讨论中 | 低 span 仍接受峰 → 可能增加 recenter；过低则误接受噪声峰 |
| **ALG-P3-02** | 多峰 / 旁瓣：outlier 与 `MAX_STRICT_LOCAL_MAXIMA` | 讨论中 | `OUTLIER_MULT`、`OUTLIER_LOCAL_HALF` 等 |
| **ALG-P3-03** | Recenter 激进度：`SWEEP_RECENTER_*_FRAC` 系列 | 讨论中 | `TSTAR_WEIGHT`、`STAGNATION_GAIN` 等对贴边通道的影响 |
| **ALG-P3-04** | 交叉峰策略：是否引入 2D 邻域能量和（非当前主路径） | 讨论中 | 当前为两轴 1D 交叉；真 2D 曲面最优需评估算力与固件扫频模型 |

**约束**：改 `M576Peak1DConstants.h` 必须跑 `CrossPeakTest` Release\|Win32；涉及 Flat/mono 语义须对照 INV-10/11，更新 `INVARIANTS.md` 与 `.cursor/rules/m576-peak-recal.mdc`。

**讨论点**：P3 是否在 P2（显式 base）见效后再做，避免调参掩盖初值问题？

---

### P4 — 定标后验收门限（PRD FR-07 对齐，产线兜底）

| ID | 事项 | 状态 | 说明 |
|----|------|------|------|
| **ALG-P4-01** | 峰成功 ≠ 合格：单步功率 / IL 门限 | 讨论中 | 交叉峰成功后复核功率或 IL；超差则不写 LUT，记 `CalibPathFailCategory` 新类别 |
| **ALG-P4-02** | 重复性测试（FR-07） | 讨论中 | PRD 要求校准后重复性 OK 才烧录；当前 App 侧待确认完整度 |
| **ALG-P4-03** | Write Bin 前坏步汇总拦截 | 讨论中 | 与 `M576RunPathSummaryDlg`、failure CSV 联动；可选「有失败则禁止烧录」策略 |

**讨论点**：IL / 重复性阈值与次数以何规格书为准？单步失败是 skip 还是整路 path 失败？

---

### P5 — Session 策略（流程级）

| ID | 事项 | 状态 | 说明 |
|----|------|------|------|
| **ALG-P5-01** | 两趟定标 | 讨论中 | 第一趟写好通道 → Write/Burn（或 RAM 生效）→ 第二趟专跑坏通道子集，利用更新后 Flash 初值 |
| **ALG-P5-02** | Diagnosis 模式用于挂测复核 | 讨论中 | 现有 `DiagnosisSession` 是否扩展为定标后抽检 |
| **ALG-P5-03** | 坏通道子路径 CSV | 讨论中 | 仅重跑 `path_bad_steps.csv` 列出的步，缩短产线时间 |

---

### P6 — 固件 / 硬件边界（上位机单独无法闭合）

| ID | 事项 | 状态 | 说明 |
|----|------|------|------|
| **ALG-P6-01** | `9999` 读 DAC 时机与一致性 | 需固件确认 | 初值质量上限由 Flash 与固件语义决定 |
| **ALG-P6-02** | 固件侧更宽扫频或真 2D 扫 | 需固件评估 | X/Y 强非正交时，两次 1D 交叉非全局最优 |
| **ALG-P6-03** | MEMS settle、功率计档位 | 需硬件/固件 | 与 INV-16/17、delay 参数相关 |

---

## 3. 建议实施顺序（讨论用，非承诺）

```text
P0 分型与回放 → P1 扫频参数实机矩阵 → P2 backup base / 难通道配置
    → P4 验收门限（与产线规格对齐）→ P3 门限微调（有 comm 证据再做）
    → P5 两趟定标 → P6 固件协同（并行跟踪）
```

**原则**（与 `m576-global.mdc` 一致）：

- 产线目标：尽可能让每次校准成功，而非边界数据上轻易 skip。  
- 可重试、可恢复、可追溯、可单测（`CrossPeakTest`）。  
- 不掩盖固件缺口：注释或 INVARIANTS 写清「上位机补什么、固件仍须保证什么」。

---

## 4. 开放问题（会议讨论清单）

1. 坏通道主因占比：寻峰失败 vs 寻峰成功 IL 差？  
2. 是否接受「难通道配置表」长期维护？维护方与变更流程？  
3. Retry 用 backup DAC 作 base：首轮 `9999` 是否保留？  
4. IL / 重复性合格标准（数值、次数）以哪份规格为准？  
5. 单步验收失败：skip、重试 N 次、还是停整路 path？  
6. 2#MCS slot2 等高发区：光路/映射/固件是否有已知系统性原因？  
7. 两趟定标是否可接受产线节拍增加？第二趟是否必须烧录中间 bin？  
8. 是否在 P2 见效前冻结 P3 大范围门限改动，避免 overfit 单台设备 comm？

---

## 5. 实施前检查（将来立项时）

- [ ] P0 报告对代表设备（≥N 台）复现坏通道分型  
- [ ] `CrossPeakTest` 全绿 + 坏步 comm 回放对比  
- [ ] 改动触及 INV-10..19 时更新 [`INVARIANTS.md`](INVARIANTS.md)  
- [ ] 实机小批量验证 IL 分布与节拍  
- [ ] 用户手册 / 产线 SOP 同步（若改 UI 或流程）

---

## 6. 修订记录

| 日期 | 说明 |
|------|------|
| 2026-06-18 | 初稿：基于 1310 产线 IL 与 comm 分析讨论整理，状态均为「讨论中、未实施」 |
