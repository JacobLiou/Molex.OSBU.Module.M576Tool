# IL vs Run Path (2026-07-08)

Sources: validationIL.png + comm recal_sweeps.csv

Rule: step = (ch1-1)*18 + ch2

## Summary (MAX-MIN desc)

| MAX-MIN | step | ch1/ch4 | ch2/ch3 | MPO in->out | sweeps | fails | note |
|--------:|-----:|---------|---------|-------------|--------:|------:|------|
| 3.5917 | 494/576 | 28/60 | 8/8 | MPO42-2->MPO90-2 | 7 | 4 | slot2Y att1 ParabolaNotDownward; slot2Y att2 ParabolaNotDownward; slot2Y att3 ParabolaNotDownward; slot2Y att4 ParabolaNotDownward |
| 2.9899 | 551/576 | 31/63 | 11/11 | MPO46-11->MPO94-11 | 7 | 4 | slot2Y att1 ParabolaNotDownward; slot2Y att2 ParabolaNotDownward; slot2Y att3 ParabolaNotDownward; slot2Y att4 ParabolaNotDownward |
| 2.3530 | 241/576 | 14/46 | 7/7 | MPO21-1->MPO69-1 | 7 | 4 | slot2Y att1 ParabolaNotDownward; slot2Y att2 VertexOutOfRange; slot2Y att3 ParabolaNotDownward; slot2Y att4 ParabolaNotDownward |
| 2.2313 | 444/576 | 25/57 | 12/12 | MPO37-12->MPO85-12 | 7 | 4 | slot2Y att1 ParabolaNotDownward; slot2Y att2 ParabolaNotDownward; slot2Y att3 ParabolaNotDownward; slot2Y att4 ParabolaNotDownward |
| 2.2015 | 458/576 | 26/58 | 8/8 | MPO39-2->MPO87-2 | 7 | 4 | slot1Y att1 ParabolaNotDownward; slot1Y att2 ParabolaNotDownward; slot1Y att3 ParabolaNotDownward; slot1Y att4 ParabolaNotDownward |
| 1.9543 | 547/576 | 31/63 | 7/7 | MPO46-7->MPO94-7 | 9 | 5 | slot1Y att1 ParabolaNotDownward; slot1Y att2 ParabolaNotDownward; slot1Y att3 ParabolaNotDownward; slot1Y att4 ParabolaNotDownward; slot2Y att1 ParabolaNotDownward |
| 1.6695 | 570/576 | 32/64 | 12/12 | MPO48-6->MPO96-6 | 7 | 4 | slot1Y att1 ParabolaNotDownward; slot1Y att2 ParabolaNotDownward; slot1Y att3 ParabolaNotDownward; slot1Y att4 ParabolaNotDownward |
| 1.1819 | 332/576 | 19/51 | 8/8 | MPO28-8->MPO76-8 | 9 | 5 | slot1Y att1 ParabolaNotDownward; slot2Y att1 ParabolaNotDownward; slot2Y att2 ParabolaNotDownward; slot2Y att3 ParabolaNotDownward; slot2Y att4 ParabolaNotDownward |
| 1.0814 | 529/576 | 30/62 | 7/7 | MPO45-1->MPO93-1 | 7 | 4 | slot2Y att1 ParabolaNotDownward; slot2Y att2 ParabolaNotDownward; slot2Y att3 ParabolaNotDownward; slot2Y att4 ParabolaNotDownward |
| 0.9973 | 187/576 | 11/43 | 7/7 | MPO16-7->MPO64-7 | 9 | 5 | slot1Y att1 ParabolaNotDownward; slot1Y att2 ParabolaNotDownward; slot1Y att3 ParabolaNotDownward; slot1Y att4 ParabolaNotDownward; slot2Y att1 ParabolaNotDownward |
| 0.7323 | 61/576 | 4/36 | 7/7 | MPO06-1->MPO54-1 | 9 | 5 | slot1Y att1 ParabolaNotDownward; slot2Y att1 ParabolaNotDownward; slot2Y att2 VertexOutOfRange; slot2Y att3 ParabolaNotDownward; slot2Y att4 ParabolaNotDownward |
| 0.6051 | 223/576 | 13/45 | 7/7 | MPO19-7->MPO67-7 | 7 | 4 | slot2Y att1 ParabolaNotDownward; slot2Y att2 ParabolaNotDownward; slot2Y att3 ParabolaNotDownward; slot2Y att4 ParabolaNotDownward |
| 0.5321 | 457/576 | 26/58 | 7/7 | MPO39-1->MPO87-1 | 7 | 4 | slot1Y att1 ParabolaNotDownward; slot1Y att2 ParabolaNotDownward; slot1Y att3 ParabolaNotDownward; slot1Y att4 ParabolaNotDownward |
| 0.5017 | 259/576 | 15/47 | 7/7 | MPO22-7->MPO70-7 | 7 | 4 | slot1Y att1 ParabolaNotDownward; slot1Y att2 ParabolaNotDownward; slot1Y att3 ParabolaNotDownward; slot1Y att4 ParabolaNotDownward |
| 0.3002 | 335/576 | 19/51 | 11/11 | MPO28-11->MPO76-11 | 7 | 4 | slot1Y att1 ParabolaNotDownward; slot1Y att2 ParabolaNotDownward; slot1Y att3 ParabolaNotDownward; slot1Y att4 ParabolaNotDownward |
| 0.2707 | 85/576 | 5/37 | 13/13 | MPO08-1->MPO56-1 | 4 | 0 | all peak_ok=1 |
| 0.2545 | 88/576 | 5/37 | 16/16 | MPO08-4->MPO56-4 | 4 | 0 | all peak_ok=1 |
| 0.2404 | 92/576 | 6/38 | 2/2 | MPO08-8->MPO56-8 | 4 | 0 | all peak_ok=1 |
| 0.2094 | 90/576 | 5/37 | 18/18 | MPO08-6->MPO56-6 | 4 | 0 | all peak_ok=1 |
| 0.1885 | 96/576 | 6/38 | 6/6 | MPO08-12->MPO56-12 | 4 | 0 | all peak_ok=1 |
| 0.1696 | 87/576 | 5/37 | 15/15 | MPO08-3->MPO56-3 | 4 | 0 | all peak_ok=1 |

