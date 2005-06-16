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
inline bool Perebor2d::Optimize(const t_rect &rect, t_stat &stat, int s, t_raskroy &raskroy, t_amounts &rashod)
{
	// ������� ����������� �� ������� s
	t_stat stat2(stat);
	if (Recursion(m_sizes[s].begin(), rect, stat, s, raskroy, rashod))
	{
		// ���� ������, �� ������� ����������� �� ������� !s
		t_amounts rashod2;
		t_raskroy raskroy2;
		if (Recursion(m_sizes[!s].begin(), rect, stat2, !s, raskroy2, rashod2)
			&& /*pcriteria->quality(*/stat2/*)*/ > /*pcriteria->quality(*/stat/*)*/)
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
bool Perebor2d::Recursion(t_sizes::iterator begin, const t_rect &rect, t_stat &stat, int s, t_raskroy &raskroy, t_amounts &rashod)
{
	if (begin == m_sizes[s].end())
		// ����� ����� ��������� ������������
		return Recursion(m_sizes[!s].begin(), rect, stat, !s, raskroy, rashod);

	bool first = true;
	t_stat bestStat;	// ������ ���������� ������ �����, ��� ������ ������������ � ������� ����������

	// ���������� ����������� ����� ����� �������� ������ �������������
	t_amounts rashodPerebor, vrashod, rashod1;
	t_raskroy remainRaskroy;	
	t_raskroy recurseRaskroy;
	for (t_sizes::iterator i = begin; i != m_sizes[s].end(); i++)
	{
		// ���� ������ ������� �������, �� ��������� ����,
		// �.�. ��������� ������� ����� ��� ������
		if (i->size > rect.size[s])
			break;

		t_raskroy::t_details details;
		if (!m_perebor.make(*i, rect.size[!s], details, rashodPerebor))
			continue;

		//stat1.sum_cut_length += rect.size[!s];
		// ��������� ������
		scalar opilki1 = m_perebor.get_Opilki() + (rect.size[!s] - m_perebor.get_Remain()) * m_perebor.get_SawThickness();
		scalar opilki2 = m_perebor.get_Remain() * m_perebor.get_SawThickness();
		// ��������� ���������� �������
		t_rect remainRect;
		remainRect.size[s] = i->size;
		remainRect.size[!s] = m_perebor.get_Remain();
		// ��������� ������� ��� ��������
		t_rect recurseRect(rect);
		// ��������, �� ������� ����� �������� ������� ��������
		scalar reduce = i->size + m_perebor.get_SawThickness();

		// ������� ���������
		int maxKratnostj = int((rect.size[s] + m_perebor.get_SawThickness())/(i->size + m_perebor.get_SawThickness()));
		if (maxKratnostj > 1)
		{
			int kolKrat = m_remains / rashodPerebor;
			if (maxKratnostj > kolKrat)
				maxKratnostj = kolKrat;
		}

		t_stat stat1(0);
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
				remainRect.size[s] += i->size;
			}
			recurseRect.size[s] -= reduce;
			stat1.opilki = opilki1 * kratnostj + opilki2;
			if (recurseRect.size[s] < 0)
			{
				stat1.opilki += rect.size[!s] * recurseRect.size[s];
				recurseRect.size[s] = 0;
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
			if (stat1 > bestStat || first)
			{
				bestStat = stat1;
				raskroy.set(s,
					kratnostj,
					i->size, details,
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
	scalar rem = rect.square();
	if (rem == 0)
		return false;
	bool useful = (rect.size[0] >= MIN_USEFUL_SIZE1 && rect.size[1] >= MIN_USEFUL_SIZE2)
		|| (rect.size[1] >= MIN_USEFUL_SIZE1 && rect.size[0] >= MIN_USEFUL_SIZE2);
	assert(rem > 0);
	useful ? stat.useful_remain += rem, stat.useful_num++ : stat.unuseful_remain += rem, stat.useful_num++;
	return false;
}

} // namespace Denisenko
} // namespace Raskroy