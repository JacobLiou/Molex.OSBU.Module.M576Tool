$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot | Join-Path -ChildPath "output"
$pmIn = Join-Path $root "standard_pm.csv"
$pdIn = Join-Path $root "standard_pd.csv"

function Get-PmSlot([int]$ti) {
    switch ($ti) {
        3 { return 0 }
        4 { return 1 }
        1 { return 2 }
        2 { return 2 }
        5 { return 3 }
        6 { return 3 }
        default { return -1 }
    }
}

function Get-PdSlot([int]$ti) {
    switch ($ti) {
        3 { return 0 }
        4 { return 1 }
        1 { return 3 }
        2 { return 3 }
        default { return -1 }
    }
}

function Split-Csv {
    param(
        [string]$inPath,
        [string[]]$outNames,
        [scriptblock]$slotFn
    )
    $lines = Get-Content -LiteralPath $inPath -Encoding UTF8
    $header = New-Object System.Collections.ArrayList
    $buckets = @(
        (New-Object System.Collections.ArrayList),
        (New-Object System.Collections.ArrayList),
        (New-Object System.Collections.ArrayList),
        (New-Object System.Collections.ArrayList)
    )
    foreach ($line in $lines) {
        if ($line -match '^\s*#' -or $line.Trim() -eq "") {
            [void]$header.Add($line)
            continue
        }
        if ($line -match '^\s*target_index') {
            [void]$header.Add($line)
            continue
        }
        $parts = $line -split ','
        if ($parts.Count -lt 2) { continue }
        $ti = 0
        if (-not [int]::TryParse($parts[0].Trim(), [ref]$ti)) { continue }
        $s = & $slotFn $ti
        if ($s -ge 0 -and $s -le 3) {
            [void]$buckets[$s].Add($line)
        }
    }
    for ($i = 0; $i -lt 4; $i++) {
        $out = Join-Path $root $outNames[$i]
        $all = New-Object System.Collections.ArrayList
        foreach ($h in $header) { [void]$all.Add($h) }
        foreach ($d in $buckets[$i]) { [void]$all.Add($d) }
        Set-Content -LiteralPath $out -Value $all -Encoding UTF8
        Write-Host ("{0} -> {1} data rows" -f $outNames[$i], $buckets[$i].Count)
    }
}

$pmNames = @("pm_mcs1.csv", "pm_mcs2.csv", "pm_1x64_1.csv", "pm_1x64_2.csv")
Split-Csv -inPath $pmIn -outNames $pmNames -slotFn ${function:Get-PmSlot}
$pdNames = @("pd_mcs1.csv", "pd_mcs2.csv", "pd_1x64_1.csv", "pd_1x64_2.csv")
Split-Csv -inPath $pdIn -outNames $pdNames -slotFn ${function:Get-PdSlot}
Write-Host "Done."
