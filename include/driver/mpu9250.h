#ifndef MPU9250_H
#define MPU9250_H

class MPU9250 {
	private:
		MPU9250(const MPU9250 &copy);
		unsigned char const address = 0x68;
		unsigned char txbuf[3];
		unsigned char rxbuf[8];
		signed int getWordReg(unsigned char const regBase);

	public:
		MPU9250() {}
		void init();
		int getAccelX() { return getWordReg(59); }
		int getAccelY() { return getWordReg(61); }
		int getAccelZ() { return getWordReg(63); }
		int getGyroX() { return getWordReg(67); }
		int getGyroY() { return getWordReg(69); }
		int getGyroZ() { return getWordReg(71); }
		void getMagnet(int *x, int *y, int *z);
		void setAccelEnable(bool x, bool y, bool z);
		void setGyroEnable(bool x, bool y, bool z);
		void setPower(bool sleep, bool gyroStandby);
		float getTemperature();
};

extern MPU9250 mpu9250;

#endif
