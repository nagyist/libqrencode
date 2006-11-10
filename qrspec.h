/*
 * qrencode - QR-code encoder
 *
 * Copyright (C) 2006 Kentaro Fukuchi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __QRSPEC_H__
#define __QRSPEC_H__

#include "qrencode.h"

/******************************************************************************
 * Version and capacity
 *****************************************************************************/

#define QRSPEC_VERSION_MAX 40

typedef struct {
	int length; //< Edge length of the symbol
	int words;   //< Data capacity (bytes)
	int ec[4];
} QRspec_Capacity;

extern QRspec_Capacity qrspecCapacity[];

/**
 * Return maximum data code length (bytes) for the version.
 * @param version
 * @param level
 * @return maximum size (bytes)
 */
extern int QRspec_getMaximumCodeLength(int version, QRenc_ErrorCorrectionLevel level);

/**
 * Return a version number that satisfies the input code length.
 * @param size input code length (byte)
 * @return version number
 */
extern int QRspec_getMinimumVersion(int size, QRenc_ErrorCorrectionLevel level);

/******************************************************************************
 * Length indicator
 *****************************************************************************/

/**
 * Return the size of lenght indicator for the mode and version.
 * @param mode
 * @param version
 * @return the size of the appropriate length indicator (bits).
 */
extern int QRspec_lengthIndicator(QRenc_EncodeMode mode, int version);

/**
 * Return the maximum length for the mode and version.
 * @param mode
 * @param version
 * @return the maximum length (bytes)
 */
extern int QRspec_maximumWords(QRenc_EncodeMode mode, int version);

#endif /* __QRSPEC_H__ */