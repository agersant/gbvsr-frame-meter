#pragma once

template <size_t N>
class Bitmask
{
	uint32_t data[(N + 31) / 32];

public:
	bool get(int bit)
	{
		return data[bit / 32] >> (bit % 32);
	}

	void set(int bit, bool value)
	{
		if (value)
		{
			data[bit / 32] |= 1 << (bit % 32);
		}
		else
		{
			data[bit / 32] &= ~(1 << (bit % 32));
		}
	}
};
