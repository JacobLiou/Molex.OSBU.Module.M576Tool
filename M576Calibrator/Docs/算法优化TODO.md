# 1310 Ѱ�� / �����㷨�Ż� TODO�����۸壩

> **״̬**�������У�**��δ����ʵʩ**��  
> **����**��1310 У׼�̼��� Flash �� DAC��`RECAL` �� `base=9999`��ΪɨƵ��㣻���ߴ������Ա�������ͨ�� IL �Բ����롣��ǰ��λ�� RECAL 3/5 + `PeakFinder2D` ����Ѱ���� `Peak1DSweepRecenter` ���Բ����ѽ�����������΢��������Ϲ�ʽ�ԡ���ֵƫԶ / ����ϡ������������������ޡ�  
> **����ĵ�**��[`INVARIANTS.md`](INVARIANTS.md)��INV-10..19����[`DEVELOPMENT_AND_CODE_GUIDE.md`](DEVELOPMENT_AND_CODE_GUIDE.md)��[`LUT_INDEXING.md`](LUT_INDEXING.md)  
> **��ش���**��`PeakFinder2D.cpp`��`Peak1DSweepRecenter.cpp`��`M576Peak1DConstants.h`��`M576CalibratorDlg.cpp`��`RunRecal1DSweepWithPeakRecenterRetry`����`CrossPeakTest/`��`dataAnalysis/analyze_il_channels.py`

---

## 1. ���� framing�����۹�ʶǰ�ᣩ

### 1.1 ��ǰ��·�����ǡ��� 2D ���������

PM/PD Run Path ʵ���ǣ�

1. `RECAL 0` ���ù�Դ / PM ��  
2. `RECAL 1` ·��  
3. `RECAL 3`���� `5`��**mode 0**���� X��ɨ Y  
4. **mode 1**���� Y@peak��ɨ X  
5. `PeakCrossFrom1DScans`������������� LSQ + ����У�飬�õ����� `(row, col)` �� DAC  

��ˡ�Peak2D �յ��㷨���������������ࣺ

| ���� | ���������� | ��λ������ |
|------|------------|------------|
| ��־�޷� / `flat` / `StrictInc` / skip | Flash ��ֵƫ��ɨƵ����δ��ס��� | recenter���� offset����ʽ base |
| �� `-> peak row=...` �� IL �� | ���塢�԰ꡢ�� Y-span����ȫ�����Ž���� | ɨƵ�ܶȡ��������ޡ���ͨ������ |
| ���Դ����Բ� | ������ϲ���ֵ/���ڸ������� | backup base�����˶��ꡢ�̼�Эͬ |

### 1.2 ������λ���������Ķ�ǰ���ظ����֣�

- Flat / �������� / �� offset / base ƽ�ƣ�INV-10��INV-11��  
- Y Ԥɨ OK������ Y ʧ�� �� ��¯ RECAL 3/0��INV-19��  
- ��� 12 �� attempt��`M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS`��  
- PM��`RECAL 0` �� `opm` ��λ�˶ԣ�INV-17��  
- ���߻ع飺`CrossPeakTest`��comm ������`dataAnalysis/analyze_il_channels.py`

### 1.3 ���� comm �������ͣ�������������

�ο� `comm_2026-06-17` ���豸 K6528198 ������

- **Ѱ��ʧ����**���� Step 565��`no peak` + `flat` + `StrictInc`��  
- **Ѱ��ɹ���IL ����**���� Step 568���з嵫 Y-span ���ͣ�~9948 vs ���� ~81875��  
- **�߷�����**���� 2#MCS slot2��Step 187��565 �ȣ������Ƿ����·/ӳ����ش�����  

**������**����ͨ���С�ʧ�� vs �ɹ��� IL ���ռ���٣���������Ͷ�� recenter �����������ޡ�

---

## 2. �Ż��� backlog�����������ȼ���

ÿ���ע��**״̬**��**�㼶**����λ�� / �̼� / �������̣���**����**��**����**��

### P0 �� ���ݷ�������ͣ�������ű���ǿ��

| ID | ���� | ״̬ | ˵�� |
|----|------|------|------|
| **ALG-P0-01** | ��ͨ���Զ����ͱ��� | ������ | ��չ `analyze_il_channels.py`��������� `validate_code`��`trend`��`Y-span`����λ���ߡ�retry ���������� base/offset������ IL ���� |
| **ALG-P0-02** | comm �� CrossPeakTest �طűջ� | ������� | ��λ�� Run Path ��ʵʱд `comm_*_recal_sweeps.csv`����ȫ�� retry����`tools/extract_recal_sweep_csv.py` ������**�� comm ��־**�طţ�`CrossPeakTest --export-peak-csv` / `--mock-sweeps` ������ `path` �� |
| **ALG-P0-03** | ��ͨ���嵥ά�� | ������ | ����ά�����߷� step / slot / MCS �项������Ϊ�����������������յ����� |

**���۵�**������ά���Ƿ��㹻���Ƿ���Ҫ�� `(fileSlot, sw, ch)` �������� path step index��

---

### P1 �� ɨƵ������ UI Ĭ��ֵ���ͷ��գ���С��Χʵ���ԣ�

| ID | ���� | ״̬ | ˵�� |
|----|------|------|------|
| **ALG-P1-01** | ��ͨ�� `dacRange` / `dacStep` / `delayMs` ����ʵ�� | ������ | �Թ̶��� step ��С����ɨ�裻ע�� `M576_MAX_RECAL_SWEEP_READ_MS` ��ʱ |
| **ALG-P1-02** | Ĭ�� UI �������߽���ֵ�ĵ��� | ������ | ��ͨ�� P0 ������д���û��ֲ�� TODO ��¼ |
| **ALG-P1-03** | ��ɨ��������������ȶ��� | ������ | `dacStep` ��С �� �������ࣻ�� `M576_PEAK1D_CUBIC_MIN_SAMPLES`��fine refine ��ϵ |

**���۵�**���Ƿ�������ȫ�� UI �������롸��ͨ�����ǡ����棿�� P2-02��

---

### P2 �� ��ʽ base ��ͨ�������ã��еȸĶ�������Ԥ�ڸߣ�

| ID | ���� | ״̬ | ˵�� |
|----|------|------|------|
| **ALG-P2-01** | Retry ʱ�� Backup LUT DAC ����ʽ `baseY`/`baseX` | ������ | Read Bin ��� `backupAll1310DAC.csv` / session LUT ȡ `(sw,ch)` DAC��ʧ������ʱ���ý� `9999`���������� Flash ��ֵ |
| **ALG-P2-02** | ��ͨ�����ñ���CSV/JSON�� | ������ | ��ʾ����`step_index, coarse_range, fine_range, delay_ms, force_base_y, force_base_x, max_extra_retry`��Run Path �������� UI |
| **ALG-P2-03** | �ڵ� / ͬ SW ��ֵ��ֵ�����ã� | ������ | ��ֵ����������У׼ͨ��������� INV-06 ϡ�� merge��������ȾδУ׼��λ |

**���۵�**��

- P2-01 �����Ƿ����� `9999`�����ǻ�ͨ���б������ּ��� backup��  
- P2-02 ����˭ά�������� / ���������Ƿ���汾������  
- P2-03 �Ƿ���ܡ������ڵ���������ϵͳ�Է��գ�

**��ڴ��루ʵʩʱ��**��`RunRecal1DSweepWithPeakRecenterRetry` �� `initialMovingBase`��PM/PD ��ѭ���� `M576_RECAL_FW_READ_BASE_DAC` ��ʹ�ô���

---

### P3 �� �㷨����΢������ CrossPeakTest + INV ͬ����

