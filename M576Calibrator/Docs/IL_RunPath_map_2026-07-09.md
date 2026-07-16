# IL vs Run Path map (2026-07-09)

Sources:
- IL: validationIL.png (21 sampled paths)
- Comm: comm_2026-07-09_recal_sweeps.csv

Rule: step = (ch1-1)*18 + ch2; ch1=1#1x64, ch2/ch3=MCS pair, ch4=2#1x64 (usually ch1+32).

## Summary (MAX-MIN desc)

| MAX-MIN | step | ch1/ch4 | ch2/ch3 | MPO in->out | sweeps | fails | note |
|--------:|-----:|---------|---------|-------------|--------:|------:|------|
| 3.5917 | 494/576 | 28/60 | 8/8 | MPO42-2->MPO90-2 | 6 | 1 | slot2Y att1 ParabolaNotDownward |
| 2.9899 | 551/576 | 31/63 | 11/11 | MPO46-11->MPO94-11 | 4 | 0 | all peak_ok=1 |
| 2.3530 | 241/576 | 14/46 | 7/7 | MPO21-1->MPO69-1 | 6 | 1 | slot2Y att1 ParabolaNotDownward |
| 2.2313 | 444/576 | 25/57 | 12/12 | MPO37-12->MPO85-12 | 4 | 0 | all peak_ok=1 |
| 2.2015 | 458/576 | 26/58 | 8/8 | MPO39-2->MPO87-2 | 6 | 1 | slot1Y att1 ParabolaNotDownward |
| 1.9543 | 547/576 | 31/63 | 7/7 | MPO46-7->MPO94-7 | 9 | 5 | slot1Y att1 ParabolaNotDownward; slot2Y att1 ParabolaNotDownward; slot2Y att2 ParabolaNotDownward; slot2Y att3 ParabolaNotDownward; slot2Y att4 ParabolaNotDownward |
| 1.6695 | 570/576 | 32/64 | 12/12 | MPO48-6->MPO96-6 | 7 | 4 | slot1Y att1 ParabolaNotDownward; slot1Y att2 ParabolaNotDownward; slot1Y att3 ParabolaNotDownward; slot1Y att4 ParabolaNotDownward |
| 1.1819 | 332/576 | 19/51 | 8/8 | MPO28-8->MPO76-8 | 6 | 1 | slot1Y att1 ParabolaNotDownward |
| 1.0814 | 529/576 | 30/62 | 7/7 | MPO45-1->MPO93-1 | 6 | 1 | slot2Y att1 ParabolaNotDownward |
| 0.9973 | 187/576 | 11/43 | 7/7 | MPO16-7->MPO64-7 | 4 | 0 | all peak_ok=1 |
| 0.7323 | 61/576 | 4/36 | 7/7 | MPO06-1->MPO54-1 | 9 | 5 | slot1Y att1 ParabolaNotDownward; slot2Y att1 ParabolaNotDownward; slot2Y att2 ParabolaNotDownward; slot2Y att3 ParabolaNotDownward; slot2Y att4 ParabolaNotDownward |
| 0.6051 | 223/576 | 13/45 | 7/7 | MPO19-7->MPO67-7 | 7 | 2 | slot1Y att1 ParabolaNotDownward; slot1Y att2 ParabolaNotDownward |
| 0.5321 | 457/576 | 26/58 | 7/7 | MPO39-1->MPO87-1 | 7 | 4 | slot1Y att1 ParabolaNotDownward; slot1Y att2 ParabolaNotDownward; slot1Y att3 ParabolaNotDownward; slot1Y att4 ParabolaNotDownward |
| 0.5017 | 259/576 | 15/47 | 7/7 | MPO22-7->MPO70-7 | 6 | 1 | slot1Y att1 ParabolaNotDownward |
| 0.3002 | 335/576 | 19/51 | 11/11 | MPO28-11->MPO76-11 | 4 | 0 | all peak_ok=1 |
| 0.2707 | 85/576 | 5/37 | 13/13 | MPO08-1->MPO56-1 | 4 | 0 | all peak_ok=1 |
| 0.2545 | 88/576 | 5/37 | 16/16 | MPO08-4->MPO56-4 | 4 | 0 | all peak_ok=1 |
| 0.2404 | 92/576 | 6/38 | 2/2 | MPO08-8->MPO56-8 | 4 | 0 | all peak_ok=1 |
| 0.2094 | 90/576 | 5/37 | 18/18 | MPO08-6->MPO56-6 | 4 | 0 | all peak_ok=1 |
| 0.1885 | 96/576 | 6/38 | 6/6 | MPO08-12->MPO56-12 | 4 | 0 | all peak_ok=1 |
| 0.1696 | 87/576 | 5/37 | 15/15 | MPO08-3->MPO56-3 | 4 | 0 | all peak_ok=1 |

