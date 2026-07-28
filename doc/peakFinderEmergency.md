# 寻峰算法紧急回退 SOP（peakFinderEmergency）

产线寻峰算法在实机 IL 不达标时的**版本对照、回退触发条件、手术式回退步骤**与复测记录。与 2026-07-08 IL 事故及 [`IL_RunPath_log_audit_2026-07-08.md`](IL_RunPath_log_audit_2026-07-08.md) 配套使用。

**原则**：只回退寻峰/扫频 pipeline，**不** `git reset --hard` 整个仓库；archive、烧录、Run Path、Z4671 bin 等非算法功能保留。

---

## 1. Git 版本对照

| 层级 | Commit | 日期 | 含义 | 何时用 |
|------|--------|------|------|--------|
| **L0 完整回退基线** | `0f80f0f` | archive 功能提交 | **无** `Peak1DSweepPipeline`、**无** stitch、**无** mesa/`dual_knee`；`RunRecal1DSweepWithPeakRecenterRetry` + `Peak1DSweepRecenter` 启发式重试 | **用户指定：无拼接、无平台峰判定** |
| L1 拼接引入 | `7cfa313` | 2026-07-06 | 新增 `Peak1DSweepPipeline`、`MergeRecal1DSweepSegments`、[`doc/拼接式扫频重试算法.md`](../../doc/拼接式扫频重试算法.md) | 仅保留 stitch、不要 mesa 时参考 |
| L2 平台峰/mesa | `7de6e2e` | 2026-07-07 | `IsMergedMesaProfile`、`dual_knee` | 07-08 高 IL 事故算法链起点 |
| HEAD | `3fdb165` | 当前 tip | fineRefine 失败 deferred stitch 等 | 开发主线 |

查看命令：

```text
git log --oneline -S "IsMergedMesaProfile" -- M576Calibrator
git log --oneline -S "Peak1DSweepPipeline" -- M576Calibrator
git show 0f80f0f --stat
```

---

## 2. 回退触发条件（实机复测后勾选）

满足**任一条**即建议执行 L0 回退（`0f80f0f` 寻峰文件状态）：

- [ ] IL > 0.2 的路径占比相对 07-08 **未明显改善**，且 comm 仍出现 `skip fineRefine` 或离谱 Y DAC（如 `|DAC|>300` 且与 merge 中心偏差 >100）
- [ ] slot2 mesa 路径仍 **Succeeded 直落盘**（mesa 修复未生效或回归）
- [ ] FATAL / `peak-pipeline-exhausted` 率 > **5%** path（可按产线调整阈值）

**不满足**上述条件时：保留当前算法（含 mesa → fineRefine + DAC 门控修复），仅做小步优化与 comm 回放回归。

---

## 3. L0 回退时保留的功能

以下在 `0f80f0f` 已存在，回退寻峰后**自动保留**（勿误删）：

| 功能 | 相关模块/提交 |
|------|----------------|
| output/archive 会话目录 | `0f80f0f` archive |
| Burn Board / Burn Flash | `ee68e61`、`572313a` |
| Run Path 总览弹框 | `d854f98` |
| SN 命名 bin | `477f4f3` |
| opm `4 1` / `5 1` 挡位校验 | `c8841af`、`b56d0b5` |
| MCS 低温合并策略 | `dd7533b` |
| CalibPathOutcome / FATAL 日志 | Dlg 层保留 |
| Z4671Core 读/写/烧 bin | 全期 |

---

## 4. L0 回退时移除的算法能力

| 移除项 | 文件/符号 |
|--------|-----------|
| Stitch k0..k4 | `Peak1DSweepPipeline.cpp` |
| `MergeRecal1DSweepSegments` | 同上 |
| `IsMergedMesaProfile` / `dual_knee` | `PeakFinder2D.cpp` |
| `ValidateMergedYPowerAtPeak`（mesa 门控） | `PeakFinder2D.cpp` |
| mesa 直成功 / skip fineRefine | L0 无 mesa 概念 |

---

## 5. L0 手术式回退步骤

### 5.1 回退前快照

