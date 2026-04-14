## Plan: M576 閫氫俊鍗忚鏃ュ織

鍩轰簬鐜版湁鐨?CRecalSession銆乑4671Command銆丮csFwTransport 鍜?CM576CalibratorDlg锛屽鍔犱竴濂楀垎灞傞€氫俊鏃ュ織鏈哄埗锛欰SCII RECAL 鍘熸枃銆乑4671 浜岃繘鍒跺抚 hex dump銆佸叧閿祦绋嬬姸鎬併€侀敊璇?瓒呮椂鍜岃€楁椂閮界粺涓€杩涘叆 UI 涓庢枃浠躲€傛帹鑽愬仛娉曟槸鍦ㄤ細璇濆眰鍜屽懡浠ゅ眰澧炲姞杞婚噺 Logger 鎺ュ彛锛岃€屼笉鏄暎钀藉湴鐩存帴璋冪敤 AppendLog锛涜繖鏍锋棦鑳芥渶灏忎镜鍏ョ幇鏈夋祦绋嬶紝鍙堣兘瑕嗙洊鈥滆繍琛岃繃绋嬩腑鎵€鏈夋寚浠や氦浜掆€濄€?

**Steps**
1. 鐩樼偣骞跺浐瀹氭棩蹇楄鐩栬寖鍥达細绾冲叆 RECAL 0/1/2/3/5 鐨勫彂閫佷笌鍝嶅簲銆丗W Start/Chunk/End銆丟etLogFileData 璇?flash 杩囩▼銆丱penPort 澶辫触鍜岃秴鏃堕敊璇紱鏄庣‘涓嶈鐩?CSV 瑙ｆ瀽銆丩UT merge 绾畻娉曞唴閮ㄧ粏鑺傘€傝繖涓楠や负鍚庣画瀛楁璁捐鎻愪緵杈圭晫銆?
2. 瀹氫箟缁熶竴鏃ュ織鎶借薄锛屾柊澧炰竴涓交閲忔帴鍙ｆ垨鍥炶皟灞傦紝鏀寔鏈€灏戣繖浜涘瓧娈碉細鏃堕棿鎴炽€佹柟鍚戯紙SEND/RECV/INFO/ERROR锛夈€侀€氶亾锛圧ECAL/Z4671/FW锛夈€佸簭鍙枫€佹枃鏈憳瑕併€佸師濮?payload銆佽€楁椂銆傛帹鑽愭斁鍦?M576CalibratorApp 鍐咃紝閬垮厤渚靛叆 Z4671Core 澶銆?鍚庣画姝ラ渚濊禆姝ゆ楠ゃ€?
3. 鍦?ASCII 閫氫俊灞傛帴鍏ユ棩蹇楋細淇敼 CRecalSession 鐨?SendRecal0/1/2/3/5 鍜?ReadAsciiResponse/ReadLineBlocking锛屽湪鍐欎覆鍙ｅ墠鍚庡拰鏀跺埌鏁磋鍝嶅簲鍚庤褰曟棩蹇楋紝淇濈暀鍘熸湁閿欒杩斿洖鏂瑰紡銆傞噸鐐规槸鎶婃瘡鏉?RECAL 鍛戒护涓庡搴斿搷搴旀寜搴忓彿鍏宠仈锛屽苟璁板綍 ReadAsciiResponse 鐨勮秴鏃舵椂闂村拰瀹為檯鑰楁椂銆?渚濊禆 2銆?
4. 鍦ㄤ簩杩涘埗鍛戒护灞傛帴鍏ユ棩蹇楋細浼樺厛鍦?Z4671Command 鐨?CmdSendExchange 鍜?CmdReadExchange 鍋氱粺涓€鏀跺彛锛岃褰曠紪鐮佸墠 payload銆佺紪鐮佸悗鍙戦€佸抚銆佽В鐮佸悗鍝嶅簲甯э紝浠ュ強 m_strLogInfo 涓殑鍗忚閿欒锛汼tartFWUpdate銆丗WTranSportFW銆丗WUpdateEnd銆丟etLogFileData 鍙ˉ鍏呰涔夊寲鎽樿鍜岃繘搴︼紝涓嶉噸澶嶆墦鍗板簳灞傚抚銆傝繖鏍疯兘瑕嗙洊褰撳墠鍜屾湭鏉ュぇ閮ㄥ垎 Z4671 鎸囦护銆?渚濊禆 2銆?
5. 鍦ㄤ紶杈撴祦绋嬪眰琛ュ厖涓氬姟涓婁笅鏂囷細鍦?McsFwUploadBinEx 鍜?McsReadLutBundleFromDevice 涓鍔?chunk 杩涘害銆乫lash 鍦板潃銆佽姹傞暱搴︺€佺煭鍖?婧㈠嚭绛夋棩蹇楋紝褰㈡垚鈥滃懡浠ゅ抚 + 涓氬姟璇箟鈥濅袱灞備俊鎭紝渚夸簬瀹氫綅鏄崗璁棶棰樿繕鏄祦绋嬮棶棰樸€?渚濊禆 4锛屽彲涓?6 骞惰銆?
6. 鍦?UI 灞傛帴绾垮苟澧炲己灞曠ず锛氭墿灞?CM576CalibratorDlg::AppendLog 鎴栨柊澧炴ˉ鎺ユ柟娉曪紝缁熶竴鎺ユ敹 Logger 杈撳嚭锛涗繚鐣欑幇鏈夌紪杈戞灞曠ず锛屼絾澧炲姞鏃堕棿鎴冲墠缂€銆佹渶澶х紦瀛橀檺鍒跺拰鑷姩婊氬姩銆侽penPort銆丷unPathPowerMeter銆丷unPathPd銆丱nBnClickedFlash銆丱nBnClickedReadFlashBackup 杩欎簺鍏ュ彛鍙緭鍑烘祦绋嬭妭鐐癸紝涓嶅啀鎵挎媴搴曞眰鍗忚鏄庣粏銆?渚濊禆 2锛屽彲涓?5 骞惰銆?
7. 澧炲姞鏂囦欢钀界洏锛氬湪绋嬪簭鍚姩鎴栫涓€娆￠€氫俊鍓嶅垱寤?output/comm.log锛岄噰鐢ㄨ拷鍔犲啓鍏ワ紱姣忔潯鏃ュ織涓€琛岋紝浜岃繘鍒舵暟鎹寜 hex 杈撳嚭锛孉SCII 鍘熸枃淇濈暀鍙杞箟瀛楃銆傝淇濊瘉鏂囦欢鍐欏叆澶辫触涓嶄細涓柇涓绘祦绋嬶紝鍙檷绾т负 UI 鏃ュ織銆?渚濊禆 2锛屽缓璁湪 6 瀹屾垚鍚庢帴鍏ャ€?
8. 鎺у埗鏃ュ織浣撻噺涓庣ǔ瀹氭€э細瀵硅秴闀?sweep 鍝嶅簲鍜屽ぇ鍧楀浐浠跺垎鐗囪缃崟琛屾埅鏂瓥鐣ユ垨鎽樿琛ュ厖锛屼緥濡備繚鐣欏墠 N 瀛楄妭/瀛楃骞堕檮鎬婚暱搴︼紱瀵?UI 缂栬緫妗嗚缃渶澶у瓧绗︽暟鎴栨渶澶ц鏁帮紝瓒呰繃闃堝€煎悗瑁佸壀鏃у唴瀹癸紱閬垮厤姣忎釜瀛楄妭绾т簨浠堕兘鐩存帴鍒锋柊 UI 瀵艰嚧鍗￠】銆?渚濊禆 3/4/6/7銆?
9. 缁熶竴閿欒涓庤秴鏃惰緭鍑猴細鎶?ReadAsciiResponse 瓒呮椂銆丆mdReadExchange 瑙ｇ爜澶辫触銆佽繑鍥炵爜闈炴垚鍔熴€佺煭鍖呫€佹枃浠跺啓澶辫触閮借鑼冩垚 ERROR 绾ф棩蹇楋紝骞跺寘鍚浉鍏冲懡浠ゅ簭鍙枫€佸懡浠ゅ悕銆佽€楁椂鍜屽師濮嬪搷搴旂墖娈碉紝纭繚璇婃柇鏃舵棤闇€浜屾鐚滄祴銆?渚濊禆 3/4/7銆?
10. 鍋氱鍒扮楠岃瘉锛氬垎鍒獙璇?PM 璺緞銆丳D 璺緞銆丷ead Flash backup 鍜?Flash 鍥涙潯涓绘祦绋嬶紝纭 UI 涓?comm.log 閮借兘鐪嬪埌瀹屾暣璇锋眰/鍝嶅簲椤哄簭锛屽苟楠岃瘉澶辫触鍦烘櫙涓嬬殑鏃ュ織瀹屾暣鎬с€?渚濊禆鍓嶉潰鍏ㄩ儴姝ラ銆?