## Detail per path

### MPO42-2 -> MPO90-2 (MAX-MIN=3.5917)

- Run Path step: 494/576 (ch1=28 ch2=8 ch3=8 ch4=60)
- Route slot1: `RECAL 1 3 28 8 8 60`; slot2: `RECAL 1 4 28 8 8 60`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 1077 | 1 | Y | 1 | Y | Ok | 426 | `RECAL 3 0 9999 9999 64 4 80` |
| 1078 | 1 | X | 1 | Y | Ok | 2197 | `RECAL 3 1 9999 456 64 4 80` |
| 2341 | 2 | Y | 1 | N | ParabolaNotDownward | -535 | `RECAL 3 0 9999 9999 64 4 80` |
| 2342 | 2 | Y | 2 | Y | Ok | -671 | `RECAL 3 0 9999 9999 200 4 80` |
| 2343 | 2 | Y | 3 | Y | Ok | -540 | `RECAL 3 0 9999 -476 64 4 80` |
| 2344 | 2 | X | 1 | Y | Ok | 2111 | `RECAL 3 1 9999 -475 64 4 80` |

### MPO46-11 -> MPO94-11 (MAX-MIN=2.9899)

- Run Path step: 551/576 (ch1=31 ch2=11 ch3=11 ch4=63)
- Route slot1: `RECAL 1 3 31 11 11 63`; slot2: `RECAL 1 4 31 11 11 63`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 1204 | 1 | Y | 1 | Y | Ok | -505 | `RECAL 3 0 9999 9999 64 4 80` |
| 1205 | 1 | X | 1 | Y | Ok | -2232 | `RECAL 3 1 9999 -466 64 4 80` |
| 2476 | 2 | Y | 1 | Y | Ok | -842 | `RECAL 3 0 9999 9999 64 4 80` |
| 2477 | 2 | X | 1 | Y | Ok | -2189 | `RECAL 3 1 9999 -771 64 4 80` |

### MPO21-1 -> MPO69-1 (MAX-MIN=2.3530)

- Run Path step: 241/576 (ch1=14 ch2=7 ch3=7 ch4=46)
- Route slot1: `RECAL 1 3 14 7 7 46`; slot2: `RECAL 1 4 14 7 7 46`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 532 | 1 | Y | 1 | Y | Ok | 779 | `RECAL 3 0 9999 9999 64 4 80` |
| 533 | 1 | X | 1 | Y | Ok | 2700 | `RECAL 3 1 9999 845 64 4 80` |
| 1796 | 2 | Y | 1 | N | ParabolaNotDownward | 165 | `RECAL 3 0 9999 9999 64 4 80` |
| 1797 | 2 | Y | 2 | Y | Ok | 29 | `RECAL 3 0 9999 9999 200 4 80` |
| 1798 | 2 | Y | 3 | Y | Ok | 164 | `RECAL 3 0 9999 228 64 4 80` |
| 1799 | 2 | X | 1 | Y | Ok | 2652 | `RECAL 3 1 9999 229 64 4 80` |

### MPO37-12 -> MPO85-12 (MAX-MIN=2.2313)

- Run Path step: 444/576 (ch1=25 ch2=12 ch3=12 ch4=57)
- Route slot1: `RECAL 1 3 25 12 12 57`; slot2: `RECAL 1 4 25 12 12 57`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 968 | 1 | Y | 1 | Y | Ok | 550 | `RECAL 3 0 9999 9999 64 4 80` |
| 969 | 1 | X | 1 | Y | Ok | -2884 | `RECAL 3 1 9999 602 64 4 80` |
| 2233 | 2 | Y | 1 | Y | Ok | 765 | `RECAL 3 0 9999 9999 64 4 80` |
| 2234 | 2 | X | 1 | Y | Ok | -2847 | `RECAL 3 1 9999 832 64 4 80` |

### MPO39-2 -> MPO87-2 (MAX-MIN=2.2015)

