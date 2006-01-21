using System;

namespace Denisenko.LC4Parser
{
	public struct LC4Numeric
	{
		private Int64 m_scaled;
		public const Int64 Scale = 100000;

		public Double ToDouble()
		{
			return (Double)m_scaled / (Double)Scale;
		}

		public override String ToString()
		{
			return ToDouble().ToString();
		}

		public static LC4Numeric FromScaled(Int64 scaled)
		{
			return new LC4Numeric(scaled);
		}

		public static LC4Numeric FromNonScaled(Int32 nonScaled)
		{
			return new LC4Numeric(nonScaled * Scale);
		}

		public Int64 Scaled
		{
			get { return m_scaled; }
			set { m_scaled = value; }
		}

		private LC4Numeric(Int64 scaled)
		{
			m_scaled = scaled;
		}
	}
}
