/*
 * This file is part of eVic SDK.
 *
 * eVic SDK is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * eVic SDK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eVic SDK.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2016 ReservedField
 * Copyright (C) 2016 kfazz
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <M451Series.h>
#include <Display.h>
#include <TimerUtils.h>
#include <Button.h>
#include <USB_VirtualCOM.h>
#include <Dataflash.h>
#include <SysInfo.h>

#include "button.h"
#include "dataflash.h"
#include "display.h"
#include "globals.h"
#include "helper.h"
#include "menu.h"
#include "settings.h"
#include "font/font_vaporware.h"

void saveDefaultSettings();

#define MAXOPTIONS 16

char *typeIdString[MAXOPTIONS];
int typeIdMapping[MAXOPTIONS];

void populateTypes(struct menuItem *MI) {
    uint8_t index = 0;
    struct vapeMaterials *VM;
    while ((VM = &vapeMaterialList[index])->name[0] != '\0') {
        typeIdString[index] = VM->name;
        typeIdMapping[index] = index;
        index++;
    }
    MI->items = &typeIdString;
    MI->count = index;
    MI->startAt = s.materialIndex;
}

void updateType(uint16_t index) {
    materialIndexSet(index);
}

char *modeIdString[MAXOPTIONS];
int modeIdMapping[MAXOPTIONS];

void populateModes(struct menuItem *MI) {
    uint8_t index = 0;
    struct vapeMode *VPM;
    while ((VPM = g.vapeModes[index])->name[0] != '\0') {
        modeIdString[index] = VPM->name;
        modeIdMapping[index] = VPM->index;
        index++;
    }
    MI->items = &modeIdString;
    MI->count = index;
    MI->startAt = s.mode;
}

void updateMode(uint16_t index) {
    setVapeMode(index);
}

void formatBrightnessNumber(int32_t value, char *formatted) {
    Display_SetContrast((char*)value);
    siprintf(formatted, "%lu", value);
}

void updateScreenBrightness(int32_t value) {
	screenBrightnessSet(value);
}

char *scaleIdString[MAXOPTIONS];
int scaleIdMapping[MAXOPTIONS];

void populateScales(struct menuItem *MI) {
    uint8_t index = 0;
    struct tempScale *TS;
    index = 0;
    while ((TS = &tempScaleType[index])->display[0] != '\0') {
        scaleIdString[index] = TS->display;
        scaleIdMapping[index] = index;
        index++;
    }
    MI->items = &scaleIdString;
    MI->count = index;
    MI->startAt = s.tempScaleTypeIndex;
}

void updateScale(uint16_t index) {
    s.tempScaleTypeIndex = index;
}

void showInfo(void) {
    char buff[63];
    uint8_t hwVerMajor, hwVerMinor;
    hwVerMajor = gSysInfo.hwVersion / 100;
    hwVerMinor = gSysInfo.hwVersion % 100;

    Display_Clear();

    Display_PutText(0, 0, "FW Ver", FONT_SMALL);
    siprintf(buff, "%s", "-0.01");
    Display_PutText(10, 15, buff, FONT_SMALL);

    Display_PutText(0, 40, "HW Ver", FONT_SMALL);
    siprintf(buff, "%d.%02d", hwVerMajor, hwVerMinor);
    Display_PutText(10, 55, buff, FONT_SMALL);

    Display_PutText(0, 80, "Display", FONT_DEJAVU_8PT);
    Display_PutText(10, 95, Display_GetType() == DISPLAY_SSD1327 ? "1327" : "1306", FONT_SMALL);

    Display_Update();

    while(Button_GetState()){;}
}

void reboot() {
    /* Unlock protected registers */
    SYS_UnlockReg();
    SYS_ResetChip();
}

void factoryReset() {
    defaultSettings();
    writeSettings();
    reboot();
}

#ifdef WITHFLASHDAMAGESUPPORT
void invalidateDataFlash() {
    if(!(Button_GetState() & BUTTON_MASK_RIGHT))
        return;

    makeDFInvalid();
}

