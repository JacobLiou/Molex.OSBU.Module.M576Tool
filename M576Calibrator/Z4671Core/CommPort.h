#pragma once

#include "OpComm.h"

/// Thin wrapper for optional future mocking; calibration code uses COpComm directly.
class CCommPortRef
{
public:
	explicit CCommPortRef(COpComm& comm) : m_p(&comm) {}
	COpComm& Comm() { return *m_p; }
	const COpComm& Comm() const { return *m_p; }
private:
	COpComm* m_p;
};
