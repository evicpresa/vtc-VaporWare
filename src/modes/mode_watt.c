#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include <Atomizer.h>
#include <Display.h>

#include "display.h"
#include "drawables.h"
#include "font/font_vaporware.h"
#include "globals.h"
#include "settings.h"
#include "helper.h"


void wattInit() {
    g.volts = wattsToVolts(s.targetWatts, g.atomInfo.resistance);
    Atomizer_SetOutputVoltage(g.volts);
}

void wattFire() {
    uint32_t newVolts;

    while (gv.fireButtonPressed) {
        // Handle fire button
        if (!Atomizer_IsOn() && g.atomInfo.resistance != 0
            && Atomizer_GetError() == OK) {
            // Power on
            Atomizer_Control(1);
        }
        // Update info
        // If resistance is zero voltage will be zero
        Atomizer_ReadInfo(&g.atomInfo);

        newVolts = wattsToVolts(s.targetWatts, g.atomInfo.resistance);

        if (newVolts != g.volts || !g.volts) {
            if (Atomizer_IsOn()) {

            // Update output voltage to correct res variations:
            // If the new voltage is lower, we only correct it in
            // 10mV steps, otherwise a flake res reading might
            // make the voltage plummet to zero and stop.
            // If the new voltage is higher, we push it up by 100mV
            // to make it hit harder on TC coils, but still keep it
            // under control.
            if (newVolts < g.volts) {
                newVolts = g.volts - (g.volts >= 10 ? 10 : 0);
            } else {
                newVolts = g.volts + 100;
            }

            }

            if (newVolts > ATOMIZER_MAX_VOLTS) {
                newVolts = ATOMIZER_MAX_VOLTS;
            }

            g.volts = newVolts;

            Atomizer_SetOutputVoltage(g.volts);
        }

        updateScreen(&g);
    }
    
    if (Atomizer_IsOn())
        Atomizer_Control(0);
}

void wattUp() {
    uint32_t tempWatts = s.targetWatts + 100;
    if (tempWatts <= MAXWATTS) {
        targetWattsSet(tempWatts);
    } else {
        targetWattsSet(MAXWATTS);
    }
}

void wattDown() {
    uint32_t tempWatts = s.targetWatts - 100;
    if (tempWatts > MINWATTS) {
        targetWattsSet(tempWatts);
    } else {
        targetWattsSet(MINWATTS);
    }
}

void wattGetText(char *buff, uint8_t len) {
    sniprintf(buff, len, "%"PRIu32".%01"PRIu32"W", s.targetWatts / 1000, s.targetWatts % 1000 / 100);
}

void wattGetAltText(char *buff, uint8_t len) {
    printNumber(buff, len, CToDisplay(g.curTemp));
}

struct vapeMode variableWattage = {
    .index = 0,
    .controlType = WATT_CONTROL,
    .name = "Wattage",
    .supportedMaterials = KANTHAL | NICKEL | TITANIUM | STAINLESS,
    .init = &wattInit,
    .fire = &wattFire,
    .increase = &wattUp,
    .decrease = &wattDown,
    .maxSetting = 75000,
    .getAltDisplayText = &wattGetAltText,
    .getDisplayText = &wattGetText,
    .altIconDrawable = TEMPICON,
};

static void __attribute__((constructor)) registerWattMode(void) {
    addVapeMode(&variableWattage);
}