```powershell
cd <repo-root>
git stash push -m "pre-peak-rollback-$(Get-Date -Format yyyyMMdd)" -- `
  M576Calibrator/M576CalibratorApp/Peak1DSweepPipeline.cpp `
  M576Calibrator/M576CalibratorApp/Peak1DSweepPipeline.h `
  M576Calibrator/M576CalibratorApp/PeakFinder2D.cpp `
  M576Calibrator/M576CalibratorApp/PeakFinder2D.h `
  M576Calibrator/M576CalibratorApp/Peak1DSweepRecenter.cpp `
  M576Calibrator/M576CalibratorApp/Peak1DSweepRecenter.h `
  M576Calibrator/M576CalibratorApp/M576CalibratorDlg.cpp `
  M576Calibrator/M576CalibratorApp/M576CalibratorDlg.h `
  M576Calibrator/M576CalibratorApp/M576Peak1DConstants.h `
  M576Calibrator/CrossPeakTest/main.cpp `
  M576Calibrator/Docs/INVARIANTS.md
```

### 5.2 从基线检出寻峰文件

```powershell
git checkout 0f80f0f -- `
  M576Calibrator/M576CalibratorApp/Peak1DSweepRecenter.cpp `
  M576Calibrator/M576CalibratorApp/Peak1DSweepRecenter.h `
  M576Calibrator/M576CalibratorApp/PeakFinder2D.cpp `
  M576Calibrator/M576CalibratorApp/PeakFinder2D.h `
  M576Calibrator/M576CalibratorApp/M576Peak1DConstants.h `
  M576Calibrator/M576CalibratorApp/M576CalibratorDlg.cpp `
  M576Calibrator/M576CalibratorApp/M576CalibratorDlg.h
```

### 5.3 删除拼接 pipeline

1. 删除 `M576Calibrator/M576CalibratorApp/Peak1DSweepPipeline.cpp`、`.h`
2. 从 `M576CalibratorApp.vcxproj`、`CrossPeakTest.vcxproj` 移除对应 `ClCompile` / `ClInclude`
3. 确认 `Peak1DSweepRecenter.cpp` 无 `#include "Peak1DSweepPipeline.h"`

### 5.4 CrossPeakTest

```powershell
git checkout 0f80f0f -- M576Calibrator/CrossPeakTest/main.cpp
```

再手工删除仍引用 `Peak1DSweepPipeline` 的用例（若冲突，以能编译为准）。

### 5.5 文档

- `INVARIANTS.md` INV-10：恢复 `0f80f0f` 版启发式 planner 描述
- `PeakFinder.md`：标注「当前生产 = L0 回退」
- **保留** `IL_RunPath_log_audit_2026-07-08.md`（历史证据）

### 5.6 验证

```text
msbuild M576Calibrator\CrossPeakTest\CrossPeakTest.vcxproj /p:Configuration=Release /p:Platform=Win32
M576Calibrator\CrossPeakTest\Release\CrossPeakTest.exe
msbuild M576Calibrator\M576CalibratorApp\M576CalibratorApp.vcxproj /p:Configuration=Release /p:Platform=Win32
```

实机抽测：3 条曾高 IL 路径 + 3 条正常路径 → 再测 IL。

---

## 6. 回退后行为差异

| 项目 | L0（0f80f0f） | 当前 HEAD（2026-07-13 起无 mesa） |
|------|---------------|----------------------------------|
| coarse@200 失败后 | 启发式 planner 重试/平移 | Stitch k1..k4 拼接 |
| 三段 merge 定粗位 | 无 stitch | **FineRefineRelaxed 单峰** + fineRefine@64 |
| 难路径 slot2 Flat | 更多重试或整轴失败 | k=2 对称三段 merge relaxed |
| IL 风险（07-08） | 无错 DAC 直写，但成功率可能更低 | 已移除 dual_knee；避免 -463/-495 类错 DAC |

---

## 7. 复测分析与决策记录

### 7.1 基准数据（2026-07-08，旧算法）

