#pragma once

#include "pch.hpp"

template<typename T>
using ComPtr = wil::com_ptr_nothrow<T>;
