#ifndef ADC_H
#define ADC_H

class ADC {
	private:
		ADC(ADC const &copy);

	public:
		ADC() {}

		float getTemp();
		float getVCC();
};

extern ADC adc;

#endif