void eraseDataFlash() {
    if(!(Button_GetState() & BUTTON_MASK_RIGHT))
        return;

    eraseDF();
}
#endif

void invertSet(uint8_t a){
	invertDisplaySet(a);

}

void flipSet(uint8_t a) {
	flipOnVapeSet(s.flipOnVape);
}

uint8_t display_flip = FLIPDEF;
uint8_t display_invert = INVERTDEF;
struct menuItem displaySubMenuItems[] = {
	{
	    .type = SELECT,
	    .label = "Scale",
	    /* .items = assigned before calling */
	    /* .startAt assinged before calling */
	    /* .count = assigned before calling */
	    .populateCallback = &populateScales,
	    .selectCallback = &updateScale,
	},
    {
        .type = EDIT,
        .label = "Brightness",
        .editMin = 0,
        .editMax = 255,
        .editStart = (int32_t *)&s.screenBrightness,
        .editCallback = &updateScreenBrightness,
        .editStep = 10,
        .editFormat = &formatBrightnessNumber
    },
	{
	    .type = TOGGLE,
	    .label = "FlipVape",
	    .on = "On",
	    .off = "Off",
	    .isSet = &display_flip,
	    .toggleCallback = &flipSet,
	},
	{
	    .type = TOGGLE,
	    .label = "Invert",
	    .on = "On",
	    .off = "Off",
	    .isSet = &display_invert,
	    .toggleCallback = &invertSet,
	},
    {
        .type = STARTBOTTOM,
    },
    {
        .type = LINE,
    },
    {
        .type = SPACE,
        .rows = 2,
    },
    {
        .type = EXITMENU,
        .label = "Back",
    },
    {
        .type = END,
    }
};

struct menuDefinition displaySettingsMenu = {
    .name = "Display Settings",
    .font = FONT_SMALL,
    .cursor = "*",
    .prev_sel = "<",
    .next_sel = ">",
    .less_sel = "-",
    .more_sel = "+",
    .menuItems = &displaySubMenuItems,
};

void showDisplay(struct menuItem *MI) {
	display_flip = s.flipOnVape;
	display_invert = s.invertDisplay;
	MI->subMenu = &displaySettingsMenu;
}

void showModeSettings(struct menuItem *MI) {
	MI->subMenu = &(*g.vapeModes[s.mode]->vapeModeMenu);
}

int shouldHideMenu() {
	return g.vapeModes[s.mode]->vapeModeMenu == NULL;
}

struct menuItem advancedMenuItems[] = {
    {
        .type = ACTION,
        .label = "Reboot",
        .actionCallback = &reboot,
    },
    {
        .type = ACTION,
        .label = "F.Reset",
        .actionCallback = &factoryReset,
    },
#ifdef WITHFLASHDAMAGESUPPORT
    {
        .type = ACTION,
        .label = "Inv.Fla",
        .actionCallback = &invalidateDataFlash,
    },
    {
        .type = ACTION,
        .label = "Era.Fla",
        .actionCallback = &eraseDataFlash,
    },
#endif

    {
        .type = STARTBOTTOM,
    },
    {
        .type = LINE,
    },
    {
        .type = SPACE,
        .rows = 2,
    },
    {
        .type = EXITMENU,
        .label = "Back",
    },
    {
        .type = END,
    }
};


struct menuDefinition advancedMenu = {
    .name = "Advanced Settings",
    .font = FONT_SMALL,
    .cursor = "*",
    .prev_sel = "<",
    .next_sel = ">",
    .less_sel = "-",
    .more_sel = "+",
    .menuItems = &advancedMenuItems,

};

/* Will always show 3 decimals, todo: make the '3' a param */
void formatFixedPoint(int32_t value, int32_t divisor, char *formatted) {
    if(divisor == 0)
        siprintf(formatted, "infin");
    else
        siprintf(formatted, "%ld.%03ld", value/divisor, value % divisor);
}

void formatThousandths(int32_t value, char *formatted) {
    formatFixedPoint(value, 1000, formatted);
}

