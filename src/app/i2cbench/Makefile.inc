arch_drivers += ,i2c

ifneq (${i2cbench_setos}, )
	COMMON_FLAGS += -DI2CBENCH_SETOS=${i2cbench_setos}
endif
