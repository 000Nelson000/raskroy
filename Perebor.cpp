#include "stdafx.h"
#include "Perebor.h"

namespace Denisenko {
namespace Raskroy {

// ����������� �������
// ���������:
//	[i] size - ������ �� ������� ����� ����������� ������
//	[o] rashod - ���������� ������������� �������
// this:
//	[io] m_pOtherSize
//	[i]  m_pEndOtherSize
//	[i]  m_remains
scalar Perebor::Recursion(scalar i_size, Amounts &o_rashods)
{
	if (m_pOtherSize != m_pEndOtherSize)
	{
		scalar bestRemain = i_size;
		bool first = true;
		scalar size = i_size;
		unsigned n = 0;
		unsigned amount = (*m_remains)[m_pOtherSize->Offset];

		while (n <= amount && size > m_sawThickness)
		{
			scalar remain;
			Amounts rashods(m_remains->size());
			m_pOtherSize++;
			remain = Recursion(size, rashods);
			m_pOtherSize--;
			if (remain < bestRemain || first)
			{
				bestRemain = remain;
				rashods[m_pOtherSize->Offset] = n;
				o_rashods = rashods;
				if (bestRemain <= 0) // ����� ���� �� �����
					return bestRemain;
				first = false;
			}
			size -= m_pOtherSize->Value + m_sawThickness;
			n++;
			// size ����� ����� ���� < 0 ��� ��������� ���� ��������� ��� ������ ���
			// ������
		}
		// ���� ������ ������ ����������� �� ���������� ����
		//if (best == size)
		//{
		//	rashod.resize(remains.size());
		//	std::fill(rashod.begin(), rashod.end(), 0);
		//}
		return bestRemain;
	}
	else
	{
		scalar fullSize = m_pOtherSize->Value + m_sawThickness;
		unsigned n = unsigned((i_size + m_sawThickness) / fullSize);
		unsigned amount = (*m_remains)[m_pOtherSize->Offset];
		if (n > amount)
			n = amount;
		//o_rashods.resize(m_remains->size());
		std::fill(o_rashods.begin(), o_rashods.end(), 0);
		o_rashods[m_pOtherSize->Offset] = n;
		// ��������� ����� ����� ���� < 0 ��� ��������� ���� ��������� ��� ������ ���
		// ������
		return i_size - n * fullSize;
	}
}

// ������� ������� ������ ��������� ������� �� ������ ������� � ��������� ��������
// ���������:
//		[i] size - �������� ������
//		[i] other_size - ���������������� ������
//		[o] stat - ����������
//		[o] details - ������������ �������, �� ���� �������� ������ ���������
//		[o] rashod - ������ �������
// ���������� true ���� ���� �� ���� ������ �����������
bool Perebor::Make(const Size &size, scalar otherSize, t_raskroy::t_details &o_details, Amounts &o_rashods, scalar &o_remain, double &o_opilki)
{
	if (otherSize < size.other_sizes.Min->Value)
		return false;

	// ��������� ���������� ��� ��������
	m_pOtherSize = size.other_sizes.begin();
	m_pEndOtherSize = size.other_sizes.end();
	m_pEndOtherSize--;
	// ����������� ������ ��� �������� [i..end]
	scalar remain = Recursion(otherSize, o_rashods);
	if (remain == otherSize)	// ���� ������ ������ �����������
		return false;

	assert(!o_rashods.IsAllZeros());

	unsigned cuts = 0;// ���������� �����
	for (OtherSizes::const_iterator pOtherSize = size.other_sizes.begin(); pOtherSize != size.other_sizes.end(); pOtherSize++)
	{
		unsigned rashod = o_rashods[pOtherSize->Offset];
		if (rashod > 0)
		{
			t_raskroy::t_detail detail;
			detail.size = pOtherSize->Value;
            detail.other_size = &*pOtherSize;
			detail.num = rashod;
			o_details.push_back(detail);
			cuts += rashod;	// ���������� �����
		}
	}
	// ��������� ���������� ������
	double opilki = (double)size.Value * (double)m_sawThickness * (double)cuts;
	// ���� ��������� ��� �������� �� ������ �� ��������� ���������� ������
	if (remain < 0)
	{
		opilki += (double)size.Value * (double)remain;
		remain = 0;
	}
	o_opilki = opilki;
	o_remain = remain;
	return true;
}

} // namespace Denisenko
} // namespace Raskroy
