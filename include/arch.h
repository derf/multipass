#ifndef ARCH_H
#define ARCH_H

class Arch {
	private:
		Arch(const Arch &copy);

	public:
		Arch () {}
		void setup();
		void idle_loop();
		void delay_us(unsigned char const us);
};

extern Arch arch;

#endif
