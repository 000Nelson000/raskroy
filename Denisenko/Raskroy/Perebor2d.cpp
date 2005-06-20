#include "stdafx.h"
#include "Perebor2d.h"

namespace Denisenko {
namespace Raskroy {

const scalar MIN_USEFUL_SIZE1 = 70.0;
const scalar MIN_USEFUL_SIZE2 = 200-34;

// ������� ����� �� ����� � �� ������, ���������� ������ �������
// ���������:
//		[i] rect - ������ �����
//		[i/o] stat - ����������
//		[o] raskroy - ������� �����
//		[o] rashod - ������ �������
//
inline bool Perebor2d::Optimize(const Rect &rect, Stat &stat, int s, t_raskroy &raskroy, Amounts &rashod)
{
	// ������� ����������� �� ������� s
	Stat stat2(stat);
	if (Recursion(m_sizes[s].begin(), rect, stat, s, raskroy, rashod))
	{
		// ���� ������, �� ������� ����������� �� ������� !s
		Amounts rashod2;
		t_raskroy raskroy2;
		if (Recursion(m_sizes[!s].begin(), rect, stat2, !s, raskroy2, rashod2)
			&& /*pcriteria->quality(*/stat/*)*/ < /*pcriteria->quality(*/stat2/*)*/)
		{
			// ���� ������ � ����� ��� �� s, �� ��������� ����� ��� �� !s
			stat = stat2;
			raskroy = raskroy2;
			rashod = rashod2;
		}
		// ����� ��������� ��� �� s
		return true;
	}
	// ����� ��� ���������� ��� �� s
	return false;
}

// ����������� ������� ���� ������� ����� �� �����/������ (s=0/1) � ������������ �������� ������������
// ���������:
//		[i] list - ������� �����
//		[i/o] stat - ����������
//		[i] s - ����� ����������� ������� �� �����/������ s=0/1
//		[o] raskroy - ������� �����
//		[o] rashod - ������ �������
//
bool Perebor2d::Recursion(Sizes::iterator begin, const Rect &rect, Stat &stat, int s, t_raskroy &raskroy, Amounts &rashod)
{
	if (begin == m_sizes[s].end())
		// ����� ����� ��������� ������������
		return Recursion(m_sizes[!s].begin(), rect, stat, !s, raskroy, rashod);

	bool first = true;
	Stat bestStat;	// ������ ���������� ������ �����, ��� ������ ������������ � ������� ����������

	// ���������� ����������� ����� ����� �������� ������ �������������
	Amounts rashodPerebor, vrashod, rashod1;
	t_raskroy remainRaskroy;	
	t_raskroy recurseRaskroy;
	for (Sizes::iterator i = begin; i != m_sizes[s].end(); i++)
	{
		// ���� ������ ������� �������, �� ��������� ����,
		// �.�. ��������� ������� ����� ��� ������
		if (i->Value > rect.Size[s])
			break;

		t_raskroy::t_details details;
		scalar opilki;
		scalar remain;
		if (!m_perebor.Make(*i, rect.Size[!s], details, rashodPerebor, opilki, remain))
			continue;

		//stat1.sum_cut_length += rect.size[!s];
		// ��������� ������
		scalar opilki1 = opilki + (rect.Size[!s] - remain) * m_perebor.get_SawThickness();
		scalar opilki2 = remain * m_perebor.get_SawThickness();
		// ��������� ���������� �������
		Rect remainRect;
		remainRect.Size[s] = i->Value;
		remainRect.Size[!s] = remain;
		// ��������� ������� ��� ��������
		Rect recurseRect(rect);
		// ��������, �� ������� ����� �������� ������� ��������
		scalar reduce = i->Value + m_perebor.get_SawThickness();

		// ������� ���������
		int maxKratnostj = int((rect.Size[s] + m_perebor.get_SawThickness()) / (i->Value + m_perebor.get_SawThickness()));
		if (maxKratnostj > 1)
		{
			int kolKrat = m_remains / rashodPerebor;
			if (maxKratnostj > kolKrat)
				maxKratnostj = kolKrat;
		}

		Stat stat1(0);
		/*stat1.useful_remain = 0;
		stat1.unuseful_remain = 0;
		stat1.useful_num = 0;*/
		rashod1 = rashodPerebor;
		for (int kratnostj = 1; kratnostj <= maxKratnostj; kratnostj++)
		{
			// ������������� �������, ����������, ��������� � ����������� �� ���������
			if (kratnostj > 1)
			{
				rashod1 = rashodPerebor * kratnostj;
				remainRect.Size[s] += i->Value;
			}
			recurseRect.Size[s] -= reduce;
			stat1.Opilki = opilki1 * kratnostj + opilki2;
			if (recurseRect.Size[s] < 0)
			{
				stat1.Opilki += rect.Size[!s] * recurseRect.Size[s];
				recurseRect.Size[s] = 0;
			}

			// ����� ���������� �������
			// ������������ ������� � ����������� �� ������������� �������
			m_remains -= rashod1;	// ����� ����� ����� ������������ �������
			bool haveRemain;
			if (haveRemain = Optimize(remainRect, stat1, !s, remainRaskroy, vrashod))
			//if (Recursion(sizes[!s].begin(), rect1, stat1, !s, remain_raskroy, rashod))
			{
				// ���� ���� ����, �� ������������� ������������ ������� � ������
				rashod1 += vrashod;
				m_remains -= vrashod;
			}

			//if (!first && pcriteria->compare(&best_stat, &stat1))	// already bad
			//	continue;

			// �������� ��������
			bool haveRecurse = Recursion(i + 1, recurseRect, stat1, s, recurseRaskroy, vrashod);
			m_remains += rashod1;	// ��������������� �������

			// ���� ��������� ����� ������� ��� ������ ��
			if (bestStat < stat1 || first)
			{
				bestStat = stat1;
				raskroy.set(s,
					kratnostj,
					i->Value, details,
					haveRemain ? &remainRaskroy : 0,
					haveRecurse ? &recurseRaskroy : 0);

				// rashod1 - ������ � ��������+� ���������� �����
				// vrashod - ������ � ����������� �����
				rashod = rashod1;
				if (haveRecurse)
					rashod += vrashod;
				first = false;
			}
		}
	}
	// ���� ��� ���������
	if (!first)
	{
		// ��������� ���� ������ ���������� � ������� ����������
		stat += bestStat;
		// ������� � ������ ��� �������� � ������ �����
		return true;
	}

	// ���� �� ���� ����������, �� � ���������� ������������ ������
	scalar rem = rect.Square();
	if (rem == 0)
		return false;
	bool useful = (rect.Size[0] >= MIN_USEFUL_SIZE1 && rect.Size[1] >= MIN_USEFUL_SIZE2)
		|| (rect.Size[1] >= MIN_USEFUL_SIZE1 && rect.Size[0] >= MIN_USEFUL_SIZE2);
	assert(rem > 0);
	useful ? stat.UsefulRemain += rem, stat.UsefulNum++ : stat.UnusefulRemain += rem, stat.UsefulNum++;
	return false;
}

} // namespace Denisenko
} // namespace Raskroy