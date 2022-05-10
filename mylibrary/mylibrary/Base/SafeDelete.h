#pragma once

template <class T>
inline void Safe_Delete(T*& p)
{
	delete p;
	p = nullptr;
}