## Detail per path

### MPO42-2 -> MPO90-2 (IL=3.5917)

- step 494/576 | ch1=28 ch2=8 ch3=8 ch4=60
- `RECAL 1 3 28 8 8 60` / `RECAL 1 4 28 8 8 60`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 1072 | 1 | Y | 1 | Y | Ok | 480 | `RECAL 3 0 9999 9999 64 4 80` |
| 1073 | 1 | X | 1 | Y | Ok | 1980 | `RECAL 3 1 9999 536 64 4 80` |
| 2367 | 2 | Y | 1 | N | ParabolaNotDownward | -147 | `RECAL 3 0 9999 9999 64 4 80` |
| 2368 | 2 | Y | 2 | N | ParabolaNotDownward | -283 | `RECAL 3 0 9999 9999 200 4 80` |
| 2369 | 2 | Y | 3 | N | ParabolaNotDownward | -683 | `RECAL 3 0 9999 -483 200 4 80` |
| 2370 | 2 | Y | 4 | N | ParabolaNotDownward | 117 | `RECAL 3 0 9999 317 200 4 80` |
| 2371 | 2 | X | 1 | Y | Ok | 2131 | `RECAL 3 1 9999 -463 64 4 80` |

### MPO46-11 -> MPO94-11 (IL=2.9899)

- step 551/576 | ch1=31 ch2=11 ch3=11 ch4=63
- `RECAL 1 3 31 11 11 63` / `RECAL 1 4 31 11 11 63`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 1193 | 1 | Y | 1 | Y | Ok | 478 | `RECAL 3 0 9999 9999 64 4 80` |
| 1194 | 1 | X | 1 | Y | Ok | -2232 | `RECAL 3 1 9999 539 64 4 80` |
| 2501 | 2 | Y | 1 | N | ParabolaNotDownward | -179 | `RECAL 3 0 9999 9999 64 4 80` |
| 2502 | 2 | Y | 2 | N | ParabolaNotDownward | -315 | `RECAL 3 0 9999 9999 200 4 80` |
| 2503 | 2 | Y | 3 | N | ParabolaNotDownward | -715 | `RECAL 3 0 9999 -515 200 4 80` |
| 2504 | 2 | Y | 4 | N | ParabolaNotDownward | 85 | `RECAL 3 0 9999 285 200 4 80` |
| 2505 | 2 | X | 1 | Y | Ok | -2223 | `RECAL 3 1 9999 -495 64 4 80` |

