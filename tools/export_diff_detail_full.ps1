param(
  [string]$BackupCsv = 'backupAll1310DAC.csv',
  [string]$StandardCsv = 'standardAll1310DAC.csv',
  [string]$OutputCsv = 'diff_detail_full.csv'
)

$ErrorActionPreference = 'Stop'

function Load-CsvSkipComment([string]$Path) {
  Get-Content $Path | Select-Object -Skip 1 | ConvertFrom-Csv
}

$keyCols = @('bin_role', 'file_suffix', 'sw_lut_idx', 'temp_idx', 'ch_idx', 'is_mid')

$bk = Load-CsvSkipComment $BackupCsv
$st = Load-CsvSkipComment $StandardCsv

$bkMap = @{}
foreach ($r in $bk) {
  $k = ($keyCols | ForEach-Object { $r.$_ }) -join '|'
  $bkMap[$k] = $r
}

$stMap = @{}
foreach ($r in $st) {
  $k = ($keyCols | ForEach-Object { $r.$_ }) -join '|'
  $stMap[$k] = $r
}

$allKeys = [System.Collections.Generic.HashSet[string]]::new()
$bkMap.Keys | ForEach-Object { [void]$allKeys.Add($_) }
$stMap.Keys | ForEach-Object { [void]$allKeys.Add($_) }

$rows = foreach ($k in ($allKeys | Sort-Object)) {
  $b = $bkMap[$k]
  $s = $stMap[$k]

  if (-not $s) {
    [PSCustomObject]@{
      key = $k
      status = 'missing_in_standard'
      bin_role = $b.bin_role
      file_suffix = $b.file_suffix
      sw_lut_idx = $b.sw_lut_idx
      temp_idx = $b.temp_idx
      ch_idx = $b.ch_idx
      is_mid = $b.is_mid
      backup_dac_x = $b.dac_x
      backup_dac_y = $b.dac_y
      standard_dac_x = $null
      standard_dac_y = $null
      delta_dac_x = $null
      delta_dac_y = $null
      backup_diff = $null
      standard_diff = $null
      delta_diff = $null
    }
    continue
  }

  if (-not $b) {
    [PSCustomObject]@{
      key = $k
      status = 'missing_in_backup'
      bin_role = $s.bin_role
      file_suffix = $s.file_suffix
      sw_lut_idx = $s.sw_lut_idx
      temp_idx = $s.temp_idx
      ch_idx = $s.ch_idx
      is_mid = $s.is_mid
      backup_dac_x = $null
      backup_dac_y = $null
      standard_dac_x = $s.dac_x
      standard_dac_y = $s.dac_y
      delta_dac_x = $null
      delta_dac_y = $null
      backup_diff = $null
      standard_diff = $null
      delta_diff = $null
    }
    continue
  }

  $bx = [double]$b.dac_x
  $by = [double]$b.dac_y
  $sx = [double]$s.dac_x
  $sy = [double]$s.dac_y

  $dx = $sx - $bx
  $dy = $sy - $by
  $bd = $bx - $by
  $sd = $sx - $sy
  $dd = $sd - $bd

  [PSCustomObject]@{
    key = $k
    status = 'matched'
    bin_role = $b.bin_role
    file_suffix = $b.file_suffix
    sw_lut_idx = $b.sw_lut_idx
    temp_idx = $b.temp_idx
    ch_idx = $b.ch_idx
    is_mid = $b.is_mid
    backup_dac_x = $bx
    backup_dac_y = $by
    standard_dac_x = $sx
    standard_dac_y = $sy
    delta_dac_x = $dx
    delta_dac_y = $dy
    backup_diff = $bd
    standard_diff = $sd
    delta_diff = $dd
  }
}

$rows | Export-Csv -Path $OutputCsv -NoTypeInformation -Encoding UTF8

Write-Output ("output_file={0}" -f $OutputCsv)
Write-Output ("row_count={0}" -f $rows.Count)
Write-Output ("status_matched={0}" -f (@($rows | Where-Object { $_.status -eq 'matched' }).Count))
Write-Output ("status_missing_in_standard={0}" -f (@($rows | Where-Object { $_.status -eq 'missing_in_standard' }).Count))
Write-Output ("status_missing_in_backup={0}" -f (@($rows | Where-Object { $_.status -eq 'missing_in_backup' }).Count))