- Run Path step: 458/576 (ch1=26 ch2=8 ch3=8 ch4=58)
- Route slot1: `RECAL 1 3 26 8 8 58`; slot2: `RECAL 1 4 26 8 8 58`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 999 | 1 | Y | 1 | N | ParabolaNotDownward | 330 | `RECAL 3 0 9999 9999 64 4 80` |
| 1000 | 1 | Y | 2 | Y | Ok | 194 | `RECAL 3 0 9999 9999 200 4 80` |
| 1001 | 1 | Y | 3 | Y | Ok | 336 | `RECAL 3 0 9999 400 64 4 80` |
| 1002 | 1 | X | 1 | Y | Ok | 2211 | `RECAL 3 1 9999 400 64 4 80` |
| 2261 | 2 | Y | 1 | Y | Ok | 1028 | `RECAL 3 0 9999 9999 64 4 80` |
| 2262 | 2 | X | 1 | Y | Ok | 2142 | `RECAL 3 1 9999 1088 64 4 80` |

### MPO46-7 -> MPO94-7 (MAX-MIN=1.9543)

- Run Path step: 547/576 (ch1=31 ch2=7 ch3=7 ch4=63)
- Route slot1: `RECAL 1 3 31 7 7 63`; slot2: `RECAL 1 4 31 7 7 63`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 1194 | 1 | Y | 1 | N | ParabolaNotDownward | 172 | `RECAL 3 0 9999 9999 64 4 80` |
| 1195 | 1 | Y | 2 | Y | Ok | 36 | `RECAL 3 0 9999 9999 200 4 80` |
| 1196 | 1 | Y | 3 | Y | Ok | 126 | `RECAL 3 0 9999 190 64 4 80` |
| 1197 | 1 | X | 1 | Y | Ok | 2829 | `RECAL 3 1 9999 187 64 4 80` |
| 2460 | 2 | Y | 1 | N | ParabolaNotDownward | -80 | `RECAL 3 0 9999 9999 64 4 80` |
| 2461 | 2 | Y | 2 | N | ParabolaNotDownward | -216 | `RECAL 3 0 9999 9999 200 4 80` |
| 2462 | 2 | Y | 3 | N | ParabolaNotDownward | -616 | `RECAL 3 0 9999 -416 200 4 80` |
| 2463 | 2 | Y | 4 | N | ParabolaNotDownward | 184 | `RECAL 3 0 9999 384 200 4 80` |
| 2464 | 2 | X | 1 | Y | Ok | 2903 | `RECAL 3 1 9999 -395 64 4 80` |

### MPO48-6 -> MPO96-6 (MAX-MIN=1.6695)

- Run Path step: 570/576 (ch1=32 ch2=12 ch3=12 ch4=64)
- Route slot1: `RECAL 1 3 32 12 12 64`; slot2: `RECAL 1 4 32 12 12 64`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 1246 | 1 | Y | 1 | N | ParabolaNotDownward | -149 | `RECAL 3 0 9999 9999 64 4 80` |
| 1247 | 1 | Y | 2 | N | ParabolaNotDownward | -285 | `RECAL 3 0 9999 9999 200 4 80` |
| 1248 | 1 | Y | 3 | N | ParabolaNotDownward | -685 | `RECAL 3 0 9999 -485 200 4 80` |
| 1249 | 1 | Y | 4 | N | ParabolaNotDownward | 115 | `RECAL 3 0 9999 315 200 4 80` |
| 1250 | 1 | X | 1 | Y | Ok | -2868 | `RECAL 3 1 9999 -465 64 4 80` |
| 2520 | 2 | Y | 1 | Y | Ok | -729 | `RECAL 3 0 9999 9999 64 4 80` |
| 2521 | 2 | X | 1 | Y | Ok | -2868 | `RECAL 3 1 9999 -675 64 4 80` |

### MPO28-8 -> MPO76-8 (MAX-MIN=1.1819)

- Run Path step: 332/576 (ch1=19 ch2=8 ch3=8 ch4=51)
- Route slot1: `RECAL 1 3 19 8 8 51`; slot2: `RECAL 1 4 19 8 8 51`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 722 | 1 | Y | 1 | N | ParabolaNotDownward | -399 | `RECAL 3 0 9999 9999 64 4 80` |
| 723 | 1 | Y | 2 | Y | Ok | -535 | `RECAL 3 0 9999 9999 200 4 80` |
| 724 | 1 | Y | 3 | Y | Ok | -420 | `RECAL 3 0 9999 -356 64 4 80` |
| 725 | 1 | X | 1 | Y | Ok | 2151 | `RECAL 3 1 9999 -355 64 4 80` |
| 1995 | 2 | Y | 1 | Y | Ok | 621 | `RECAL 3 0 9999 9999 64 4 80` |
| 1996 | 2 | X | 1 | Y | Ok | 1834 | `RECAL 3 1 9999 686 64 4 80` |

