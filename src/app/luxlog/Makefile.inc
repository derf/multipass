ifdef app
	override loop = 1
	override arch_drivers += ,i2c
	override drivers += ,max44009
endif
