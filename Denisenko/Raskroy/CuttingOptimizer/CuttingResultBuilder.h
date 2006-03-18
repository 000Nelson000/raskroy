#pragma once

#include "raskroy.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace Denisenko::Cutting;

namespace Denisenko {
namespace Cutting {

Decimal FromScaled(Int64 scaled)
{
	return Decimal(scaled) / 1000;
}

Int64 ToScaled(Decimal value)
{
	return Decimal::ToInt64(value * 1000);
}

public ref class CuttingResultBuilder
{
public:
	CuttingResultBuilder()
	{
	}

	void LoadSections(const Denisenko::Raskroy::t_raskroy& input, ParametersCollection^ parameters, CuttingScheme^ output)
	{
		m_result = output;
		m_parameters = parameters;
		CutType cutType = input.s == 0 ? CutType::Vertical : CutType::Horizontal;
		Recursive(input, cutType, output->RootSection);
	}

private:
	CuttingScheme^ m_result;
	ParametersCollection^ m_parameters;

	void Recursive(const Denisenko::Raskroy::t_raskroy& input, CutType cutType, Section^ output)
	{
		m_result->MakeSureEdgeEven(output, cutType);

		Decimal lineWidth = (FromScaled(input.cut) + m_parameters->CutterThickness) * input.kratnostj - m_parameters->CutterThickness;
		Section^ rightLine;
		Section^ leftLine = m_result->Cut(output, lineWidth, cutType, rightLine);
		Section^ bottomLine;
		m_result->MakeSureEdgeEven(leftLine, CuttingScheme::Rotate(cutType));
		if(input.kratnostj > 1)
		{
			Section^ detailsLine = m_result->Cut(leftLine, GetDetailsLength(input.details), CuttingScheme::Rotate(cutType), bottomLine);
			for(Int32 i = input.kratnostj; i > 0; i--) {
				Section^ detailsRemain;
				detailsLine = m_result->Cut(detailsLine, FromScaled(input.cut), cutType, detailsRemain);
				AddDetails(input.details, detailsLine, CuttingScheme::Rotate(cutType));
				detailsLine = detailsRemain;
			}
		}
		else
		{
			bottomLine = AddDetails(input.details, leftLine, CuttingScheme::Rotate(cutType));
		}

		// ���������� ������ �����, ���� ����
		if(input.watchRemain() != NULL)
		{
			CutType newCutType = input.watchRemain()->s != input.s ? CuttingScheme::Rotate(cutType) : cutType;
			Recursive(*input.watchRemain(), newCutType, bottomLine);
		}

		// ���������� ������ �����, ���� ����
		if(input.watchRecurse() != NULL)
		{
			CutType newCutType = input.watchRecurse()->s != input.s ? CuttingScheme::Rotate(cutType) : cutType;
			Recursive(*input.watchRecurse(), newCutType, rightLine);
		}
	}

	Decimal GetDetailsLength(const Raskroy::t_raskroy::t_details& details)
	{
		Decimal result;
		for(UInt32 i = 0; i < details.size(); i++)
		{
			result += FromScaled(details[i].size + ToScaled(m_parameters->CutterThickness)) * details[i].num;
		}
		result -= m_parameters->CutterThickness;
		return result;
	}

	Section^ AddDetails(const Raskroy::t_raskroy::t_details& details, Section^ output, CutType cutType)
	{
		Section^ result = output;
		for(UInt32 i = 0; i < details.size(); i++)
		{
			for(Int32 j = details[i].num; j > 0; j--)
			{
				Section^ partSection = m_result->Cut(result, FromScaled(details[i].size), cutType, result);
				m_result->MarkAsPart(partSection);
			}
		}
		return result;
	}
};

}
}