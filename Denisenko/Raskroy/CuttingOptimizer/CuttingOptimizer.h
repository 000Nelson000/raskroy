// CuttingOptimizer.h

#pragma once

#include "raskroy.h"

using namespace std;
using namespace System;
using namespace Denisenko::Raskroy;

namespace Denisenko {
namespace Cutting {
namespace Optimizing {

	public ref class Optimizer
	{
	public:

		Optimizer()
		{
			m_started = false;
			m_parts = gcnew PartsCollection();
			m_sheets = gcnew SheetsCollection();
			m_parameters = gcnew ParametersCollection();
			m_raskroy = new Denisenko::Raskroy::Raskroy();
		}

		~Optimizer()
		{
			delete m_raskroy;
		}

		property PartsCollection^ Parts { PartsCollection^ get() { return m_parts; } }
		property SheetsCollection^ Sheets { SheetsCollection^ get() { return m_sheets; } }
		property ParametersCollection^ Parameters { ParametersCollection^ get() { return m_parameters; } }
		property CuttingScheme^ CurrentResult { CuttingScheme^ get() { return m_result; } }

		void Reset()
		{
			m_started = false;
		}

		Boolean NextResult() 
		{
			if(!m_started)
			{
				Denisenko::Raskroy::Parts convParts = ConvertParts();
				Denisenko::Raskroy::Parts convSheets = ConvertSheets();
				m_raskroy->put_SawThickness(Parameters->CutterThickness.Scaled);
				m_raskroy->Begin(convParts, convSheets);
				m_started = true;
			}
			Denisenko::Raskroy::t_result result;
			if(m_raskroy->NextResult(result))
			{
				m_result = ConvertResult(result);
				return true;
			}
			else
			{
				m_result = nullptr;
				return false;
			}
		}

	private:
		Denisenko::Raskroy::Raskroy* m_raskroy;
		PartsCollection^ m_parts;
		SheetsCollection^ m_sheets;
		Boolean m_started;
		ParametersCollection^ m_parameters;
		CuttingScheme^ m_result;

		Denisenko::Raskroy::Parts ConvertParts()
		{
			Denisenko::Raskroy::Parts result;
			for each(Part^ part in Parts)
			{
				result.push_back(Denisenko::Raskroy::Part(
					part->Length.Scaled,
					part->Width.Scaled,
					part->CanRotate,
					part->Quantity));
			}
			return result;
		}

		Denisenko::Raskroy::Parts ConvertSheets()
		{
			Denisenko::Raskroy::Parts result;
			for each(Sheet^ sheet in Sheets)
			{
				result.push_back(Denisenko::Raskroy::Part(
					(sheet->Length - Parameters->CutOffLeft - Parameters->CutOffRight).Scaled,
					(sheet->Width - Parameters->CutOffTop - Parameters->CutOffBottom).Scaled));
			}
			return result;
		}

		CuttingScheme^ ConvertResult(const Denisenko::Raskroy::t_result& result)
		{
			CuttingResultBuilder^ builder = gcnew CuttingResultBuilder();
			return builder->Convert(result, Parameters,
				Size::FromScaled(result.sheet->Rect.Length),
				Size::FromScaled(result.sheet->Rect.Width));
		}
	};
}
}
}