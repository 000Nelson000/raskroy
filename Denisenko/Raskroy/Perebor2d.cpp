#include "stdafx.h"
#include "Perebor2d.h"

namespace Denisenko {
namespace Raskroy {

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
	if (Recursion(_sizes[s].begin(), rect, stat, s, raskroy, rashod))
	{
		// ���� ������, �� ������� ����������� �� ������� !s
		t_amounts rashod2;
		t_raskroy raskroy2;
		if (Recursion(_sizes[!s].begin(), rect, stat2, !s, raskroy2, rashod2)
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
	if (begin == _sizes[s].end())
		// ����� ����� ��������� ������������
		return Recursion(_sizes[!s].begin(), rect, stat, !s, raskroy, rashod);

	bool first = true;
	t_stat best_stat;	// ������ ���������� ������ �����, ��� ������ ������������ � ������� ����������

	// ���������� ����������� ����� ����� �������� ������ �������������
	t_amounts rashod_perebor, vrashod, rashod1;
	t_raskroy remain_raskroy;	
	t_raskroy recurse_raskroy;
	for (t_sizes::iterator i = begin; i != _sizes[s].end(); i++)
	{
		// ���� ������ ������� �������, �� ��������� ����,
		// �.�. ��������� ������� ����� ��� ������
		if (i->size > rect.size[s])
			break;

		t_raskroy::t_details details;
		if (!_perebor.make(*i, rect.size[!s], details, rashod_perebor))
			continue;

		//stat1.sum_cut_length += rect.size[!s];
		// ��������� ������
		scalar opilki1 = _perebor.opilki + (rect.size[!s] - _perebor.remain)*_perebor.saw_thickness;
		scalar opilki2 = _perebor.remain * _perebor.saw_thickness;
		// ��������� ���������� �������
		t_rect remain_rect;
		remain_rect.size[s] = i->size;
		remain_rect.size[!s] = _perebor.remain;
		// ��������� ������� ��� ��������
		t_rect recurse_rect(rect);
		// ��������, �� ������� ����� �������� ������� ��������
		scalar reduce = i->size + _perebor.saw_thickness;

		// ������� ���������
		int max_kratnostj = int((rect.size[s] + _perebor.saw_thickness)/(i->size + _perebor.saw_thickness));
		if (max_kratnostj > 1)
		{
			int kol_krat = _remains / rashod_perebor;
			if (max_kratnostj > kol_krat)
				max_kratnostj = kol_krat;
		}

		t_stat stat1(0);
		/*stat1.useful_remain = 0;
		stat1.unuseful_remain = 0;
		stat1.useful_num = 0;*/
		rashod1 = rashod_perebor;
		for (int kratnostj = 1; kratnostj <= max_kratnostj; kratnostj++)
		{
			// ������������� �������, ����������, ��������� � ����������� �� ���������
			if (kratnostj > 1)
			{
				rashod1 = rashod_perebor*kratnostj;
				remain_rect.size[s] += i->size;
			}
			recurse_rect.size[s] -= reduce;
			stat1.opilki = opilki1*kratnostj + opilki2;
			if (recurse_rect.size[s] < 0)
			{
				stat1.opilki += rect.size[!s]*recurse_rect.size[s];
				recurse_rect.size[s] = 0;
			}

			// ����� ���������� �������
			// ������������ ������� � ����������� �� ������������� �������
			_remains -= rashod1;	// ����� ����� ����� ������������ �������
			bool have_remain;
			if (have_remain = Optimize(remain_rect, stat1, !s, remain_raskroy, vrashod))
			//if (Recursion(sizes[!s].begin(), rect1, stat1, !s, remain_raskroy, rashod))
			{
				// ���� ���� ����, �� ������������� ������������ ������� � ������
				rashod1 += vrashod;
				_remains -= vrashod;
			}

			//if (!first && pcriteria->compare(&best_stat, &stat1))	// already bad
			//	continue;

			// �������� ��������
			bool have_recurse = Recursion(i + 1, recurse_rect, stat1, s, recurse_raskroy, vrashod);
			_remains += rashod1;	// ��������������� �������

			// ���� ��������� ����� ������� ��� ������ ��
			if (stat1 > best_stat || first)
			{
				best_stat = stat1;
				raskroy.set(s,
					kratnostj,
					i->size, details,
					have_remain ? &remain_raskroy : 0,
					have_recurse ? &recurse_raskroy : 0);

				// rashod1 - ������ � ��������+� ���������� �����
				// vrashod - ������ � ����������� �����
				rashod = rashod1;
				if (have_recurse)
					rashod += vrashod;
				first = false;
			}
		}
	}
	// ���� ��� ���������
	if (!first)
	{
		// ��������� ���� ������ ���������� � ������� ����������
		stat += best_stat;
		// ������� � ������ ��� �������� � ������ �����
		return true;
	}

	// ���� �� ���� ����������, �� � ���������� ������������ ������
	scalar rem = rect.square();
	if (rem == 0)
		return false;
	bool useful = (rect.size[0] >= 70 && rect.size[1] >= 200-34)
		|| (rect.size[1] >= 70 && rect.size[0] >= 200-34);
	assert(rem > 0);
	useful ? stat.useful_remain += rem, stat.useful_num++ : stat.unuseful_remain += rem, stat.useful_num++;
	return false;
}

} // namespace Denisenko
} // namespace Raskroy