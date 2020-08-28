ifdef app
	override arch_drivers += ,i2c
	override drivers += ,ccs811
	override loop = 1
endif
