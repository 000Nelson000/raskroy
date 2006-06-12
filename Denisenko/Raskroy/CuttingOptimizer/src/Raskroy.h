#pragma once

#include "common.h"
#include "Perebor2d.h"

namespace Denisenko {
namespace Raskroy {

class Raskroy {
public:
	bool ControlRemains;
	Raskroy(void)
		: m_remains(0), m_perebor2d(m_sizes, m_minSize, &m_remains),
		ControlRemains(false)
	{
	}
	void   put_SawThickness(scalar x) throw () {m_perebor2d.put_SawThickness(x);}
	scalar get_SawThickness(void) throw () {return m_perebor2d.get_SawThickness();}
	void Begin(Parts &parts, const Parts &sheets);
	bool NextResult(t_result &res);

	__declspec(property(get = GetPercentCompleted)) float PercentCompleted;
	float GetPercentCompleted();

private:
	Amounts m_remains;
	Perebor2d m_perebor2d;
	Sizes m_sizes[2];
	scalar m_minSize[2];
	Parts m_sheets;
	Parts m_inputParts;
	Parts m_splitedParts;

	void RemoveExostedSizes(void);
	void CheckResult(const t_result& result);
};

Parts SplitEqualParts(const Parts& parts);
double GetEstimatedTime(const Parts& parts, const Sheet& sheet);
double GetAverageSizeA(const Parts& parts);
double GetAverageSizeB(const Parts& parts);
int Combinations(int smallSet, int fullSet);

} // namespace Denisenko
} // namespace Raskroy