**Relevant files**
- `c:\Users\menghl2\WorkSpace\Projects\UDL\M576澧炲姞1310娉㈤暱鑷牎鍑嗚蒋浠跺紑鍙慭M576Calibrator\M576CalibratorApp\RecalSession.h` 鈥?鎵╁睍鏃ュ織鎺ュ彛鎴?logger 缁戝畾鐐癸紝淇濇寔 ASCII RECAL 浼氳瘽鑱岃矗闆嗕腑銆?
- `c:\Users\menghl2\WorkSpace\Projects\UDL\M576澧炲姞1310娉㈤暱鑷牎鍑嗚蒋浠跺紑鍙慭M576Calibrator\M576CalibratorApp\RecalSession.cpp` 鈥?鍦?SendRecal0/1/2/3/5銆丷eadAsciiResponse銆丷eadLineBlocking 璁板綍璇锋眰銆佸搷搴斻€佽€楁椂銆佽秴鏃躲€?
- `c:\Users\menghl2\WorkSpace\Projects\UDL\M576澧炲姞1310娉㈤暱鑷牎鍑嗚蒋浠跺紑鍙慭M576Calibrator\Z4671Core\Z4671Command.h` 鈥?澧炲姞 logger 鎴愬憳鎴栧洖璋冩敞鍏ョ偣锛屼繚璇佸懡浠ゅ眰鍙粺涓€杈撳嚭甯ф棩蹇椼€?
- `c:\Users\menghl2\WorkSpace\Projects\UDL\M576澧炲姞1310娉㈤暱鑷牎鍑嗚蒋浠跺紑鍙慭M576Calibrator\Z4671Core\Z4671Command.cpp` 鈥?鍦?CmdSendExchange銆丆mdReadExchange銆丼tartFWUpdate銆丗WTranSportFW銆丗WUpdateEnd銆丟etLogFileData 璁板綍浜岃繘鍒跺抚鍜屽崗璁姸鎬併€?
- `c:\Users\menghl2\WorkSpace\Projects\UDL\M576澧炲姞1310娉㈤暱鑷牎鍑嗚蒋浠跺紑鍙慭M576Calibrator\M576CalibratorApp\McsFwTransport.h` 鈥?濡傞渶鎶婃棩蹇楀洖璋冩部娴佺▼灞傞€忎紶锛屽彲鍦ㄨ繖閲屾墿灞曞嚱鏁扮鍚嶆垨澹版槑鍏变韩 logger銆?
- `c:\Users\menghl2\WorkSpace\Projects\UDL\M576澧炲姞1310娉㈤暱鑷牎鍑嗚蒋浠跺紑鍙慭M576Calibrator\M576CalibratorApp\McsFwTransport.cpp` 鈥?璁板綍 chunk 杩涘害銆乫lash 鍦板潃/闀垮害銆佷紶杈撹捣姝㈣妭鐐广€侀敊璇笂涓嬫枃銆?
- `c:\Users\menghl2\WorkSpace\Projects\UDL\M576澧炲姞1310娉㈤暱鑷牎鍑嗚蒋浠跺紑鍙慭M576Calibrator\M576CalibratorApp\M576CalibratorDlg.cpp` 鈥?UI 灞曠ず銆佹棩蹇楁ˉ鎺ャ€佸叆鍙ｆ祦绋嬭妭鐐硅緭鍑恒€佺紪杈戞瑁佸壀鍜屾枃浠惰矾寰勫垵濮嬪寲銆?
- `c:\Users\menghl2\WorkSpace\Projects\UDL\M576澧炲姞1310娉㈤暱鑷牎鍑嗚蒋浠跺紑鍙慭M576Calibrator\Z4671Core\OpComm.h` 鈥?浠呬綔涓哄簳灞備覆鍙ｈ兘鍔涘弬鑰冿紝涓嶅缓璁洿鎺ュ湪 ReadBuffer/WriteBuffer 鍋氬叏灞€鏃ュ織锛岄伩鍏嶆薄鏌撴墍鏈変覆鍙ｈ皟鐢ㄣ€?
- `c:\Users\menghl2\WorkSpace\Projects\UDL\M576澧炲姞1310娉㈤暱鑷牎鍑嗚蒋浠跺紑鍙慭M576Calibrator\Z4671Core\OpComm.cpp` 鈥?濡傚懡浠ゅ眰鏃犳硶瀹屾暣鏀跺彛锛屽啀浣滀负鍏滃簳璇勪及鐐癸紱榛樿涓嶆敼銆?

