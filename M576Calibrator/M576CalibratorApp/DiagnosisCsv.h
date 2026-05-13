#pragma once

#include <vector>
#include <afxstr.h>

/// One input *group* loaded from output/diagnosis_sw.csv.
///
/// CSV semantics (per group, one CSV data line):
///   - **Legacy:** `# CH343` comment then a line of SW only: `SW 1 1 20|SW 1 2 52|…`
///     — `label` holds the comment text; `channel` is empty (Python CSV uses `label` as Channel).
///   - **Inline channel:** first `|` token is not an SW command: `CH343|SW 1 1 20|…`
///     — `channel` is that token; remaining tokens are SW commands (channel is **not** sent on serial).
///   - Trailing CR is added automatically by the host before each SW line.
/// `label` is the optional preceding `#` / `;` comment (reporting / fallback channel).
struct M576DiagnosisRow
{
	CStringA label;
	CStringA channel;
	std::vector<CStringA> swCommands;
};

/// Fixed post-path (three blocks after the CSV SW group):
///   s1 (SFP_1550): `WL 1550`, `SW 3 1 1`, `pd 1`, `opm 3 1` — `wl1310Reply` unused.
///   s2 (SFP_1310): `WL 1310`, `SW 3 1 4`, `pd 1`, `opm 3 1` — `wl1550Reply` unused.
///   s3 (Laser_1310): `WL 1310`, `SW 3 1 8`, `pd 1`, `opm 3 1` — `wl1550Reply` unused.
/// `sw3Third` is the third token of `SW 3 1 <n>` (1, 4, or 8). Result CSV columns
/// keep the same names; empty fields mean that wavelength line was not sent for that scenario.
struct M576DiagnosisWlScenarioResult
{
	int sw3Third;
	CStringA wl1550Reply;
	CStringA sw3Reply;
	CStringA wl1310Reply;
	CStringA pdReply;
	CStringA opmReply;

	M576DiagnosisWlScenarioResult()
		: sw3Third(0)
	{
	}
};

/// One output row in output/diagnosis_<timestamp>.csv. Replies are joined back
/// with `|` so the row reproduces the input grouping verbatim. `swOkCount` /
/// `swCount` give a quick FAIL summary without needing to re-parse the joined
/// reply column. `totalMs` is the sum of every sub-exchange (all CSV SW +
/// three measure blocks: WL + SW + pd + opm each, with per-scenario WL as above).
struct M576DiagnosisResultRow
{
	int step;
	CStringA label;
	CStringA swCmds;
	CStringA swReplies;
	int swCount;
	int swOkCount;
	M576DiagnosisWlScenarioResult wlScen[3];
	DWORD totalMs;

	M576DiagnosisResultRow()
		: step(0)
		, swCount(0)
		, swOkCount(0)
		, totalMs(0)
	{
	}
};

/// Parse output/diagnosis_sw.csv into groups.
/// - Lines starting with `#` or `;` are comments. The trimmed text after the
///   marker becomes the *next* data line's `label` (subsequent comment lines
///   overwrite earlier ones; empty lines do not clear the pending label).
/// - Each non-comment line is split by `|`; empty tokens are skipped.
///   If the first token starts with `SW` (case-insensitive), all tokens are SW commands
///   and `channel` is left empty (use `label` for Python Channel column). Otherwise the
///   first token is `channel` and the rest are SW commands.
/// - UTF-8 BOM on the first line is tolerated.
/// Returns FALSE only on hard I/O errors or when no groups were parsed.
BOOL M576LoadDiagnosisSwCsv(LPCTSTR path, std::vector<M576DiagnosisRow>& rows, CString& err);

/// Fixed unified log path: `{outBaseDir}\diagnosis_log.csv` (append-only PD/OPM rows).
CString M576GetDiagnosisUnifiedLogCsvPath(LPCTSTR outBaseDir);

/// Append one Python-format row (Channel + s1..s3 pd/opm replies). Opens in binary append
/// mode; writes UTF-8 header if file is empty. Calls fflush after each row.
BOOL M576AppendDiagnosisPythonRow(LPCTSTR path, const CStringA& channel, const M576DiagnosisWlScenarioResult wlScen[3], CString& err);

/// Compose `output/diagnosis_<yyyymmdd_HHMMSS>.csv` next to the exe under
/// the given output dir. The returned path is absolute and includes the
/// timestamp; `outBaseDir` should already point at the output folder.
CString M576MakeDiagnosisResultCsvPath(LPCTSTR outBaseDir);

/// Write the result CSV (RFC4180 quoting for fields with comma/quote/CR/LF).
/// Per scenario s1..s3: columns wl1550, sw3, wl1310, pd, opm (unused WL fields empty;
/// for s2/s3 the WL 1310 command is sent before SW 3 1 on the wire).
BOOL M576WriteDiagnosisResultCsv(LPCTSTR path, const std::vector<M576DiagnosisResultRow>& rows, CString& err);
