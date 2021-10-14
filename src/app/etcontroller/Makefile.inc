ifdef app
	override timer_s = 0
	loop = 1
endif

ifneq (${etcontroller_reset_pin}, )
	COMMON_FLAGS += -DCONFIG_ETCONTROLLER_RESET_PIN=${etcontroller_reset_pin}
endif

ifeq (${etcontroller_nrst}, 1)
	COMMON_FLAGS += -DCONFIG_ETCONTROLLER_NRST
endif