### MPO45-1 -> MPO93-1 (MAX-MIN=1.0814)

- Run Path step: 529/576 (ch1=30 ch2=7 ch3=7 ch4=62)
- Route slot1: `RECAL 1 3 30 7 7 62`; slot2: `RECAL 1 4 30 7 7 62`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 1158 | 1 | Y | 1 | Y | Ok | -917 | `RECAL 3 0 9999 9999 64 4 80` |
| 1159 | 1 | X | 1 | Y | Ok | 2787 | `RECAL 3 1 9999 -849 64 4 80` |
| 2420 | 2 | Y | 1 | N | ParabolaNotDownward | -19 | `RECAL 3 0 9999 9999 64 4 80` |
| 2421 | 2 | Y | 2 | Y | Ok | -155 | `RECAL 3 0 9999 9999 200 4 80` |
| 2422 | 2 | Y | 3 | Y | Ok | -163 | `RECAL 3 0 9999 -99 64 4 80` |
| 2423 | 2 | X | 1 | Y | Ok | 2610 | `RECAL 3 1 9999 -104 64 4 80` |

### MPO16-7 -> MPO64-7 (MAX-MIN=0.9973)

- Run Path step: 187/576 (ch1=11 ch2=7 ch3=7 ch4=43)
- Route slot1: `RECAL 1 3 11 7 7 43`; slot2: `RECAL 1 4 11 7 7 43`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 414 | 1 | Y | 1 | Y | Ok | 841 | `RECAL 3 0 9999 9999 64 4 80` |
| 415 | 1 | X | 1 | Y | Ok | 2739 | `RECAL 3 1 9999 901 64 4 80` |
| 1684 | 2 | Y | 1 | Y | Ok | -680 | `RECAL 3 0 9999 9999 64 4 80` |
| 1685 | 2 | X | 1 | Y | Ok | 2447 | `RECAL 3 1 9999 -597 64 4 80` |

### MPO06-1 -> MPO54-1 (MAX-MIN=0.7323)

- Run Path step: 61/576 (ch1=4 ch2=7 ch3=7 ch4=36)
- Route slot1: `RECAL 1 3 4 7 7 36`; slot2: `RECAL 1 4 4 7 7 36`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 126 | 1 | Y | 1 | N | ParabolaNotDownward | -211 | `RECAL 3 0 9999 9999 64 4 80` |
| 127 | 1 | Y | 2 | Y | Ok | -347 | `RECAL 3 0 9999 9999 200 4 80` |
| 128 | 1 | Y | 3 | Y | Ok | -203 | `RECAL 3 0 9999 -139 64 4 80` |
| 129 | 1 | X | 1 | Y | Ok | 2630 | `RECAL 3 1 9999 -131 64 4 80` |
| 1392 | 2 | Y | 1 | N | ParabolaNotDownward | 31 | `RECAL 3 0 9999 9999 64 4 80` |
| 1393 | 2 | Y | 2 | N | ParabolaNotDownward | -105 | `RECAL 3 0 9999 9999 200 4 80` |
| 1394 | 2 | Y | 3 | N | ParabolaNotDownward | -505 | `RECAL 3 0 9999 -305 200 4 80` |
| 1395 | 2 | Y | 4 | N | ParabolaNotDownward | 295 | `RECAL 3 0 9999 495 200 4 80` |
| 1396 | 2 | X | 1 | Y | Ok | 2747 | `RECAL 3 1 9999 -284 64 4 80` |

### MPO19-7 -> MPO67-7 (MAX-MIN=0.6051)

