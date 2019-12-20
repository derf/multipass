override drivers += ,nrf24l01
timer_s ?= 1
loop ?= 1

ifeq (${tx}, 1)
	COMMON_FLAGS += -DNRF24L01TEST_TX
endif