void formatINT(int32_t value, char *formatted) {
    siprintf(formatted, "%ld", value);
}

void saveTCR(int32_t value) {
    if (value < 0) {
        /* don't set a default if it's invalid, somehow */
        return;
    }
    tcrSet(value & 0xFFFF);
}

void saveTemp(int32_t value) {
    g.baseFromUser = 1;
    baseTempSet(value & 0xFFFF);
}

void saveBaseRes(int32_t value) {
    g.baseFromUser = 1;
    baseResSet(value & 0xFFFF);
}

struct menuItem dragonMenuItems[] = {
    {
        .type = EDIT,
        .label = "TCR",
        .editMin = TCRMIN,
        .editMax = TCRMAX,
        .editStart = &g.m3,
        .editStep = 1,
        .editFormat = &formatINT,
        .editCallback = &saveTCR,
    },
    {
        .type = EDIT,
        .label = "B.Temp",
        .editMin = BTEMPMIN,
        .editMax = BTEMPMAX,
        .editStart = &g.m2,
        .editStep = 1,
        .editFormat = &formatINT,
        .editCallback = &saveTemp,
    },
    {
        .type = EDIT,
        .label = "B.Res",
        .editMin = 50,
        .editMax = 3450,
        .editStart = &g.m1,
        .editStep = 5,
        .editFormat = &formatThousandths,
        .editCallback = &saveBaseRes
    },
    {
        .type = EXITMENU,
        .label = "Back",
    },
    {
        .type = END,
    }
};

struct menuDefinition TheDragonning = {
    .name = "Dragon Mode On",
    .font = FONT_SMALL,
    .cursor = "*",
    .prev_sel = "<",
    .next_sel = ">",
    .less_sel = "-",
    .more_sel = "+",
    .menuItems = &dragonMenuItems,
};

void showAdvanced(struct menuItem *MI) {
    if (Button_GetState() & BUTTON_MASK_RIGHT) {
        MI->subMenu = &TheDragonning;
    } else {
        MI->subMenu = &advancedMenu;
    }
}


struct menuItem settingsMenuItems[] = {
    {
        .type = SELECT,
        .label = "Type",
        /* .items = assigned before calling */
        /* .startAt assinged before calling */
        /* .count = assigned before calling */
        .populateCallback = &populateTypes,
        .selectCallback = &updateType,
    },
    {
        .type = SELECT,
        .label = "Mode",
        /* .items = assigned before calling */
        /* .startAt assinged before calling */
        /* .count = assigned before calling */
        .populateCallback = &populateModes,
        .selectCallback = &updateMode,
    },
	{
		.type = SUBMENU,
		.label = "Mode Settings",
		.getMenuDef = &showModeSettings,
		.hidden = &shouldHideMenu,
	},
	{
		.type = SUBMENU,
		.label = "Display",
		.getMenuDef = &showDisplay,
	},
    {
        .type = STARTBOTTOM,
    },
    {
        .type = LINE,
    },
    {
        .type = SPACE,
        .rows = 2,
    },
    {
        .type = ACTION,
        .label = "Info",
        .actionCallback = &showInfo,
    },
    {
        .type = SUBMENU,
        .label = "Advnced",
        .getMenuDef = &showAdvanced,
    },
    {
        .type = EXITMENU,
        .label = "Exit",
    },
    {
        .type = END,
    }
};

struct menuDefinition settingsMenu = {
    .name = "Settings",
    .font = FONT_SMALL,
    .cursor = "*",
    .prev_sel = "<",
    .next_sel = ">",
    .less_sel = "-",
    .more_sel = "+",
    .menuItems = &settingsMenuItems,
};

int load_settings(void) {
    s.fromRom = 0;
    // Should become part of globals instead of settings
    s.dumpPids = 0;
    readSettings();

    return 1;
}


void showMenu() {
    runMenu(&settingsMenu);
    if (g.settingsChanged == 1) {
        writeSettings();
    }
}