- Run Path step: 223/576 (ch1=13 ch2=7 ch3=7 ch4=45)
- Route slot1: `RECAL 1 3 13 7 7 45`; slot2: `RECAL 1 4 13 7 7 45`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 490 | 1 | Y | 1 | N | ParabolaNotDownward | 10 | `RECAL 3 0 9999 9999 64 4 80` |
| 491 | 1 | Y | 2 | N | ParabolaNotDownward | -126 | `RECAL 3 0 9999 9999 200 4 80` |
| 492 | 1 | Y | 3 | Y | Ok | -526 | `RECAL 3 0 9999 -326 200 4 80` |
| 493 | 1 | Y | 4 | Y | Ok | -434 | `RECAL 3 0 9999 -370 64 4 80` |
| 494 | 1 | X | 1 | Y | Ok | 2749 | `RECAL 3 1 9999 -306 64 4 80` |
| 1760 | 2 | Y | 1 | Y | Ok | -623 | `RECAL 3 0 9999 9999 64 4 80` |
| 1761 | 2 | X | 1 | Y | Ok | 2553 | `RECAL 3 1 9999 -542 64 4 80` |

### MPO39-1 -> MPO87-1 (MAX-MIN=0.5321)

- Run Path step: 457/576 (ch1=26 ch2=7 ch3=7 ch4=58)
- Route slot1: `RECAL 1 3 26 7 7 58`; slot2: `RECAL 1 4 26 7 7 58`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 994 | 1 | Y | 1 | N | ParabolaNotDownward | 104 | `RECAL 3 0 9999 9999 64 4 80` |
| 995 | 1 | Y | 2 | N | ParabolaNotDownward | -32 | `RECAL 3 0 9999 9999 200 4 80` |
| 996 | 1 | Y | 3 | N | ParabolaNotDownward | -432 | `RECAL 3 0 9999 -232 200 4 80` |
| 997 | 1 | Y | 4 | N | ParabolaNotDownward | 368 | `RECAL 3 0 9999 568 200 4 80` |
| 998 | 1 | X | 1 | Y | Ok | 2832 | `RECAL 3 1 9999 -211 64 4 80` |
| 2259 | 2 | Y | 1 | Y | Ok | 1021 | `RECAL 3 0 9999 9999 64 4 80` |
| 2260 | 2 | X | 1 | Y | Ok | 2748 | `RECAL 3 1 9999 1084 64 4 80` |

### MPO22-7 -> MPO70-7 (MAX-MIN=0.5017)

- Run Path step: 259/576 (ch1=15 ch2=7 ch3=7 ch4=47)
- Route slot1: `RECAL 1 3 15 7 7 47`; slot2: `RECAL 1 4 15 7 7 47`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 568 | 1 | Y | 1 | N | ParabolaNotDownward | -346 | `RECAL 3 0 9999 9999 64 4 80` |
| 569 | 1 | Y | 2 | Y | Ok | -482 | `RECAL 3 0 9999 9999 200 4 80` |
| 570 | 1 | Y | 3 | Y | Ok | -333 | `RECAL 3 0 9999 -269 64 4 80` |
| 571 | 1 | X | 1 | Y | Ok | 2756 | `RECAL 3 1 9999 -270 64 4 80` |
| 1836 | 2 | Y | 1 | Y | Ok | 1002 | `RECAL 3 0 9999 9999 64 4 80` |
| 1837 | 2 | X | 1 | Y | Ok | 2568 | `RECAL 3 1 9999 1062 64 4 80` |

### MPO28-11 -> MPO76-11 (MAX-MIN=0.3002)

- Run Path step: 335/576 (ch1=19 ch2=11 ch3=11 ch4=51)
- Route slot1: `RECAL 1 3 19 11 11 51`; slot2: `RECAL 1 4 19 11 11 51`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 732 | 1 | Y | 1 | Y | Ok | -741 | `RECAL 3 0 9999 9999 64 4 80` |
| 733 | 1 | X | 1 | Y | Ok | -2159 | `RECAL 3 1 9999 -696 64 4 80` |
| 2001 | 2 | Y | 1 | Y | Ok | 615 | `RECAL 3 0 9999 9999 64 4 80` |
| 2002 | 2 | X | 1 | Y | Ok | -2248 | `RECAL 3 1 9999 672 64 4 80` |

### MPO08-1 -> MPO56-1 (MAX-MIN=0.2707)

- Run Path step: 85/576 (ch1=5 ch2=13 ch3=13 ch4=37)
- Route slot1: `RECAL 1 3 5 13 13 37`; slot2: `RECAL 1 4 5 13 13 37`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 194 | 1 | Y | 1 | Y | Ok | -2165 | `RECAL 3 0 9999 9999 64 4 80` |
| 195 | 1 | X | 1 | Y | Ok | -2705 | `RECAL 3 1 9999 -2105 64 4 80` |
| 1454 | 2 | Y | 1 | Y | Ok | -2431 | `RECAL 3 0 9999 9999 64 4 80` |
| 1455 | 2 | X | 1 | Y | Ok | -2904 | `RECAL 3 1 9999 -2372 64 4 80` |

