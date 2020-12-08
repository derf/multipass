#!/usr/bin/env python3
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

import numpy as np
import sys

# include/arch/msp430fr5969lp/driver/timer.h
def get_timer_frequency(f_cpu, f_timer):
    if f_cpu == 16000000:
        ta0_main_div = 8
    elif f_cpu == 8000000:
        ta0_main_div = 4
    elif f_cpu == 4000000:
        ta0_main_div = 2
    elif f_cpu == 1000000:
        ta0_main_div = 1
    else:
        raise ValueError("Invalid f_cpu")

    if f_cpu == 1000000:
        if f_timer >= 1000:
            divisor = 1
            counter = 1000 / (f_timer / 1000)
        elif f_timer >= 20:
            divisor = 1
            counter = 1000000 / f_timer
        else:
            divisor = 8 * 2
            counter = 62500 / f_timer
    else:
        if f_timer >= 1000:
            divisor = ta0_main_div * 1
            counter = 2000 / (f_timer / 1000)
        else:
            divisor = ta0_main_div * 1
            counter = 2000000 / f_timer

    return f_cpu / divisor / int(counter)

module = sys.argv[1]

if module == 'f_timer' and len(sys.argv) == 4:
    try:
        f_cpu = int(sys.argv[2])
        f_timer = int(sys.argv[3])
    except:
        sys.exit(0)
    if f_cpu != 0 and f_timer != 0:
        print('{:.2f}'.format(get_timer_frequency(f_cpu, f_timer)))
