#include "stdafx.h"
#include "perebor.h"

namespace raskroy {
	// ����������� �������
	// ���������:
	//	[i] size - ������ �� ������� ����� ����������� ������
	//	[o] rashod - ���������� ������������� �������
	//	[io] i
	//	[i] end
	//	[i] remains
	scalar perebor::recursive(scalar size, t_amounts &rashod)
	{
		scalar register size1;
		unsigned n;
		if (i != end)
		{
			n = 0;
			scalar best = size;
			bool first = true;
			size1 = size;
			t_amounts rashod1;
			unsigned remain = remains[i->offset];
			while (n <= remain && size1 > saw_thickness)
			{
				scalar remain;
				i++;
				remain = recursive(size1, rashod1);
				i--;
				if (remain < best || first)
				{
					best = remain;
					rashod = rashod1;
					rashod[i->offset] = n;
					if (best <= 0)
						return best;
					first = false;
				}
				n++;
				size1 = size - (i->size + saw_thickness)*n;
				// size1 ����� ����� ���� < 0 ��� ��������� ���� ��������� ��� ������ ���
				// ������
			}
			// ���� ������ ������ ����������� �� ���������� ����
			//if (best == size)
			//{
			//	rashod.resize(remains.size());
			//	std::fill(rashod.begin(), rashod.end(), 0);
			//}
			return best;
		}
		else
		{
			n = unsigned((size + saw_thickness)/(i->size + saw_thickness));
			unsigned remain = remains[i->offset];
			if (n > remain)
				n = remain;
			rashod.resize(remains.size());
			std::fill(rashod.begin(), rashod.end(), 0);
			rashod[i->offset] = n;
			size1 = size - n*(i->size + saw_thickness);
			// size1 ����� ����� ���� < 0 ��� ��������� ���� ��������� ��� ������ ���
			// ������
			return size1;
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
	bool perebor::make(const t_size &size, scalar other_size, t_raskroy::t_details &details, t_amounts &rashod)
	{
		if (other_size < size.other_sizes.min->size)
			return false;

		// ��������� ���������� ��� ��������
		i = size.other_sizes.begin();
		end = size.other_sizes.end(); end--;
		// ����������� ������ ��� �������� [i..end]
		remain = recursive(other_size, rashod);
		if (remain == other_size)	// ���� ������ ������ �����������
			return false;

		unsigned nums = 0;
		for (i = size.other_sizes.begin(); i != size.other_sizes.end(); i++)
		{
			unsigned rashodi = rashod[i->offset];
			if (rashodi > 0)
			{
				t_raskroy::t_detail detail;
				detail.size = i->size;
				detail.num = rashodi;
				details.push_back(detail);
				nums += rashodi;	// ���������� �����
			}
		}
		// ��������� ���������� ������
		opilki = size.size*saw_thickness*nums;
		// ���� ��������� ��� �������� �� ������ �� ��������� ���������� ������
		if (remain < 0)
		{
			opilki += size.size*remain;
			remain = 0;
		}
		return true;
	}
}