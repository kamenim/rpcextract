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

sector_t		NspiSector[] =
{
	{ 0, "NspiBind"},
	{ 1, "NspiUnbind"},
	{ 2, "NspiUpdateStat"},
	{ 3, "NspiQueryRows"},
	{ 4, "NspiSeekEntries"},
	{ 5, "NspiGetMatches"},
	{ 6, "NspiResortRestriction"},
	{ 7, "NspiDNToEph"},
	{ 8, "NspiGetPropList"},
	{ 9, "NspiGetProps"},
	{ 10, "NspiCompareDNTs"},
	{ 11, "NspiModProps"},
	{ 12, "NspiGetHierarchyInfo"},
	{ 13, "NspiGetTemplateInfo"},
	{ 14, "NspiModLinkAttr"},
	{ 15, "NspiDeleteEntries"},
	{ 16, "NspiQueryColumns"},
	{ 17, "NspiGetNamesFromIDs"},
	{ 18, "NspiGetIDsFromNames"},
	{ 19, "NspiResolveNames"},
	{ 0, NULL}
};

const char		*GetNspiOpStr(short opnum)
{
	int		i;

	for (i = 0; NspiSector[i].opstr; i++)
	{
		if (opnum == NspiSector[i].opnum)
			return (NspiSector[i].opstr);
	}
	return (UNKNOWN_NSPI_OP);
}
