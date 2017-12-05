#ifndef GPIO_H
#define GPIO_H

class GPIO {
	private:
		GPIO(const GPIO &copy);

	public:
		GPIO () {}
		void setup();
		void led_on(unsigned char id);
		void led_off(unsigned char id);
		void led_toggle(unsigned char id);
};

extern GPIO gpio;

#endif