**Verification**
1. 鎵撳紑 429F 涓插彛鍚庢墽琛?PM 妯″紡 Run Path锛屾鏌?UI 鍜?output/comm.log 鏄惁鍑虹幇瀹屾暣鐨?RECAL 0銆丷ECAL 1銆丷ECAL 3(0/1) 璇锋眰涓庡搷搴斿簭鍒楋紝浠ュ強姣忔鑰楁椂銆?
2. 鎵ц PD 妯″紡 Run Path锛岀‘璁?RECAL 2銆丷ECAL 5(0/1) 涔熸寜鍚屾牱鏍煎紡杈撳嚭锛屼笖涓?PM 妯″紡搴忓彿杩炵画鎴栨寜浼氳瘽绛栫暐鏄庣‘閲嶇疆銆?
3. 鎵ц Read Flash backup锛岀‘璁?GetLogFileData 鐨勫湴鍧€銆侀暱搴︺€佸搷搴旈暱搴︺€乧hunk 杩涘害鍜屽け璐ヤ俊鎭畬鏁磋褰曘€?
4. 鎵ц Flash锛岀‘璁?StartFWUpdate銆丗WTranSportFW 鍒嗙墖銆丗WUpdateEnd 鍜屽畬鎴愮瓑寰呴樁娈靛潎鏈夋棩蹇楋紝鑷冲皯鍙畾浣嶅け璐ュ彂鐢熷湪鍝竴鐗囥€?
5. 浜哄伐鍒堕€犲紓甯革細鏂紑涓插彛鎴栬緭鍏ラ敊璇?COM 鍙ｏ紝楠岃瘉瓒呮椂銆佹墦寮€澶辫触銆佺┖鍝嶅簲銆佽繑鍥炵爜寮傚父閮借兘鍦?UI 鍜屾枃浠朵腑鐪嬪埌 ERROR 鏃ュ織銆?
6. 闀挎椂闂磋窇鏁板悗妫€鏌?UI 缂栬緫妗嗘槸鍚︿粛鍙搷搴旓紝纭鏃ュ織瑁佸壀绛栫暐鐢熸晥涓?comm.log 鏈奖鍝嶄富娴佺▼銆?

