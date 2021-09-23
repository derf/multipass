# vim:ft=make
#
# Copyright 2021 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	loop = 1
endif

COMMON_FLAGS += -Isrc/lib/MCCI_LoRaWAN_LMIC_library/src

C_TARGETS += src/lib/MCCI_LoRaWAN_LMIC_library/src/lmic/lmic.c
C_TARGETS += src/lib/MCCI_LoRaWAN_LMIC_library/src/lmic/lmic_eu868.c
C_TARGETS += src/lib/MCCI_LoRaWAN_LMIC_library/src/lmic/lmic_util.c
C_TARGETS += src/lib/MCCI_LoRaWAN_LMIC_library/src/lmic/lmic_eu_like.c
C_TARGETS += src/lib/MCCI_LoRaWAN_LMIC_library/src/lmic/lmic_channelshuffle.c
C_TARGETS += src/lib/MCCI_LoRaWAN_LMIC_library/src/lmic/oslmic.c
C_TARGETS += src/lib/MCCI_LoRaWAN_LMIC_library/src/lmic/radio.c
C_TARGETS += src/lib/MCCI_LoRaWAN_LMIC_library/src/aes/lmic.c
C_TARGETS += src/lib/MCCI_LoRaWAN_LMIC_library/src/aes/other.c

CXX_TARGETS += src/lib/MCCI_LoRaWAN_LMIC_library/src/hal/hal.cc
CXX_TARGETS += src/lib/MCCI_LoRaWAN_LMIC_library/src/aes/ideetron/AES-128_V10.cc
