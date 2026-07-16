# 复测分析基线（2026-07-10）

在**新一轮 comm log / sweep CSV / IL Excel 入库前**，基于已有材料整理的对比基线与判定结论。完整 SOP 见 [`peakFinderEmergency.md`](peakFinderEmergency.md)。

---

## 1. 数据可用性

| 数据集 | 文件 | 状态 |
|--------|------|------|
| 07-08 事故跑 | `comm_2026-07-08.log`、`comm_2026-07-08_recal_sweeps.csv`、`验证IL.png` | 已入库 |
| 07-08 审计 | [`IL_RunPath_log_audit_2026-07-08.md`](IL_RunPath_log_audit_2026-07-08.md) | 已完成 |
| 07-09 映射 | [`IL_RunPath_map_2026-07-09.md`](IL_RunPath_map_2026-07-09.md) | 仅有 MD/JSON，**无** comm_2026-07-09 源文件 |
| 新一轮实机 | `comm_YYYY-MM-DD.*`、IL Excel | **待用户提供** |

---

## 2. 07-08 基准结论（旧算法）

- IL > 0.2：**15 / 21** 采样路径
- comm 中 `mesa bypass`：**16** 次
- 高 IL 与 slot2 mesa 直成功 + 错 Y DAC + cross Y 功率=0 强相关
- IL ≤ 0.2 的 6 条：无 mesa，4 sweep 全 peak_ok=1

---

## 3. 当前算法修复 + CrossPeakTest

| 修复项 | 预期 |
|--------|------|
| dual_knee → fineRefine | 不再 skip fineRefine 直成功 |
| DacAtMergedSampleIndex | pendingFineBase 远离 -463 等 |
| ValidateMergedYPowerAtPeak | 无效 merge Y 不落 cross |

**验证（2026-07-10）**：`CrossPeakTest.exe` exit=0（含 `RunComm20260708Slot2IlMesaReplayTests`）。

---

## 4. 新数据对比模板

| 指标 | 07-08 旧算法 | 新跑 YYYY-MM-DD | 判定 |
|------|-------------|-----------------|------|
| IL>0.2 path 数 | 15 | | |
| skip fineRefine | 有 | | 应为 0 |
| 错 DAC | 有 | | 应为 0 |
| FATAL 率 | — | | 建议 <5% |

---

## 5. 决策（2026-07-10）

- **L0 回退（0f80f0f）**：**不执行** — 新 IL/log 未入库；pipeline 修复 + 回放已通过
- **下一步**：提供新 comm / sweep / IL → 更新 peakFinderEmergency §7.2、§8