# M576 定标不变量（INVARIANTS）

本文档列出**不得随意破坏**的约定，用于代码评审、与固件/产线对表、以及 Cursor AI 改码前自检。细节下标见 [`LUT_INDEXING.md`](LUT_INDEXING.md)。

**维护**：改 Merge / RECAL / Read Flash 行为时，先检查是否违反下列条目，并同步更新本文与相关 `.cursor/rules/`。

---

## 1. LUT / BIN 写入

| ID | 不变量 |
|----|--------|
| **INV-01** | 1310 定标结果只写入 `wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][0/1]`（`IDX_TEMP_LOW == 0`）。不得在未走 1550 流程时改写 ROOM/HIGH。 |
| **INV-02** | 1310 Merge / MakeBin 仅覆盖 **sw 0..31**（`M576_MCS_LUT_SW_MERGE_COUNT == 32`）。**不得**用 session 覆盖 **sw 32、33**（对外 SN33/SN34，1×64 镜像槽）。 |
| **INV-03** | 路径 CSV 中 MCS 通道为 **c2/c3 或 PD ch2：1..18**；对应 LUT **ch 0..17**。未参与本次 Run Path 的 **ch 18..31**（对外 CH19–CH32）须保留 backup，禁止被 session 全表 merge 清零。 |
| **INV-04** | DAC 为有符号 int16，合法范围 `M576_RECAL_DAC_MIN`..`M576_RECAL_DAC_MAX`（-32768..32767）。**禁止**用 `if (dac < 0) dac = 0` 代替钳位。 |
| **INV-05** | `MergeLut1310LowTempSlot` 当前会同步 `wTemperaturePoint[sw][IDX_TEMP_LOW]` 与 `pchCalibDate`；若 session 未填元数据，Write BIN 前须明确是否应从 backup 保留（见实现与 PRD）。 |
| **INV-06** | **目标态（稀疏 merge）**：仅更新 session 中已校准的 `(sw, ch)`（建议以 `PushCalibStatRow` / 校准统计为准）；禁止对全 `ch < 32` 无差别拷贝 session。 |

---

## 2. Read Flash / 对表

| ID | 不变量 |
|----|--------|
| **INV-20** | `McsFwTransport::ReadLutBundleOnCurrentTunnel` 经 0xC4 读入完整 `stLutSettingZ4671` payload；Read 路径**不会**单独将 sw 32/33 置零。 |
| **INV-21** | `M576_FLASH_LUT_READ_BASE` 默认 `0`（`CalibConstants.h`）；若固件实际为 `0x65000` 等，须固件确认后改宏并记录版本，否则 Read 与设备不一致。 |
| **INV-22** | 若 backup/Read 结果中 **SN33/SN34 的 LOW 为 0 且 ROOM/HIGH 非 0**，优先按「1310 不写 sw 32/33 低温槽」或历史 bin 布局理解，再查上位机是否曾用 `sw<34` 全表 merge（历史 bug）。 |

---

## 3. RECAL / 寻峰

| ID | 不变量 |
|----|--------|
| **INV-10** | `profile.trend == Flat` 时 **`IsRetryablePeakFailure` 必须为 false**（平扫不重试）。 |
| **INV-11** | `StrictInc` / `StrictDec` 下多种失败码可重试；`ParabolaNotDownward` + `NonMono` 仅当 argmax 贴边（index ≤1 或 ≥ n-2）可重试。 |
| **INV-12** | 寻峰 recenter 失败时，须至少尝试一次 `SuggestSweepRecenterNewBase` 再放弃该轴；不得未重试直接 skip 下一轴扫（通信失败除外）。 |
| **INV-13** | 重试 `newBase` 由 `SuggestSweepRecenterNewBase` 钳位到 int16 范围，**不得**将负值抹为 0。 |
| **INV-14** | 固件无效功率占位仅认 `M576_RECAL_POW_INVALID_1` (-999999.0) 与 `M576_RECAL_POW_INVALID_2` (-999900.0)，拟合前剔除。 |
| **INV-15** | 定标步骤失败须带可机读 `Peak1DValidateCode`（及 trend/col0/attempts 等）；禁止仅靠无码日志判成败。 |
| **INV-16** | **PM / RECAL 3 only**：有效样本全局极大值 `dBm = raw/10000` 须在界面 `pm_range` 0..3 对应区间内；`pm_range==4`（auto）跳过。失败码 `PmRangeMismatch`，**不重试** recenter，丢弃该 path 步。PD（RECAL 5）不做挡位校验。 |
| **INV-17** | **PM Run Path only**：`RECAL 0` 成功后须发 `opm 4 1`，读回挡位（单行数字 0..4）须与界面/RECAL 0 的 `pm_range` 一致（`pm_range==4` auto 仅记录读回、不比对）。不一致或通信/解析失败：**日志 + 弹窗 + 立即停止**整次 Run Path，不进入路径 CSV。 |

---

## 4. 架构与通信

| ID | 不变量 |
|----|--------|
| **INV-30** | Z4671Core 不依赖 MFC 对话框；bin 布局以 `Z4767StructDefine.h` 为准，不擅自改 `stLutSettingZ4671` 成员布局。 |
| **INV-31** | 二进制 MCS 访问必须先 `trans n`，结束 `$$`；ASCII RECAL 与透传勿并发抢占同一串口无超时。 |
| **INV-32** | 所有串口/Flash IO 须有超时与明确失败返回；禁止 UI 线程长时间阻塞。 |

---

## 5. 验证清单（改码后）

- [ ] 动 Merge / Write BIN：对照 Z4671 导出 CSV，检查 **SN33/34、CH19–32** 是否与 backup 一致（未校准则应不变）。
- [ ] 动寻峰/重试：运行 `CrossPeakTest.exe`（Release\|Win32）。
- [ ] 动 Read：确认 `M576_FLASH_LUT_READ_BASE` 与固件一致。

---

## 6. 相关文档与规则

| 资源 | 路径 |
|------|------|
| 下标对照 | [`LUT_INDEXING.md`](LUT_INDEXING.md) |
| 架构 | [`DEVELOPMENT_AND_CODE_GUIDE.md`](DEVELOPMENT_AND_CODE_GUIDE.md) |
| Cursor | `.cursor/rules/m576-lut-bin.mdc`、`m576-peak-recal.mdc` |