### MPO21-1 -> MPO69-1 (IL=2.3530)

- step 241/576 | ch1=14 ch2=7 ch3=7 ch4=46
- `RECAL 1 3 14 7 7 46` / `RECAL 1 4 14 7 7 46`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 513 | 1 | Y | 1 | Y | Ok | -922 | `RECAL 3 0 9999 9999 64 4 80` |
| 514 | 1 | X | 1 | Y | Ok | 2811 | `RECAL 3 1 9999 -868 64 4 80` |
| 1798 | 2 | Y | 1 | N | ParabolaNotDownward | -111 | `RECAL 3 0 9999 9999 64 4 80` |
| 1799 | 2 | Y | 2 | N | VertexOutOfRange | -247 | `RECAL 3 0 9999 9999 200 4 80` |
| 1800 | 2 | Y | 3 | N | ParabolaNotDownward | -647 | `RECAL 3 0 9999 -447 200 4 80` |
| 1801 | 2 | Y | 4 | N | ParabolaNotDownward | 153 | `RECAL 3 0 9999 353 200 4 80` |
| 1802 | 2 | X | 1 | Y | Ok | 2725 | `RECAL 3 1 9999 -427 64 4 80` |

### MPO37-12 -> MPO85-12 (IL=2.2313)

- step 444/576 | ch1=25 ch2=12 ch3=12 ch4=57
- `RECAL 1 3 25 12 12 57` / `RECAL 1 4 25 12 12 57`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 960 | 1 | Y | 1 | Y | Ok | -711 | `RECAL 3 0 9999 9999 64 4 80` |
| 961 | 1 | X | 1 | Y | Ok | -2886 | `RECAL 3 1 9999 -649 64 4 80` |
| 2258 | 2 | Y | 1 | N | ParabolaNotDownward | -140 | `RECAL 3 0 9999 9999 64 4 80` |
| 2259 | 2 | Y | 2 | N | ParabolaNotDownward | -276 | `RECAL 3 0 9999 9999 200 4 80` |
| 2260 | 2 | Y | 3 | N | ParabolaNotDownward | -676 | `RECAL 3 0 9999 -476 200 4 80` |
| 2261 | 2 | Y | 4 | N | ParabolaNotDownward | 124 | `RECAL 3 0 9999 324 200 4 80` |
| 2262 | 2 | X | 1 | Y | Ok | -2760 | `RECAL 3 1 9999 -455 64 4 80` |

### MPO39-2 -> MPO87-2 (IL=2.2015)

- step 458/576 | ch1=26 ch2=8 ch3=8 ch4=58
- `RECAL 1 3 26 8 8 58` / `RECAL 1 4 26 8 8 58`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 991 | 1 | Y | 1 | N | ParabolaNotDownward | -104 | `RECAL 3 0 9999 9999 64 4 80` |
| 992 | 1 | Y | 2 | N | ParabolaNotDownward | -240 | `RECAL 3 0 9999 9999 200 4 80` |
| 993 | 1 | Y | 3 | N | ParabolaNotDownward | -640 | `RECAL 3 0 9999 -440 200 4 80` |
| 994 | 1 | Y | 4 | N | ParabolaNotDownward | 160 | `RECAL 3 0 9999 360 200 4 80` |
| 995 | 1 | X | 1 | Y | Ok | 2201 | `RECAL 3 1 9999 -420 64 4 80` |
| 2289 | 2 | Y | 1 | Y | Ok | 734 | `RECAL 3 0 9999 9999 64 4 80` |
| 2290 | 2 | X | 1 | Y | Ok | 2137 | `RECAL 3 1 9999 798 64 4 80` |

### MPO46-7 -> MPO94-7 (IL=1.9543)