| ID | ���� | ״̬ | ˵�� |
|----|------|------|------|
| **ALG-P3-00** | PeakFinder ��ͻ���� + �Գ���ϴ� + INI ���� `MinProminenceDb` | **��ʵʩ** | `Peak1DDbToRawDelta` ������**2026-07 �ſ�**��0.3 dB ��Ԥ����ǰȫ���� span Ӳ�ܣ�����ѡ����L523/step7 ��� Ok��CrossPeakTest �ع� |
| **ALG-P3-05** | JumpFlatMax@200 ��ɨ�ڷ��Ϸ� �� FineRefine | **��ʵʩ** | `M576_PEAK1D_COARSE_MIN_SPAN_DB`��0.5 dB����`IsCoarseExpandedInteriorPeak`��`Peak1DArgmaxHasLeft/RightProminence`��step7 att2 ���� / att3 ����Strict ��ɨ�Կ� fail |
| **ALG-P3-06** | Flat@200 FlatAtMaxShift ping-pong ������խ��Χ�� | **��ʵʩ** | �� Flat@max��`IsInteriorPeakHint`��hint ���ģ�`DetectSweepRecenterOscillation`��FineRefine��Mono/ShiftOnly ��ع飻CrossPeakTest frozen + step7 |
| **ALG-P3-07** | planner 早退 GiveUp -> INV-12 fallback 打满 12 次 | **已实施** | `SuggestFallbackSweepRetryPlan`；`IsFlatSweepFailure` 含 VOR/EdgeNotAllowed；`PmRangeMismatch` 唯一业务早退；CrossPeakTest `RunMaxAttemptsFallbackSelfTests` + vor-diag |
| **ALG-P3-08** | 拼接式扫频 pipeline 替换 INV-10/11/12 planner | **已实施** | `Peak1DSweepPipeline`；64→200→交替拼接→精扫半宽；FATAL `peak-pipeline`；`RunSweepPipelineSelfTests`；产线 Dlg 不再调用 `PlanNextRecal1DSweepAttempt` |
| **ALG-P3-01** | ����ͨ����`M576_PEAK1D_MIN_SPAN_DB` / `FLAT_REL_SPAN_FRAC` | ������ | �� span �Խ��ܷ� �� �������� recenter������������������� |
| **ALG-P3-02** | ��� / �԰꣺outlier �� `MAX_STRICT_LOCAL_MAXIMA` | ������ | `OUTLIER_MULT`��`OUTLIER_LOCAL_HALF` �� |
| **ALG-P3-03** | Recenter �����ȣ�`SWEEP_RECENTER_*_FRAC` ϵ�� | ������ | `TSTAR_WEIGHT`��`STAGNATION_GAIN` �ȶ�����ͨ����Ӱ�� |
| **ALG-P3-04** | �������ԣ��Ƿ����� 2D ���������ͣ��ǵ�ǰ��·���� | ������ | ��ǰΪ���� 1D ���棻�� 2D ��������������������̼�ɨƵģ�� |

**Լ��**���� `M576Peak1DConstants.h` ������ `CrossPeakTest` Release\|Win32���漰 Flat/mono ��������� INV-10/11������ `INVARIANTS.md` �� `.cursor/rules/m576-peak-recal.mdc`��

**���۵�**��P3 �Ƿ��� P2����ʽ base����Ч����������������ڸǳ�ֵ���⣿

---

### P4 �� ������������ޣ�PRD FR-07 ���룬���߶��ף�

| ID | ���� | ״̬ | ˵�� |
|----|------|------|------|
| **ALG-P4-01** | ��ɹ� �� �ϸ񣺵������� / IL ���� | ������ | �����ɹ��󸴺˹��ʻ� IL��������д LUT���� `CalibPathFailCategory` ����� |
| **ALG-P4-02** | �ظ��Բ��ԣ�FR-07�� | ������ | PRD Ҫ��У׼���ظ��� OK ����¼����ǰ App ���ȷ�������� |
| **ALG-P4-03** | Write Bin ǰ������������ | ������ | �� `M576RunPathSummaryDlg`��failure CSV ��������ѡ����ʧ�����ֹ��¼������ |

