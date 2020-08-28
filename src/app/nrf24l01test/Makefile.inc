ifdef app
	override loop = 1
	override timer_s = 1
	override drivers += ,nrf24l01
endif

ifeq (${tx}, 1)
	COMMON_FLAGS += -DNRF24L01TEST_TX
endif