- step 547/576 | ch1=31 ch2=7 ch3=7 ch4=63
- `RECAL 1 3 31 7 7 63` / `RECAL 1 4 31 7 7 63`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 1180 | 1 | Y | 1 | N | ParabolaNotDownward | -117 | `RECAL 3 0 9999 9999 64 4 80` |
| 1181 | 1 | Y | 2 | N | ParabolaNotDownward | -253 | `RECAL 3 0 9999 9999 200 4 80` |
| 1182 | 1 | Y | 3 | N | ParabolaNotDownward | -653 | `RECAL 3 0 9999 -453 200 4 80` |
| 1183 | 1 | Y | 4 | N | ParabolaNotDownward | 147 | `RECAL 3 0 9999 347 200 4 80` |
| 1184 | 1 | X | 1 | Y | Ok | 2686 | `RECAL 3 1 9999 -433 64 4 80` |
| 2489 | 2 | Y | 1 | N | ParabolaNotDownward | 239 | `RECAL 3 0 9999 9999 64 4 80` |
| 2490 | 2 | Y | 2 | Y | Ok | 103 | `RECAL 3 0 9999 9999 200 4 80` |
| 2491 | 2 | Y | 3 | Y | Ok | 233 | `RECAL 3 0 9999 297 64 4 80` |
| 2492 | 2 | X | 1 | Y | Ok | 2796 | `RECAL 3 1 9999 297 64 4 80` |

### MPO48-6 -> MPO96-6 (IL=1.6695)

- step 570/576 | ch1=32 ch2=12 ch3=12 ch4=64
- `RECAL 1 3 32 12 12 64` / `RECAL 1 4 32 12 12 64`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 1237 | 1 | Y | 1 | N | ParabolaNotDownward | -101 | `RECAL 3 0 9999 9999 64 4 80` |
| 1238 | 1 | Y | 2 | N | ParabolaNotDownward | -237 | `RECAL 3 0 9999 9999 200 4 80` |
| 1239 | 1 | Y | 3 | N | ParabolaNotDownward | -637 | `RECAL 3 0 9999 -437 200 4 80` |
| 1240 | 1 | Y | 4 | N | ParabolaNotDownward | 163 | `RECAL 3 0 9999 363 200 4 80` |
| 1241 | 1 | X | 1 | Y | Ok | -2847 | `RECAL 3 1 9999 -417 64 4 80` |
| 2542 | 2 | Y | 1 | Y | Ok | -805 | `RECAL 3 0 9999 9999 64 4 80` |
| 2543 | 2 | X | 1 | Y | Ok | -2896 | `RECAL 3 1 9999 -737 64 4 80` |

### MPO28-8 -> MPO76-8 (IL=1.1819)

- step 332/576 | ch1=19 ch2=8 ch3=8 ch4=51
- `RECAL 1 3 19 8 8 51` / `RECAL 1 4 19 8 8 51`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 711 | 1 | Y | 1 | N | ParabolaNotDownward | 212 | `RECAL 3 0 9999 9999 64 4 80` |
| 712 | 1 | Y | 2 | Y | Ok | 76 | `RECAL 3 0 9999 9999 200 4 80` |
| 713 | 1 | Y | 3 | Y | Ok | 190 | `RECAL 3 0 9999 254 64 4 80` |
| 714 | 1 | X | 1 | Y | Ok | 2113 | `RECAL 3 1 9999 253 64 4 80` |
| 2009 | 2 | Y | 1 | N | ParabolaNotDownward | -34 | `RECAL 3 0 9999 9999 64 4 80` |
| 2010 | 2 | Y | 2 | N | ParabolaNotDownward | -170 | `RECAL 3 0 9999 9999 200 4 80` |
| 2011 | 2 | Y | 3 | N | ParabolaNotDownward | -570 | `RECAL 3 0 9999 -370 200 4 80` |
| 2012 | 2 | Y | 4 | N | ParabolaNotDownward | 230 | `RECAL 3 0 9999 430 200 4 80` |
| 2013 | 2 | X | 1 | Y | Ok | 1976 | `RECAL 3 1 9999 -350 64 4 80` |

### MPO45-1 -> MPO93-1 (IL=1.0814)

- step 529/576 | ch1=30 ch2=7 ch3=7 ch4=62
- `RECAL 1 3 30 7 7 62` / `RECAL 1 4 30 7 7 62`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 1144 | 1 | Y | 1 | Y | Ok | 804 | `RECAL 3 0 9999 9999 64 4 80` |
| 1145 | 1 | X | 1 | Y | Ok | 2722 | `RECAL 3 1 9999 860 64 4 80` |
| 2444 | 2 | Y | 1 | N | ParabolaNotDownward | -74 | `RECAL 3 0 9999 9999 64 4 80` |
| 2445 | 2 | Y | 2 | N | ParabolaNotDownward | -210 | `RECAL 3 0 9999 9999 200 4 80` |
| 2446 | 2 | Y | 3 | N | ParabolaNotDownward | -610 | `RECAL 3 0 9999 -410 200 4 80` |
| 2447 | 2 | Y | 4 | N | ParabolaNotDownward | 190 | `RECAL 3 0 9999 390 200 4 80` |
| 2448 | 2 | X | 1 | Y | Ok | 2854 | `RECAL 3 1 9999 -389 64 4 80` |

