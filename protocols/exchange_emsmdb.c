/*
   RPCExtract Implementation

   Copyright (C) Jerome MEDEGAN 2006

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include		"rpcextract.h"

sector_t		MapiSector[] =
{
	{ 0, "EcDoConnect"},
	{ 1, "EcDoDisconnect"},
	{ 2, "EcDoRpc"},
	{ 3, "EcGetMoreRpc"},
	{ 4, "EcRRegisterPushNotification"},
	{ 5, "EcRUnregisterPushNotification"},
	{ 6, "EcDummyRpc"},
	{ 7, "EcRGetDCName"},
	{ 8, "EcRNetGetDCName"},
	{ 9, "EcDoRpcExt"},
	{0, NULL}
};

const char		*GetMapiOpStr(short opnum)
{
	int		i;

	for (i = 0; MapiSector[i].opstr; i++)
	{
		if (opnum == MapiSector[i].opnum)
			return (MapiSector[i].opstr);
	}
	return (UNKNOWN_MAPI_OP);
}
