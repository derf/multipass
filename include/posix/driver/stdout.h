#ifndef STANDARDOUTPUT_H
#define STANDANDOUTPUT_H

#include "object/outputstream.h"

class StandardOutput : public OutputStream {
	private:
		StandardOutput(const StandardOutput &copy);

	public:
		StandardOutput () {}
		void setup() {}

		virtual void put(char c) override;
		virtual void flush() override;
};

extern StandardOutput kout;

#endif
