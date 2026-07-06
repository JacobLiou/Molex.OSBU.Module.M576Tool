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
| **INV-10** | **拼接式扫频 pipeline**（`Peak1DSweepPipeline` / `RunRecal1DSweepWithPeakRecenterRetry`）：**Phase1** `offset=uiFineRange`（UI `m_dacRange`，通常 64）`Strict` — Ok 则**直接返回**，不再精扫；Fail → **Phase2** `offset=M576_MAX_DAC_RANGE`（200）`Strict` — Ok → **Phase3 精扫** `offset=uiFineRange/2`（64→32）`FineRefineRelaxed`；Fail → **Phase4 拼接**：以首次@200 的 `movingBase` 为 `anchorBase`，左右交替最多 `M576_PEAK1D_STITCH_MAX_RETRIES`（3）次，位移 `k×M576_PEAK1D_STITCH_UNIT_DAC`（k=1 左 −200、k=2 右 +400、k=3 左 −600）；每段 append 后 **DAC 坐标合并**（重叠取平均）再寻峰（Relaxed + argmax 回退），**合成有峰即停** → 精扫半宽；拼接耗尽仍无峰 → **整轴 FATAL**（见 INV-15）。硬顶扫频 `M576_PEAK1D_PIPELINE_MAX_SWEEPS`（9）。`PlanNextRecal1DSweepAttempt` 等旧 planner **仅 CrossPeakTest 遗留自测**，产线路径不再调用。**平坦门限**统一为 INI `MinProminenceDb`（默认 0.3 dB）：离群剔除后 `useOk` 点 `max−min`（`Peak1DMinFlatSpanRaw`）；已移除 2.5 dB / `relFlat` 第二套判据。 |
| **INV-11** | pipeline **粗扫@200** 与 **精扫半宽** 分工：粗扫定位用 `Strict`；精扫用 `FineRefineRelaxed`（`Peak1DFitPolicyForSweepResult`：offset≤uiFine/2 视为精扫）。粗/合成峰 DAC 由 `PeakBaseFromCoarseHint` 定心。交叉寻峰对已采集 Y/X 用 `Peak1DFitPolicyForSweepResult(attemptRange, m_dacRange)`。 |
| **INV-12** | pipeline 业务早退仅 **`PmRangeMismatch`**（INV-16）与 **整轴算法耗尽**（FATAL）；通信/解析/串口失败由 Dlg 另判（`CommSweep` / `CommSerialBreak`），**不落** peak-pipeline FATAL。Y/X 预扫与 cross 重扫共用同一 `Recal1DSweepPipelineState` 引擎。 |
| **INV-13** | ���� `newBase` �� `SuggestSweepRecenterNewBase` ǯλ�� int16 ��Χ��**����**����ֵĨΪ 0�� |
| **INV-14** | �̼���Ч����ռλ���� `M576_RECAL_POW_INVALID_1` (-999999.0) �� `M576_RECAL_POW_INVALID_2` (-999900.0)�����ǰ�޳��� |
| **INV-15** | 单轴寻峰失败须结构化 `Peak1DValidateCode`、phase（`fine64`/`coarse200`/`stitch_kN`/`fineHalf`）、trend/col0/attempts 等，禁止仅靠 `LogInfo` 判成败。**pipeline 整轴耗尽**须 UI `[FATAL][peak-pipeline]` + `M576AppendFatalLogUtf8`（`output/m576_fatal.log`），`PushPathFailureOutcome` 用 `failStage=peak-pipeline-exhausted` / `CalibPathFailCategory::PeakPipelineExhausted`。 |
| **INV-16** | **PM / RECAL 3 only**����Ч����ȫ�ּ���ֵ `dBm = raw/10000` ���ڽ��� `pm_range` 0..3 ��Ӧ�����ڣ�`pm_range==4`��auto��������ʧ���� `PmRangeMismatch`��**������** recenter�������� path ����PD��RECAL 5��������λУ�顣 |
| **INV-17** | **PM Run Path only**��`RECAL 0` �ɹ����뷢 `opm 4 1` �� `opm 5 1`��Ӧ���Ϊ�������� 0..4������·������һ�£��������/RECAL 0 �� `pm_range` һ�£�`pm_range==4` auto ����¼���ء����ȶԣ�����һ�»�ͨ��/����ʧ�ܣ�**��־ + ���� + ����ֹͣ**���� Run Path��������·�� CSV�� |
| **INV-18** | **RECAL 3 / RECAL 5 ɨƵ**�� **6 ����**��`{mode} {baseX} {baseY} {offset} {step} {delay}`��mode **0**���� X ɨ Y����`baseX=9999` ���䣬��ɨ `baseY=9999`������ֻ�� `baseY`��mode **1**���� Y ɨ X����`baseY=Y@peak` ȫ�̲��䣬��ɨ `baseX=9999`������ֻ�� `baseX`��`RECAL 5` �� `RECAL 3` ͬ������ 5 �����̼�/��־���ټ��ݡ� |
| **INV-19** | **Y 预扫通过但 cross Y 失败**：`PeakCrossFrom1DScans` 在 Y 轴 `Peak1DValidateCode != Ok` 时触发 **重炉 RECAL 3/5 mode 0** 重扫 Y；`PlanRecalYCrossResweep` → `PlanRecalYCrossResweepPipeline` 用 `PeakBaseFromCoarseHint` 平移 `baseY`，**offset 保持 UI `m_dacRange`**，再跑完整 **拼接式 pipeline**（与 Y 预扫同一引擎）；成功后再以 `Y@peak` 扫 X；**X cross 失败**时 `PlanRecalYCrossResweepPipeline` 只重扫 X，不重炉 Y。 |

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
