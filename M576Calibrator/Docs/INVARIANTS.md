# M576 ���겻������INVARIANTS��

���ĵ��г�**���������ƻ�**��Լ�������ڴ���������̼�/���߶Ա����Լ� Cursor AI ����ǰ�Լ졣ϸ���±�� [`LUT_INDEXING.md`](LUT_INDEXING.md)��

**ά��**���� Merge / RECAL / Read Flash ��Ϊʱ���ȼ���Ƿ�Υ��������Ŀ����ͬ�����±�������� `.cursor/rules/`��

---

## 1. LUT / BIN д��

| ID | ������ |
|----|--------|
| **INV-01** | 1310 ������ֻд�� `wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][0/1]`��`IDX_TEMP_LOW == 0`����������δ�� 1550 ����ʱ��д ROOM/HIGH�� |
| **INV-02** | 1310 Merge / MakeBin ������ **sw 0..31**��`M576_MCS_LUT_SW_MERGE_COUNT == 32`����**����**�� session ���� **sw 32��33**������ SN33/SN34��1��64 ����ۣ��� |
| **INV-03** | ·�� CSV �� MCS ͨ��Ϊ **c2/c3 �� PD ch2��1..18**����Ӧ LUT **ch 0..17**��δ���뱾�� Run Path �� **ch 18..31**������ CH19�CCH32���뱣�� backup��`MergeLut1310LowTempSlot` **��ʵ��**���ϲ� ch 0..17�� |
| **INV-04** | DAC Ϊ�з��� int16���Ϸ���Χ `M576_RECAL_DAC_MIN`..`M576_RECAL_DAC_MAX`��-32768..32767����**��ֹ**�� `if (dac < 0) dac = 0` ����ǯλ�� |
| **INV-05** | `MergeLut1310LowTempSlot` ��ǰ��ͬ�� `wTemperaturePoint[sw][IDX_TEMP_LOW]` �� `pchCalibDate`���� session δ��Ԫ���ݣ�Write BIN ǰ����ȷ�Ƿ�Ӧ�� backup ��������ʵ���� PRD���� |
| **INV-06** | **Ŀ��̬��ϡ�� merge��**�������� session ����У׼�� `(sw, ch)`�������� `PushCalibStatRow` / У׼ͳ��Ϊ׼������ֹ��ȫ `ch < 32` �޲�𿽱� session�� |
| **INV-06b** | **1��64 1310**��`MergeMems1310LowTempSlot` ���ϲ� `stCalibDAC[0].stChnDAC[0..16]`��PM ӳ�� CH_y 1..17����**`stChnDAC[17]`���� 18 ·��������������`stMidDAC[]` ��Զ���� backup**�����ñ��Ự 0 ���ǡ� |

---

## 2. Read Flash / �Ա�

| ID | ������ |
|----|--------|
| **INV-20** | `McsFwTransport::ReadLutBundleOnCurrentTunnel` �� 0xC4 �������� `stLutSettingZ4671` payload��Read ·��**����**������ sw 32/33 ���㡣 |
| **INV-21** | `M576_FLASH_LUT_READ_BASE` Ĭ�� `0`��`CalibConstants.h`�������̼�ʵ��Ϊ `0x65000` �ȣ���̼�ȷ�Ϻ�ĺ겢��¼�汾������ Read ���豸��һ�¡� |
| **INV-22** | �� backup/Read ����� **SN33/SN34 �� LOW Ϊ 0 �� ROOM/HIGH �� 0**�����Ȱ���1310 ��д sw 32/33 ���²ۡ�����ʷ bin �������⣬�ٲ���λ���Ƿ����� `sw<34` ȫ�� merge����ʷ bug���� |

---

## 3. RECAL / Ѱ��

| ID | ������ |
|----|--------|
| **INV-10** | **Flat 扫频**（`profile.trend == Flat` + `ParabolaNotDownward` 等）：**一步跳到** `M576_MAX_DAC_RANGE`（`SuggestJumpMaxDacRange`，非 ×2 阶梯），base 不变；已在 max 仍失败则 **FlatAtMaxShift**（`AdjustProfileForFlatAtMaxShift` + `SuggestSweepRecenterNewBase`，保持 offset=200）。粗扫 cubic OK 或 coarse hint 后须 **FineRefine** 回 UI `m_dacRange`（`NeedsFineRefineAfterSuccess`）。FineRefine 扫频使用 `Peak1DFitPolicy::FineRefineRelaxed`（`IsFineRefineSweepAttempt` / `Peak1DFitPolicyForSweepResult`：跳过 relFlat/全序列单调/贴边导数；三阶失败可 argmax 回退）；**交叉寻峰**对已在 uiFineRange 的 Y/X 数据同样用 Relaxed（`Peak1DFitPolicyForSweepResult` / `Peak1DFitPolicyForCrossAxis`）。`IsRetryablePeakFailure` 对纯 Flat 仍返回 false；Flat@max 平移为产线特例。最大 attempt = `M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS`（12）。 |
| **INV-11** | `StrictInc` / `StrictDec` 及贴边 NonMono + `ParabolaNotDownward` / `NotEnoughValidSamples`：**首步 MonoCoarseShift**（`IsMonotoneSweepFailure` → offset=`M576_PEAK1D_COARSE_DAC_RANGE`（200）**且同一步** `SuggestSweepRecenterNewBase` 平移 base）；粗扫阶段后续失败走 **ShiftOnly**（保持 coarse range）。粗扫 OK 或 `IsCoarsePeakHint`（内峰 NonMono / 有限 t*）→ **FineRefine** 至 UI range（精扫同上 FineRefineRelaxed）。贴边 NonMono：argmax index ≤1 或 ≥ n-2。 |
| **INV-12** | Ѱ�� recenter ʧ��ʱ�������ٳ���һ�� `SuggestSweepRecenterNewBase` �ٷ������᣻����δ����ֱ�� skip ��һ��ɨ��ͨ��ʧ�ܳ��⣩�� |
| **INV-13** | ���� `newBase` �� `SuggestSweepRecenterNewBase` ǯλ�� int16 ��Χ��**����**����ֵĨΪ 0�� |
| **INV-14** | �̼���Ч����ռλ���� `M576_RECAL_POW_INVALID_1` (-999999.0) �� `M576_RECAL_POW_INVALID_2` (-999900.0)�����ǰ�޳��� |
| **INV-15** | ���경��ʧ������ɻ��� `Peak1DValidateCode`���� trend/col0/attempts �ȣ�����ֹ����������־�гɰܡ� |
| **INV-16** | **PM / RECAL 3 only**����Ч����ȫ�ּ���ֵ `dBm = raw/10000` ���ڽ��� `pm_range` 0..3 ��Ӧ�����ڣ�`pm_range==4`��auto��������ʧ���� `PmRangeMismatch`��**������** recenter�������� path ����PD��RECAL 5��������λУ�顣 |
| **INV-17** | **PM Run Path only**��`RECAL 0` �ɹ����뷢 `opm 4 1` �� `opm 5 1`��Ӧ���Ϊ�������� 0..4������·������һ�£��������/RECAL 0 �� `pm_range` һ�£�`pm_range==4` auto ����¼���ء����ȶԣ�����һ�»�ͨ��/����ʧ�ܣ�**��־ + ���� + ����ֹͣ**���� Run Path��������·�� CSV�� |
| **INV-18** | **RECAL 3 / RECAL 5 ɨƵ**�� **6 ����**��`{mode} {baseX} {baseY} {offset} {step} {delay}`��mode **0**���� X ɨ Y����`baseX=9999` ���䣬��ɨ `baseY=9999`������ֻ�� `baseY`��mode **1**���� Y ɨ X����`baseY=Y@peak` ȫ�̲��䣬��ɨ `baseX=9999`������ֻ�� `baseX`��`RECAL 5` �� `RECAL 3` ͬ������ 5 �����̼�/��־���ټ��ݡ� |
| **INV-19** | **Y 预扫通过但 cross Y 失败**：`PeakCrossFrom1DScans` 在 Y 轴 `Peak1DValidateCode != Ok` 时触发 **重炉 RECAL 3/5 mode 0** 重扫 Y；各 round 由 `PlanRecalYCrossResweep`（内部 `PlanNextRecal1DSweepAttempt`）规划，与 Y 预扫重试 **同一 planner**（INV-10/11 粗扫/细扫两轨）；成功后再以 `Y@peak` 扫 X；X cross 失败时只重扫 X，不重炉 Y。 |

---

## 4. �ܹ���ͨ��

| ID | ������ |
|----|--------|
| **INV-30** | Z4671Core ������ MFC �Ի���bin ������ `Z4767StructDefine.h` Ϊ׼�������Ը� `stLutSettingZ4671` ��Ա���֡� |
| **INV-31** | ������ MCS ���ʱ����� `trans n`������ `$$`��ASCII RECAL ��͸���𲢷���ռͬһ�����޳�ʱ�� |
| **INV-32** | ���д���/Flash IO ���г�ʱ����ȷʧ�ܷ��أ���ֹ UI �̳߳�ʱ�������� |

---

## 5. ��֤�嵥�������

- [ ] �� Merge / Write BIN������ Z4671 ���� CSV����� **SN33/34��CH19�C32** �Ƿ��� backup һ�£�δУ׼��Ӧ���䣩��
- [ ] ��Ѱ��/���ԣ����� `CrossPeakTest.exe`��Release\|Win32����
- [ ] �� Read��ȷ�� `M576_FLASH_LUT_READ_BASE` ��̼�һ�¡�

---

## 6. ����ĵ������

| ��Դ | ·�� |
|------|------|
| �±���� | [`LUT_INDEXING.md`](LUT_INDEXING.md) |
| �ܹ� | [`DEVELOPMENT_AND_CODE_GUIDE.md`](DEVELOPMENT_AND_CODE_GUIDE.md) |
| Cursor | `.cursor/rules/m576-lut-bin.mdc`��`m576-peak-recal.mdc` |