| 项目 | 结论 |
|------|------|
| 数据源 | `comm_2026-07-08.log`、`comm_2026-07-08_recal_sweeps.csv`、`验证IL.png` |
| IL>0.2 | 15 条；约 10 条命中 mesa bypass |
| 根因 | mesa 直成功 + 错 DAC + cross Y=0，非单纯 1D 拟合错误 |
| 详细映射 | [`IL_RunPath_log_audit_2026-07-08.md`](IL_RunPath_log_audit_2026-07-08.md) |

### 7.2 新一轮实机数据（待填入）

| 文件 | 路径（示例） | 状态 |
|------|--------------|------|
| comm log | `comm_YYYY-MM-DD.log` | **待提供** |
| sweep CSV | `comm_YYYY-MM-DD_recal_sweeps.csv` | **待提供** |
| IL 验证 | Excel 或 `验证IL.png` | **待提供** |

已有辅助文档：

- [`IL_RunPath_map_2026-07-09.md`](IL_RunPath_map_2026-07-09.md)（**CSV/log 未入库**）
- [`IL_RunPath_retest_baseline_2026-07-10.md`](IL_RunPath_retest_baseline_2026-07-10.md)（复测前基线与决策）

### 7.3 新数据分析清单

1. IL Excel：标 MAX-MIN > 0.2；`step = (ch1-1)*18 + ch2`；分 slot1/slot2
2. comm log：搜 `symmetric trio merge relaxed`、`fineRefine`、`[FATAL]`、`Base DAC`、`交叉 Y轴.*功率=0`
3. sweep CSV：高 IL slot2 step 跑 CrossPeakTest comm 回放
4. 输出：`IL_RunPath_map_YYYY-MM-DD.md` + 与 07-08 对比
5. 勾选 §2 → 决定是否执行 §5

### 7.4 当前算法验证（mesa 已移除，2026-07-13）

| 项 | 状态 |
|----|------|
| mesa / dual_knee | **已删除** |
| 三段 merge | **FineRefineRelaxed 单峰** + `DacAtMergedSampleIndex` |
| k=2 对称三段 | `lastStitchSymmetricTrioMerge` 豁免 Strict |
| CrossPeakTest comm 回放 494/551/241/444/529/332 | `RunComm20260708Slot2IlStitchMergeReplayTests` |

### 7.5 决策（2026-07-10）

| 决策项 | 结果 |
|--------|------|
| 是否执行 L0 回退 | **否** — 新 comm/sweep/IL 尚未提供 |
| 下一步 | 实机复测 → 填 §7.2 → 再勾选 §2 |
| L0 回退执行记录 | **未执行**（见 §8） |

---

## 8. 复测记录表

| 日期 | Git commit / 构建 | IL 异常 path 数 | mesa 直成功 | FATAL 率 | 决策 |
|------|-------------------|-----------------|-------------|----------|------|
| 2026-07-08 | 修复前（`7de6e2e` 链） | 15 / 21 采样 | 有 | — | 定位 pipeline 根因 |
| 2026-07-10 | mesa→fineRefine 修复 + CrossPeakTest exit=0 | （待新 IL） | （待新 log） | （待新 log） | **暂不回退；L0 未执行** |
| 2026-07-13 | **移除 mesa**；merge 统一 Relaxed 单峰 | （待新 IL） | 无 | （待新 log） | 待实机复测 |
| | | | | | |

---

## 9. 历史说明（mesa 已废弃）

2026-07-13 起产线算法**不再使用** `IsMergedMesaProfile` / `dual_knee`。三段 merge 统一 `FineRefineRelaxed` 单峰 + fineRefine@64。历史 mesa 事故分析见 [`IL_RunPath_log_audit_2026-07-08.md`](IL_RunPath_log_audit_2026-07-08.md)。

---

## 10. 相关文档

- [`INVARIANTS.md`](INVARIANTS.md)
- [`PeakFinder.md`](PeakFinder.md)
- [`IL_RunPath_log_audit_2026-07-08.md`](IL_RunPath_log_audit_2026-07-08.md)
- [`IL_RunPath_retest_baseline_2026-07-10.md`](IL_RunPath_retest_baseline_2026-07-10.md)
- [`doc/拼接式扫频重试算法.md`](../../doc/拼接式扫频重试算法.md)