/******************************************************************************
*
 * Copyright(c) 2015 - 2017 Realtek Corporation.
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of version 2 of the GNU General Public License as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
 *****************************************************************************/



/******************************************************************************
*                           MSDIO.TXT
******************************************************************************/


u16 EFUSE_GetArrayLen_MP_8822E_MSDIO(void);

void EFUSE_GetMaskArray_MP_8822E_MSDIO(u8 *Array);

BOOLEAN EFUSE_IsAddressMasked_MP_8822E_MSDIO(u16 Offset);

u16 EFUSE_GetBTArrayLen_MP_8822E_MSDIO(void);

void EFUSE_GetBTMaskArray_MP_8822E_MSDIO(u8 *Array);

BOOLEAN EFUSE_IsBTAddressMasked_MP_8822E_MSDIO(u16 Offset);

void EFUSE_Ver_GetArray_MP_8822E_MSDIO(u8 *Array);

u16 EFUSE_Ver_GetArrayLen_MP_8822E_MSDIO(void);