### MPO08-4 -> MPO56-4 (MAX-MIN=0.2545)

- Run Path step: 88/576 (ch1=5 ch2=16 ch3=16 ch4=37)
- Route slot1: `RECAL 1 3 5 16 16 37`; slot2: `RECAL 1 4 5 16 16 37`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 200 | 1 | Y | 1 | Y | Ok | -2163 | `RECAL 3 0 9999 9999 64 4 80` |
| 201 | 1 | X | 1 | Y | Ok | 1156 | `RECAL 3 1 9999 -2103 64 4 80` |
| 1460 | 2 | Y | 1 | Y | Ok | -2476 | `RECAL 3 0 9999 9999 64 4 80` |
| 1461 | 2 | X | 1 | Y | Ok | 1272 | `RECAL 3 1 9999 -2419 64 4 80` |

### MPO08-8 -> MPO56-8 (MAX-MIN=0.2404)

- Run Path step: 92/576 (ch1=6 ch2=2 ch3=2 ch4=38)
- Route slot1: `RECAL 1 3 6 2 2 38`; slot2: `RECAL 1 4 6 2 2 38`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 208 | 1 | Y | 1 | Y | Ok | 1965 | `RECAL 3 0 9999 9999 64 4 80` |
| 209 | 1 | X | 1 | Y | Ok | -2144 | `RECAL 3 1 9999 2034 64 4 80` |
| 1468 | 2 | Y | 1 | Y | Ok | 2035 | `RECAL 3 0 9999 9999 64 4 80` |
| 1469 | 2 | X | 1 | Y | Ok | -2096 | `RECAL 3 1 9999 2103 64 4 80` |

### MPO08-6 -> MPO56-6 (MAX-MIN=0.2094)

- Run Path step: 90/576 (ch1=5 ch2=18 ch3=18 ch4=37)
- Route slot1: `RECAL 1 3 5 18 18 37`; slot2: `RECAL 1 4 5 18 18 37`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 204 | 1 | Y | 1 | Y | Ok | -2172 | `RECAL 3 0 9999 9999 64 4 80` |
| 205 | 1 | X | 1 | Y | Ok | 2589 | `RECAL 3 1 9999 -2109 64 4 80` |
| 1464 | 2 | Y | 1 | Y | Ok | -2594 | `RECAL 3 0 9999 9999 64 4 80` |
| 1465 | 2 | X | 1 | Y | Ok | 2806 | `RECAL 3 1 9999 -2537 64 4 80` |

### MPO08-12 -> MPO56-12 (MAX-MIN=0.1885)

- Run Path step: 96/576 (ch1=6 ch2=6 ch3=6 ch4=38)
- Route slot1: `RECAL 1 3 6 6 6 38`; slot2: `RECAL 1 4 6 6 6 38`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 216 | 1 | Y | 1 | Y | Ok | 2049 | `RECAL 3 0 9999 9999 64 4 80` |
| 217 | 1 | X | 1 | Y | Ok | 2594 | `RECAL 3 1 9999 2119 64 4 80` |
| 1476 | 2 | Y | 1 | Y | Ok | 2096 | `RECAL 3 0 9999 9999 64 4 80` |
| 1477 | 2 | X | 1 | Y | Ok | 2580 | `RECAL 3 1 9999 2162 64 4 80` |

### MPO08-3 -> MPO56-3 (MAX-MIN=0.1696)

- Run Path step: 87/576 (ch1=5 ch2=15 ch3=15 ch4=37)
- Route slot1: `RECAL 1 3 5 15 15 37`; slot2: `RECAL 1 4 5 15 15 37`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 198 | 1 | Y | 1 | Y | Ok | -2157 | `RECAL 3 0 9999 9999 64 4 80` |
| 199 | 1 | X | 1 | Y | Ok | -1253 | `RECAL 3 1 9999 -2100 64 4 80` |
| 1458 | 2 | Y | 1 | Y | Ok | -2460 | `RECAL 3 0 9999 9999 64 4 80` |
| 1459 | 2 | X | 1 | Y | Ok | -1333 | `RECAL 3 1 9999 -2401 64 4 80` |

