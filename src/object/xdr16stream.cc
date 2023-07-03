/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "object/xdr16stream.h"

XDRStream & XDRStream::operator<<(unsigned char c)
{
	*this << (uint16_t)c;
	return *this;
}

XDRStream & XDRStream::operator<<(char c)
{
	*this << (int16_t)c;
	return *this;
}

XDRStream & XDRStream::operator<<(uint16_t number)
{
	put((number >> 8) & 0xffU);
	put(number & 0xffU);
	return *this;
}

XDRStream & XDRStream::operator<<(int16_t number)
{
	put((number >> 8) & 0xffU);
	put(number & 0xffU);
	return *this;
}

XDRStream & XDRStream::operator<<(uint32_t number)
{
	put((number >> 24) & 0xffU);
	put((number >> 16) & 0xffU);
	put((number >> 8) & 0xffU);
	put(number & 0xffU);
	return *this;
}

XDRStream & XDRStream::operator<<(int32_t number)
{
	put((number >> 24) & 0xffU);
	put((number >> 16) & 0xffU);
	put((number >> 8) & 0xffU);
	put(number & 0xffU);
	return *this;
}

XDRStream & XDRStream::operator<<(uint64_t number)
{
	put((number >> 56) & 0xffU);
	put((number >> 48) & 0xffU);
	put((number >> 40) & 0xffU);
	put((number >> 32) & 0xffU);
	put((number >> 24) & 0xffU);
	put((number >> 16) & 0xffU);
	put((number >> 8) & 0xffU);
	put(number & 0xffU);
	return *this;
}

XDRStream & XDRStream::operator<<(int64_t number)
{
	put((number >> 56) & 0xffU);
	put((number >> 48) & 0xffU);
	put((number >> 40) & 0xffU);
	put((number >> 32) & 0xffU);
	put((number >> 24) & 0xffU);
	put((number >> 16) & 0xffU);
	put((number >> 8) & 0xffU);
	put(number & 0xffU);
	return *this;
}

XDRStream & XDRStream::operator<<(float number)
{
	union {
		uint32_t i;
		float f;
	} v;
	v.f = number;
	*this << v.i;
	return *this;
}

XDRStream & XDRStream::operator<<(double number)
{
	union {
		uint64_t i;
		double d;
	} v;
	v.d = number;
	*this << v.i;
	return *this;
}

XDRStream & XDRStream::operator<<(char const *data){
	if (!is_fixed_length) {
		*this << next_array_len;
	}
	uint32_t i;
	for (i = 0; i < next_array_len; i++) {
		put(data[i]);
	}
	while ((i++) % 2 != 0){
		put('\0');
	}
	return *this;
}

template<uint16_t TSize>
XDRStream & XDRStream::operator<<(char const (&data)[TSize]){
	if (!is_fixed_length) {
		*this << TSize;
	}
	uint32_t i;
	for (i = 0; i < TSize; i++) {
		put(data[i]);
	}
	while ((i++) % 2 != 0){
		put('\0');
	}
	return *this;
}

XDRStream & XDRStream::operator<<(XDRStream & (*fkt) (XDRStream &))
{
	return fkt(*this);
}

XDRStream & flush(XDRStream & os)
{
	os.flush();
	return os;
}

XDRStream & term(XDRStream & os)
{
	os.put('\0');
	os.flush();
	return os;
}

template<int N>
XDRStream & opaque(XDRStream & os)
{
	os.setNextArrayLen(N);
	return os;
}

XDRStream & fixed(XDRStream & os)
{
	os.setFixedLength();
	return os;
}

XDRStream & variable(XDRStream & os)
{
	os.setVariableLength();
	return os;
}