**���۵�**��IL / �ظ�����ֵ������Ժι����Ϊ׼������ʧ���� skip ������· path ʧ�ܣ�

---

### P5 �� Session ���ԣ����̼���

| ID | ���� | ״̬ | ˵�� |
|----|------|------|------|
| **ALG-P5-01** | ���˶��� | ������ | ��һ��д��ͨ�� �� Write/Burn���� RAM ��Ч���� �ڶ���ר�ܻ�ͨ���Ӽ������ø��º� Flash ��ֵ |
| **ALG-P5-02** | Diagnosis ģʽ���ڹҲ⸴�� | ������ | ���� `DiagnosisSession` �Ƿ���չΪ������� |
| **ALG-P5-03** | ��ͨ����·�� CSV | ������ | ������ `path_bad_steps.csv` �г��Ĳ������̲���ʱ�� |

---

### P6 �� �̼� / Ӳ���߽磨��λ�������޷��պϣ�

| ID | ���� | ״̬ | ˵�� |
|----|------|------|------|
| **ALG-P6-01** | `9999` �� DAC ʱ����һ���� | ��̼�ȷ�� | ��ֵ���������� Flash ��̼�������� |
| **ALG-P6-02** | �̼������ɨƵ���� 2D ɨ | ��̼����� | X/Y ǿ������ʱ������ 1D �����ȫ������ |
| **ALG-P6-03** | MEMS settle�����ʼƵ�λ | ��Ӳ��/�̼� | �� INV-16/17��delay ������� |

---

## 3. ����ʵʩ˳�������ã��ǳ�ŵ��

```text
P0 ������ط� �� P1 ɨƵ����ʵ������ �� P2 backup base / ��ͨ������
    �� P4 �������ޣ�����߹����룩�� P3 ����΢������ comm ֤��������
    �� P5 ���˶��� �� P6 �̼�Эͬ�����и��٣�
```

**ԭ��**���� `m576-global.mdc` һ�£���

- ����Ŀ�꣺��������ÿ��У׼�ɹ������Ǳ߽����������� skip��  
- �����ԡ��ɻָ�����׷�ݡ��ɵ��⣨`CrossPeakTest`����  
- ���ڸǹ̼�ȱ�ڣ�ע�ͻ� INVARIANTS д�塸��λ����ʲô���̼����뱣֤ʲô����

---

## 4. �������⣨���������嵥��

1. ��ͨ������ռ�ȣ�Ѱ��ʧ�� vs Ѱ��ɹ� IL �  
2. �Ƿ���ܡ���ͨ�����ñ�������ά����ά�����������̣�  
3. Retry �� backup DAC �� base������ `9999` �Ƿ�����  
4. IL / �ظ��Ժϸ��׼����ֵ�����������ķݹ��Ϊ׼��  
5. ��������ʧ�ܣ�skip������ N �Ρ�����ͣ��· path��  
6. 2#MCS slot2 �ȸ߷�������·/ӳ��/�̼��Ƿ�����֪ϵͳ��ԭ��  
7. ���˶����Ƿ�ɽ��ܲ��߽������ӣ��ڶ����Ƿ������¼�м� bin��  
8. �Ƿ��� P2 ��Чǰ���� P3 ��Χ���޸Ķ������� overfit ��̨�豸 comm��

---

## 5. ʵʩǰ��飨��������ʱ��

- [ ] P0 ����Դ����豸����N ̨�����ֻ�ͨ������  
- [ ] `CrossPeakTest` ȫ�� + ���� comm �طŶԱ�  
- [ ] �Ķ����� INV-10..19 ʱ���� [`INVARIANTS.md`](INVARIANTS.md)  
- [ ] ʵ��С������֤ IL �ֲ������  
- [ ] �û��ֲ� / ���� SOP ͬ�������� UI �����̣�

---

## 6. �޶���¼

| ���� | ˵�� |
|------|------|
| 2026-06-18 | ���壺���� 1310 ���� IL �� comm ��������������״̬��Ϊ�������С�δʵʩ�� |