### MPO16-7 -> MPO64-7 (IL=0.9973)

- step 187/576 | ch1=11 ch2=7 ch3=7 ch4=43
- `RECAL 1 3 11 7 7 43` / `RECAL 1 4 11 7 7 43`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 394 | 1 | Y | 1 | N | ParabolaNotDownward | -70 | `RECAL 3 0 9999 9999 64 4 80` |
| 395 | 1 | Y | 2 | N | ParabolaNotDownward | -206 | `RECAL 3 0 9999 9999 200 4 80` |
| 396 | 1 | Y | 3 | N | ParabolaNotDownward | -606 | `RECAL 3 0 9999 -406 200 4 80` |
| 397 | 1 | Y | 4 | N | ParabolaNotDownward | 194 | `RECAL 3 0 9999 394 200 4 80` |
| 398 | 1 | X | 1 | Y | Ok | 2787 | `RECAL 3 1 9999 -385 64 4 80` |
| 1681 | 2 | Y | 1 | N | ParabolaNotDownward | 290 | `RECAL 3 0 9999 9999 64 4 80` |
| 1682 | 2 | Y | 2 | Y | Ok | 154 | `RECAL 3 0 9999 9999 200 4 80` |
| 1683 | 2 | Y | 3 | Y | Ok | 268 | `RECAL 3 0 9999 332 64 4 80` |
| 1684 | 2 | X | 1 | Y | Ok | 2760 | `RECAL 3 1 9999 331 64 4 80` |

### MPO06-1 -> MPO54-1 (IL=0.7323)

- step 61/576 | ch1=4 ch2=7 ch3=7 ch4=36
- `RECAL 1 3 4 7 7 36` / `RECAL 1 4 4 7 7 36`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 126 | 1 | Y | 1 | N | ParabolaNotDownward | -373 | `RECAL 3 0 9999 9999 64 4 80` |
| 127 | 1 | Y | 2 | Y | Ok | -509 | `RECAL 3 0 9999 9999 200 4 80` |
| 128 | 1 | Y | 3 | Y | Ok | -355 | `RECAL 3 0 9999 -291 64 4 80` |
| 129 | 1 | X | 1 | Y | Ok | 2597 | `RECAL 3 1 9999 -291 64 4 80` |
| 1384 | 2 | Y | 1 | N | ParabolaNotDownward | -68 | `RECAL 3 0 9999 9999 64 4 80` |
| 1385 | 2 | Y | 2 | N | VertexOutOfRange | -204 | `RECAL 3 0 9999 9999 200 4 80` |
| 1386 | 2 | Y | 3 | N | ParabolaNotDownward | -604 | `RECAL 3 0 9999 -404 200 4 80` |
| 1387 | 2 | Y | 4 | N | ParabolaNotDownward | 196 | `RECAL 3 0 9999 396 200 4 80` |
| 1388 | 2 | X | 1 | Y | Ok | 2686 | `RECAL 3 1 9999 -384 64 4 80` |

### MPO19-7 -> MPO67-7 (IL=0.6051)

- step 223/576 | ch1=13 ch2=7 ch3=7 ch4=45
- `RECAL 1 3 13 7 7 45` / `RECAL 1 4 13 7 7 45`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 477 | 1 | Y | 1 | Y | Ok | 843 | `RECAL 3 0 9999 9999 64 4 80` |
| 478 | 1 | X | 1 | Y | Ok | 2768 | `RECAL 3 1 9999 874 64 4 80` |
| 1757 | 2 | Y | 1 | N | ParabolaNotDownward | -103 | `RECAL 3 0 9999 9999 64 4 80` |
| 1758 | 2 | Y | 2 | N | ParabolaNotDownward | -239 | `RECAL 3 0 9999 9999 200 4 80` |
| 1759 | 2 | Y | 3 | N | ParabolaNotDownward | -639 | `RECAL 3 0 9999 -439 200 4 80` |
| 1760 | 2 | Y | 4 | N | ParabolaNotDownward | 161 | `RECAL 3 0 9999 361 200 4 80` |
| 1761 | 2 | X | 1 | Y | Ok | 2926 | `RECAL 3 1 9999 -418 64 4 80` |

