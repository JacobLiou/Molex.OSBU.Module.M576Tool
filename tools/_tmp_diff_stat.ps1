$ErrorActionPreference='Stop'
$fBackup='backupAll1310DAC.csv'
$fStd='standardAll1310DAC.csv'

function Load-CsvSkipComment([string]$path){
  Get-Content $path | Select-Object -Skip 1 | ConvertFrom-Csv
}

$bk = Load-CsvSkipComment $fBackup
$st = Load-CsvSkipComment $fStd
$keyCols = @('bin_role','file_suffix','sw_lut_idx','temp_idx','ch_idx','is_mid')

$bkMap = @{}
foreach($r in $bk){ $k = ($keyCols | ForEach-Object { $r.$_ }) -join '|'; $bkMap[$k] = $r }
$stMap = @{}
foreach($r in $st){ $k = ($keyCols | ForEach-Object { $r.$_ }) -join '|'; $stMap[$k] = $r }

$allKeys = [System.Collections.Generic.HashSet[string]]::new()
$bkMap.Keys | ForEach-Object { [void]$allKeys.Add($_) }
$stMap.Keys | ForEach-Object { [void]$allKeys.Add($_) }

$missingInStd = 0
$missingInBk = 0
$rows = foreach($k in $allKeys){
  $b = $bkMap[$k]; $s = $stMap[$k]
  if(-not $s){ $missingInStd++; continue }
  if(-not $b){ $missingInBk++; continue }
  $bx = [double]$b.dac_x; $by = [double]$b.dac_y
  $sx = [double]$s.dac_x; $sy = [double]$s.dac_y
  $dx = $sx - $bx; $dy = $sy - $by
  $bd = $bx - $by; $sd = $sx - $sy; $dd = $sd - $bd
  [PSCustomObject]@{ key=$k; delta_dac_x=$dx; delta_dac_y=$dy; delta_diff=$dd }
}

function Stat($arr, $prop){
  $vals = @($arr | ForEach-Object { [double]($_.$prop) })
  $abs = @($vals | ForEach-Object { [math]::Abs($_) })
  [PSCustomObject]@{
    count = $vals.Count
    changed = @($vals | Where-Object { $_ -ne 0 }).Count
    min = ($vals | Measure-Object -Minimum).Minimum
    max = ($vals | Measure-Object -Maximum).Maximum
    mean = [math]::Round((($vals | Measure-Object -Average).Average),4)
    mean_abs = [math]::Round((($abs | Measure-Object -Average).Average),4)
  }
}

$sx = Stat $rows 'delta_dac_x'
$sy = Stat $rows 'delta_dac_y'
$sd = Stat $rows 'delta_diff'

Write-Output "matched_rows=$($rows.Count)"
Write-Output "missing_in_standard=$missingInStd"
Write-Output "missing_in_backup=$missingInBk"
Write-Output "delta_dac_x: changed=$($sx.changed), min=$($sx.min), max=$($sx.max), mean=$($sx.mean), mean_abs=$($sx.mean_abs)"
Write-Output "delta_dac_y: changed=$($sy.changed), min=$($sy.min), max=$($sy.max), mean=$($sy.mean), mean_abs=$($sy.mean_abs)"
Write-Output "delta_diff: changed=$($sd.changed), min=$($sd.min), max=$($sd.max), mean=$($sd.mean), mean_abs=$($sd.mean_abs)"
Write-Output "dist_delta_diff_top="
$rows | Group-Object delta_diff | Sort-Object Count -Descending | Select-Object -First 10 Name,Count | ForEach-Object { Write-Output ("  {0}: {1}" -f $_.Name,$_.Count) }
