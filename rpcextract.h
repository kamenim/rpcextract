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

#ifndef			__RPCEXTRACT_H_
#define			__RPCEXTRACT_H_

#ifndef			__BSD_VISIBLE
#define			_GNU_SOURCE
#endif

#include		<stdlib.h>
#include		<unistd.h>
#include		<stdio.h>
#include		<string.h>
#include		<ctype.h>
#include		<limits.h>
#include		<fcntl.h>
#include		<errno.h>
#include		<sys/types.h>
#include		<sys/socket.h>
#include		<sys/stat.h>

#include		<pcap.h>

#include		<netinet/in_systm.h>
#include		<netinet/in.h>
#include		<netinet/ip.h>
#include		<ifaddrs.h>
#include		<netdb.h>
#include		<net/if.h>
#include		<net/ethernet.h>
#include		<netinet/tcp.h>
#include		<arpa/inet.h>

#ifdef			__BSD_VISIBLE
#define	SPORT		th_sport
#define	DPORT		th_dport
#else
#define	SPORT		source
#define	DPORT		dest
#endif

#define	OPTIONS		"vp:i:d:l:"
#define	USAGE		"%s : -i <pcap file> | -l <device> [-v] [-d <outdir>] [-p <proto_name#1>] [-p <proto_name#2>] ...\n"
#define	USAGE_ERROR	0xBADF00D
#define	FAILURE_ERROR	0xBADC0DE

#define	UNKNOWN_PROTO	"UnknownProtocol"
#define UNKNOWN_NSPI_OP	"UnknownNspiOperation"
#define UNKNOWN_MAPI_OP	"UnknownMapiOperation"

#define RPC_REQ		0
#define RPC_RESP	2

#define	ETHERTYPE_IPINV	0x0008

#define FLWD_TSTRM	(((strncmp(cur_ip_src, ip_src, IP_LEN) == 0) && (strncmp(cur_ip_dst, ip_dst, IP_LEN) == 0)) || ((strncmp(cur_ip_src, ip_dst, IP_LEN) == 0) && (strncmp(cur_ip_dst, ip_src, IP_LEN) == 0)))
#define FLWD_DPORT	(dst_port == ENDPM_PORT)
#define FLWD_SPORT	(src_port == ENDPM_PORT)
#define PKT_GOT_DATA	(psh)
#define	ACK_ONLY	((ack) && (!syn) && (!ffin))

#define SNAP_LEN	1514
#define TIMEOUT		1000
#define	IP_LEN		17
#define	ENDPM_PORT	135
#define LEN2ENDPMAP	78
#define	ENDPMAPHLEN	20
#define FLOORSTUFFLEN	13
#define TOWERSTUFFLEN	14
#define	UUID_LEN	16
#define	UUIDPTRLEN	20
#define	LEN2FLOOR	10
#define	LEN2ALLOCH	16

typedef	struct		filter_s
{
	char		*filter;
	struct filter_s *next_filter;
}			filter_t;

typedef struct		proto_s
{
	const char	*uuid;
	const char	*name;
}			proto_t;

typedef struct		sector_s
{
	short		opnum;
	const char	*opstr;
}			sector_t;

typedef struct		bport_s
{
	char		*proto;
	int		bport;
	struct bport_s	*next_bport;
}			bport_t;

typedef	struct		opts_s
{
	char		*exe_file;
	char		*infile;
	char		*device;
	char		*outdir;
	filter_t	*filters;
	int		verbose:1;
}			opts_t;

extern char		*optarg;
extern int		optind;
extern int		optopt;
extern int		opterr;
extern int		optreset;

extern int		errno;

opts_t			opts;
bport_t			*bports;
short			last_opnum;

void			parse_options(int argc, char **argv);
const char		*GetNspiOpStr(short opnum);
const char		*GetMapiOpStr(short opnum);

#endif			/* __RPCEXTRACT_H_ */
