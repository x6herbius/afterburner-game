#pragma once

#include <cstddef>

template<class T, size_t N>
inline constexpr size_t ArraySize(T(&)[N])
{
	return N;
}
