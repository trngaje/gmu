#!/bin/bash

set -ex

make clean

CC="/usr/bin/arm-linux-gnueabihf-gcc" LFLAGS="-lpthread -lmi_common -lmi_sys -lmi_disp -lmi_panel -lmi_gfx -lmi_divp -lmi_ao -lmad" ./configure --libs /usr/lib/arm-linux-gnueabihf --includes /usr/include/arm-linux-gnueabihf --target-device miyoo --disable=notify-frontend

make
