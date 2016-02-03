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
 */

#ifndef EVICSDK_USB_VCOM_H
#define EVICSDK_USB_VCOM_H

#include <M451Series.h>

/**
 * Initializes the USB virtual COM port.
 * System control registers must be unlocked.
 */
void USB_VirtualCOM_Init();

/**
 * Sends data over the USB virtual COM port.
 *
 * @param buf  Data buffer.
 * @param size Number of bytes to send.
 */
void USB_VirtualCOM_Send(const uint8_t *buf, uint32_t size);

/**
 * Sends a string over the USB virtual COM port.
 *
 * @param str String to send.
 */
void USB_VirtualCOM_SendString(const char *str);

/**
 * Gets the number of received bytes available for reading.
 *
 * @return Number of available bytes.
 */
uint16_t USB_VirtualCOM_GetAvailableSize();

/**
 * Reads received data from the USB virtual COM port.
 * If the asked read size exceeds the available data size,
 * it will be reduced.
 *
 * @param buf  Destination buffer.
 * @param size Number of bytes to read.
 *
 * @return Number of bytes actually read.
 */
uint16_t USB_VirtualCOM_Read(uint8_t *buf, uint16_t size);

#endif
