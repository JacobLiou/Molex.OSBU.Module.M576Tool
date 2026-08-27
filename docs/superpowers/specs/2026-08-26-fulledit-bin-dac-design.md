# FullEdit Bin + 坏光路 DAC 总结 — Design Spec

**Date:** 2026-08-26  
**Audience:** 研发工程师（与 FineTune / Make Bin 同级调试工具）  
**Scope:** 旁路编辑 10 路 burn bin 全温区 DAC；按产品 CH 汇总四级 DAC。不进入产线 SOP。

## 1. 主流程隔离（硬约束）

- 不改变 `Read Bin` → `Calibrate` → `Write BIN` → `Burn Flash` 语义、门限、Merge/INV。
- 只读写磁盘 `{SN}_backup.bin` / `{SN}_standard.bin` 与 `fulledit/` CSV。
- 不自动 Burn；不改路径 CSV / RECAL 默认。
- 写回后**默认不同步**主窗 session LUT/Mems；可选勾选「同步到会话内存」。
- UI 标明「研发调试」；按钮靠近 Make Bin / FineTune。

## 2. 交互

1. 选 Backup / Standard  
2. Export → `fulledit/{role}/working|baseline/{SN}_dac.csv`（各 10）  
3. Excel 改 working  
4. Unlock（危险行）→ Validate & Write（仅 diff 行/文件）  
5. Path Impact：CH 1..576 → 四级 DAC 总结（不自动改 CSV）

## 3. 文件命名

```text
{outDir}/fulledit/{backup|standard}/
  working/{SN}_dac.csv
  baseline/{SN}_dac.csv
  path_impact/CH{nnn}_impact.txt
```

Burn index 0..9 与 FineTune / `M576SnForBurnFileIndex` 一致。

## 4. CSV Schema

### MCS (`schema=mcs`)

行键 `(sw_lut_idx, ch_idx)`。列含三温 `*_dac_y/*_dac_x`、`temp_point_*`、`delta_*`（写回忽略 delta）。

### 1×64 (`schema=mems`)

行键 `(row_kind, row_idx)`：`CH` 0..35 / `MID` 0..24。三温对应 `stCalibDAC[0/1/2]`。  
轴约定（与 FineTune 一致）：CSV `dac_y` ? `sDACx`，CSV `dac_x` ? `sDACy`。

## 5. 写回

相对 baseline 差分写回；全表可写（无 Unlock UI）。默认不同步 session LUT。

## 6. 错误码

`FE_OK`, `FE_NO_SN`, `FE_BIN_MISSING`, `FE_EXPORT_FAIL`, `FE_BASELINE_MISSING`, `FE_CSV_PARSE`, `FE_CSV_KEY`, `FE_DAC_RANGE`, `FE_DANGEROUS_LOCKED`, `FE_DIFF_EMPTY`, `FE_WRITE_FAIL`, `FE_VERIFY_FAIL`, `FE_PATH_CH`, `FE_BUSY`

## 7. Path Impact

`sw=(CH-1)/18+1`, `mcsCh=(CH-1)%18+1`, `pb=sw+32`。  
MCS1/2：block=`sw`, ch=`mcsCh`。  
1×64：Mapping 唯一行 `(c1=sw,c4=pb)` → SW/CH_y。

## 8. 非目标

- 不替代 Run Path / Merge  
- 不提供产线一键校准  
- CrossPeakTest 覆盖映射与 CSV diff；不要求硬件  