**Decisions**
- 閲囩敤 `UI + 鏂囦欢` 鍙岃緭鍑猴紝鏂囦欢榛樿璺緞寤鸿涓虹▼搴忕洰褰曚笅鐨?`output\comm.log`銆?
- 榛樿璁板綍 `瀹屾暣鎶ユ枃`锛欰SCII 淇濈暀鍘熸枃锛屼簩杩涘埗杈撳嚭 hex dump锛屽苟闄勮涔夋憳瑕佸拰鑰楁椂銆?
- 鎺ㄨ崘鎶婃棩蹇椾富鏀跺彛鏀惧湪 `CRecalSession` 鍜?`Z4671Command`锛岃€屼笉鏄洿鎺ヤ笅娌夊埌 `COpComm`锛屽洜涓哄綋鍓嶉渶瑕佺殑鏄€滃崗璁氦浜掓棩蹇椻€濊€屼笉鏄墍鏈変覆鍙ｅ瓧鑺傛祦銆?
- 淇濇寔鏈€灏忎镜鍏ュ師鍒欙細UI 灞備繚鐣欑幇鏈?`AppendLog` 浣滀负灞曠ず绔紝鏂板 logger 鍙礋璐ｆ爣鍑嗗寲杈撳嚭锛屼笉閲嶅啓鐜版湁鏍″噯娴佺▼銆?
- 鏈鑼冨洿涓嶅寘鍚柊鐨勭嫭绔嬫棩蹇楁煡鐪嬪櫒銆佷笉寮曞叆绗笁鏂规棩蹇楀簱銆佷笉鍋氬绾跨▼閲嶆瀯锛涘鏋滃悗缁?UI 鍗￠】鏄庢樉锛屽啀鍗曠嫭璇勪及寮傛钀界洏鍜屾秷鎭槦鍒椼€?

**Further Considerations**
1. 浜岃繘鍒舵棩蹇楀缓璁悓鏃惰緭鍑衡€滅紪鐮佸墠 payload鈥濆拰鈥滅紪鐮佸悗鍙戦€佸抚鈥濓紱鍓嶈€呭埄浜庣悊瑙ｅ懡浠よ涔夛紝鍚庤€呭埄浜庢帓鏌ヨ浆涔変笌甯ц竟鐣岄棶棰樸€?
2. 瀵?sweep 杩斿洖鐨勪竴闀夸覆娴偣鏁版嵁锛屽缓璁?UI 涓樉绀烘憳瑕佺増骞跺湪鏂囦欢涓繚鐣欏畬鏁村唴瀹癸紝閬垮厤缂栬緫妗嗘€ц兘閫€鍖栥€