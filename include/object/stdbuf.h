#ifndef STDBUF_H
#define STDBUF_H

#include "object/outputstream.h"

template <class T = OutputStream>
class BufferOutput : public T {
	private:
		BufferOutput(const BufferOutput &copy);
		char *buffer;

	public:
		BufferOutput(char *target) { buffer = target; }
		virtual void put(char c) {
			*buffer = c;
			buffer++;
		}
};

#endif
