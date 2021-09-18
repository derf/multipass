#ifndef STANDARDOUTPUT_H
#define STANDARDOUTPUT_H

#include "object/outputstream.h"

class StandardOutput : public OutputStream {
	private:
		StandardOutput(const StandardOutput &copy);

	public:
		StandardOutput () {}
		void setup();

		virtual void put(char c) override;
};

extern StandardOutput kout;

#endif
