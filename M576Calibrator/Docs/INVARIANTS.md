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
| **INV-10** | **Flat ɨƵ**��`profile.trend == Flat` + `ParabolaNotDownward` �ȣ������� **���� offset**����2���� `M576_MAX_DAC_RANGE`����base ��������ƽ̹���޷�����ʧ�ܡ��� range ������ Flat���� mono recenter��INV-11/12����**���� range ����� plateau+����β��NonMono �� tail StrictDec/Inc������ mono recenter���ұ������� offset ������**��`IsRetryablePeakFailure` �� Flat �Է��� false��mono ƽ�Ʋ����� Flat����`afterFlatExpandRange=true` ʱ���� tail/��Ե NonMono ת mono����� attempt = `M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS`��10���� |
| **INV-11** | `StrictInc` / `StrictDec` 及贴边 NonMono + `ParabolaNotDownward` / `NotEnoughValidSamples`：base 平移前须先 **扩大 offset 一次**（`IsMonotoneSweepFailure` → `SuggestFlatRetryDacRange`，×2，cap `M576_MAX_DAC_RANGE`，base 不变）；扩大后仍失败则 `SuggestSweepRecenterNewBase` 平移。贴边 NonMono：argmax index ≤1 或 ≥ n-2。 |
| **INV-12** | Ѱ�� recenter ʧ��ʱ�������ٳ���һ�� `SuggestSweepRecenterNewBase` �ٷ������᣻����δ����ֱ�� skip ��һ��ɨ��ͨ��ʧ�ܳ��⣩�� |
| **INV-13** | ���� `newBase` �� `SuggestSweepRecenterNewBase` ǯλ�� int16 ��Χ��**����**����ֵĨΪ 0�� |
| **INV-14** | �̼���Ч����ռλ���� `M576_RECAL_POW_INVALID_1` (-999999.0) �� `M576_RECAL_POW_INVALID_2` (-999900.0)�����ǰ�޳��� |
| **INV-15** | ���경��ʧ������ɻ��� `Peak1DValidateCode`���� trend/col0/attempts �ȣ�����ֹ����������־�гɰܡ� |
| **INV-16** | **PM / RECAL 3 only**����Ч����ȫ�ּ���ֵ `dBm = raw/10000` ���ڽ��� `pm_range` 0..3 ��Ӧ�����ڣ�`pm_range==4`��auto��������ʧ���� `PmRangeMismatch`��**������** recenter�������� path ����PD��RECAL 5��������λУ�顣 |
| **INV-17** | **PM Run Path only**��`RECAL 0` �ɹ����뷢 `opm 4 1` �� `opm 5 1`��Ӧ���Ϊ�������� 0..4������·������һ�£��������/RECAL 0 �� `pm_range` һ�£�`pm_range==4` auto ����¼���ء����ȶԣ�����һ�»�ͨ��/����ʧ�ܣ�**��־ + ���� + ����ֹͣ**���� Run Path��������·�� CSV�� |
| **INV-18** | **RECAL 3 / RECAL 5 ɨƵ**�� **6 ����**��`{mode} {baseX} {baseY} {offset} {step} {delay}`��mode **0**���� X ɨ Y����`baseX=9999` ���䣬��ɨ `baseY=9999`������ֻ�� `baseY`��mode **1**���� Y ɨ X����`baseY=Y@peak` ȫ�̲��䣬��ɨ `baseX=9999`������ֻ�� `baseX`��`RECAL 5` �� `RECAL 3` ͬ������ 5 �����̼�/��־���ټ��ݡ� |
| **INV-19** | **Y Ԥ��ͨ���� cross Y ʧ��**��`PeakCrossFrom1DScans` �� Y �� `Peak1DValidateCode != Ok`������ **��¯ RECAL 3/5 mode 0** ��ɨ Y����� round �� `M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS`������ `PlanRecalYCrossResweep` �滮��Flat �� shallow �� **�� offset**��INV-10�������� **ƽ�� baseY**��INV-11/12�����ɹ������� `Y@peak` ��ɨ X��X cross ʧ����ֻ���� X������¯ Y�� |

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
