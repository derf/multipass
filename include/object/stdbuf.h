/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef STDBUF_H
#define STDBUF_H

#include "object/outputstream.h"

template <class T = OutputStream>
class BufferOutput : public T {
	private:
		BufferOutput(const BufferOutput &copy);
		char *buffer;
		uint16_t length;

	public:
		BufferOutput(char *target) : buffer(target), length(0) {}
		virtual void put(char c) {
			*buffer = c;
			buffer++;
			length++;
		}
		inline uint16_t size() {
			return length;
		}
};

#endif