### MPO39-1 -> MPO87-1 (IL=0.5321)

- step 457/576 | ch1=26 ch2=7 ch3=7 ch4=58
- `RECAL 1 3 26 7 7 58` / `RECAL 1 4 26 7 7 58`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 986 | 1 | Y | 1 | N | ParabolaNotDownward | -119 | `RECAL 3 0 9999 9999 64 4 80` |
| 987 | 1 | Y | 2 | N | ParabolaNotDownward | -255 | `RECAL 3 0 9999 9999 200 4 80` |
| 988 | 1 | Y | 3 | N | ParabolaNotDownward | -655 | `RECAL 3 0 9999 -455 200 4 80` |
| 989 | 1 | Y | 4 | N | ParabolaNotDownward | 145 | `RECAL 3 0 9999 345 200 4 80` |
| 990 | 1 | X | 1 | Y | Ok | 2830 | `RECAL 3 1 9999 -435 64 4 80` |
| 2287 | 2 | Y | 1 | Y | Ok | 753 | `RECAL 3 0 9999 9999 64 4 80` |
| 2288 | 2 | X | 1 | Y | Ok | 2718 | `RECAL 3 1 9999 820 64 4 80` |

### MPO22-7 -> MPO70-7 (IL=0.5017)

- step 259/576 | ch1=15 ch2=7 ch3=7 ch4=47
- `RECAL 1 3 15 7 7 47` / `RECAL 1 4 15 7 7 47`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 549 | 1 | Y | 1 | N | ParabolaNotDownward | -74 | `RECAL 3 0 9999 9999 64 4 80` |
| 550 | 1 | Y | 2 | N | ParabolaNotDownward | -210 | `RECAL 3 0 9999 9999 200 4 80` |
| 551 | 1 | Y | 3 | N | ParabolaNotDownward | -610 | `RECAL 3 0 9999 -410 200 4 80` |
| 552 | 1 | Y | 4 | N | ParabolaNotDownward | 190 | `RECAL 3 0 9999 390 200 4 80` |
| 553 | 1 | X | 1 | Y | Ok | 2816 | `RECAL 3 1 9999 -390 64 4 80` |
| 1841 | 2 | Y | 1 | Y | Ok | 1101 | `RECAL 3 0 9999 9999 64 4 80` |
| 1842 | 2 | X | 1 | Y | Ok | 2724 | `RECAL 3 1 9999 1165 64 4 80` |

### MPO28-11 -> MPO76-11 (IL=0.3002)

- step 335/576 | ch1=19 ch2=11 ch3=11 ch4=51
- `RECAL 1 3 19 11 11 51` / `RECAL 1 4 19 11 11 51`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 719 | 1 | Y | 1 | N | ParabolaNotDownward | -1 | `RECAL 3 0 9999 9999 64 4 80` |
| 720 | 1 | Y | 2 | N | ParabolaNotDownward | -137 | `RECAL 3 0 9999 9999 200 4 80` |
| 721 | 1 | Y | 3 | N | ParabolaNotDownward | -537 | `RECAL 3 0 9999 -337 200 4 80` |
| 722 | 1 | Y | 4 | N | ParabolaNotDownward | 263 | `RECAL 3 0 9999 463 200 4 80` |
| 723 | 1 | X | 1 | Y | Ok | -2315 | `RECAL 3 1 9999 -317 64 4 80` |
| 2022 | 2 | Y | 1 | Y | Ok | -581 | `RECAL 3 0 9999 9999 64 4 80` |
| 2023 | 2 | X | 1 | Y | Ok | -2336 | `RECAL 3 1 9999 -509 64 4 80` |

### MPO08-1 -> MPO56-1 (IL=0.2707)

