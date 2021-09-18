#ifndef ADC_H
#define ADC_H

class AVRADC {
	private:
		AVRADC(AVRADC const &copy);

	public:
		AVRADC() {}

		int16_t getTemp_mdegC(int16_t offset = 205);
		uint16_t getVCC_mV();
		uint16_t getVBat_mV(bool controlCharger);
};

extern AVRADC adc;

#endif