- step 85/576 | ch1=5 ch2=13 ch3=13 ch4=37
- `RECAL 1 3 5 13 13 37` / `RECAL 1 4 5 13 13 37`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 176 | 1 | Y | 1 | Y | Ok | -2084 | `RECAL 3 0 9999 9999 64 4 80` |
| 177 | 1 | X | 1 | Y | Ok | -2769 | `RECAL 3 1 9999 -2024 64 4 80` |
| 1449 | 2 | Y | 1 | Y | Ok | -2238 | `RECAL 3 0 9999 9999 64 4 80` |
| 1450 | 2 | X | 1 | Y | Ok | -2733 | `RECAL 3 1 9999 -2172 64 4 80` |

### MPO08-4 -> MPO56-4 (IL=0.2545)

- step 88/576 | ch1=5 ch2=16 ch3=16 ch4=37
- `RECAL 1 3 5 16 16 37` / `RECAL 1 4 5 16 16 37`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 182 | 1 | Y | 1 | Y | Ok | -2159 | `RECAL 3 0 9999 9999 64 4 80` |
| 183 | 1 | X | 1 | Y | Ok | 1132 | `RECAL 3 1 9999 -2099 64 4 80` |
| 1455 | 2 | Y | 1 | Y | Ok | -2319 | `RECAL 3 0 9999 9999 64 4 80` |
| 1456 | 2 | X | 1 | Y | Ok | 1160 | `RECAL 3 1 9999 -2257 64 4 80` |

### MPO08-8 -> MPO56-8 (IL=0.2404)

- step 92/576 | ch1=6 ch2=2 ch3=2 ch4=38
- `RECAL 1 3 6 2 2 38` / `RECAL 1 4 6 2 2 38`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 190 | 1 | Y | 1 | Y | Ok | 2294 | `RECAL 3 0 9999 9999 64 4 80` |
| 191 | 1 | X | 1 | Y | Ok | -2106 | `RECAL 3 1 9999 2356 64 4 80` |
| 1463 | 2 | Y | 1 | Y | Ok | 2140 | `RECAL 3 0 9999 9999 64 4 80` |
| 1464 | 2 | X | 1 | Y | Ok | -2286 | `RECAL 3 1 9999 2203 64 4 80` |

### MPO08-6 -> MPO56-6 (IL=0.2094)

- step 90/576 | ch1=5 ch2=18 ch3=18 ch4=37
- `RECAL 1 3 5 18 18 37` / `RECAL 1 4 5 18 18 37`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 186 | 1 | Y | 1 | Y | Ok | -2052 | `RECAL 3 0 9999 9999 64 4 80` |
| 187 | 1 | X | 1 | Y | Ok | 2610 | `RECAL 3 1 9999 -1995 64 4 80` |
| 1459 | 2 | Y | 1 | Y | Ok | -2255 | `RECAL 3 0 9999 9999 64 4 80` |
| 1460 | 2 | X | 1 | Y | Ok | 2608 | `RECAL 3 1 9999 -2192 64 4 80` |

### MPO08-12 -> MPO56-12 (IL=0.1885)

- step 96/576 | ch1=6 ch2=6 ch3=6 ch4=38
- `RECAL 1 3 6 6 6 38` / `RECAL 1 4 6 6 6 38`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 198 | 1 | Y | 1 | Y | Ok | 2300 | `RECAL 3 0 9999 9999 64 4 80` |
| 199 | 1 | X | 1 | Y | Ok | 2605 | `RECAL 3 1 9999 2362 64 4 80` |
| 1471 | 2 | Y | 1 | Y | Ok | 2284 | `RECAL 3 0 9999 9999 64 4 80` |
| 1472 | 2 | X | 1 | Y | Ok | 2742 | `RECAL 3 1 9999 2347 64 4 80` |

### MPO08-3 -> MPO56-3 (IL=0.1696)

- step 87/576 | ch1=5 ch2=15 ch3=15 ch4=37
- `RECAL 1 3 5 15 15 37` / `RECAL 1 4 5 15 15 37`

| csv_line | slot | axis | att | ok | code | col0 | cmd |
|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|
| 180 | 1 | Y | 1 | Y | Ok | -2163 | `RECAL 3 0 9999 9999 64 4 80` |
| 181 | 1 | X | 1 | Y | Ok | -1323 | `RECAL 3 1 9999 -2104 64 4 80` |
| 1453 | 2 | Y | 1 | Y | Ok | -2251 | `RECAL 3 0 9999 9999 64 4 80` |
| 1454 | 2 | X | 1 | Y | Ok | -1279 | `RECAL 3 1 9999 -2186 64 4 80` |

