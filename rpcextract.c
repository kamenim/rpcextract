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

#include			"rpcextract.h"

proto_t				protos[] =
{
	{"\x18\x5a\xcc\xf5\x64\x42\x1a\x10\x8c\x59\x08\x00\x2b\x2f\x84\x26", "Nspi"},
	/* {"f5cc5a18-4264-101a-8c59-08002b2f8426", "Nspi"}, */
	{"\x00\xdb\xf1\xa4\x47\xca\x67\x10\xb3\x1f\x00\xdd\x01\x06\x62\xda", "Mapi"},
	/* {"a4f1db00-ca47-1067-b31f-00dd010662da", "Mapi"}, */
	/* {"\x04\x5d\x88\x8a\xeb\x1c\xc9\x11\x9f\xe8\x08\x00\x2b\x10\x48\x60", "Epm"}, */
	{0, 0}
};

void				add_binded_port(char *proto_name, int proto_port)
{
	bport_t			*new_bport;

	new_bport = (bport_t *)malloc(sizeof(bport_t));
	new_bport->proto = strdup(proto_name);
	new_bport->bport = proto_port;
	new_bport->next_bport = bports;
	bports = new_bport;
}

int				seek_port(bport_t **bport, u_int port1, u_int port2)
{
	bport_t			*save;

	save = bports;
	while (save)
	{
		if ((save->bport == port1) || (save->bport == port2))
		{
			*bport = save;
			return (1);
		}
		save = save->next_bport;
	}
	return (0);
}

int				is_in_filters(char *proto)
{
	filter_t		*save;

	save = opts.filters;
	while (save)
	{
		if (strcmp(proto, save->filter) == 0)
			return (1);
		save = save->next_filter;
	}
	return (0);
}

char				*rip_proto(u_char *data)
{
	u_char			*endp_map = data + LEN2ENDPMAP;
	u_char			*tower_ptr = endp_map + UUIDPTRLEN;
	u_char			*fst_floor = tower_ptr + LEN2FLOOR + 4;
	short			*nb_floors = (short *)(tower_ptr + 4 + (LEN2FLOOR - 2));
	short			*lhs;
	short			*rhs;
	u_char			*prot;
	u_char			*uuid;
	int			i;
	int			j;

	uuid = malloc(sizeof(char) * (UUID_LEN + 1));
	uuid[UUID_LEN] = 0;
	for (i = 0; i < (*nb_floors); i++)
	{
		lhs = (short *)fst_floor;
		prot = (u_char *)fst_floor + sizeof(short);
		rhs = (short *)(fst_floor + sizeof(short) + (*lhs));
		if (*prot == 0x0d)
		{
			strncpy((char *)uuid, (char *)fst_floor + sizeof(short) + sizeof(u_char), UUID_LEN);
			for (j = 0; protos[j].name; j++)
			{
				if (strncmp((char *)protos[j].uuid, (char *)uuid, UUID_LEN) == 0)
					return (strdup(protos[j].name));
			}			
		}
		fst_floor += (*lhs) + (*rhs) + (2 * sizeof(short));
	}
	return (UNKNOWN_PROTO);
}

int				rip_port(u_char *data)
{
	u_char			*endp_map = data + LEN2ENDPMAP;
	int			*nb_towers = (int *)(endp_map + ENDPMAPHLEN);
	u_char			*tower_ptr = endp_map + ENDPMAPHLEN + 16;
	short			*nb_floors = (short *)(tower_ptr + (4 * (*nb_towers)) + (LEN2FLOOR - 2));
	u_char			*fst_floor = tower_ptr + (4 * (*nb_towers))+ LEN2FLOOR;
	short			*lhs;
	short			*rhs;
	short			*port;
	u_char			*prot;
	int			i;

	for (i = 0; i < (*nb_floors); i++)
	{
		lhs = (short *)fst_floor;
		prot = (u_char *)fst_floor + sizeof(short);
		rhs = (short *)(fst_floor + sizeof(short) + (*lhs));
		if (*prot == 0x07)
		{
			port = (short *)(fst_floor + (2 * sizeof(short)) + (*lhs));
			return (ntohs(*port));
		}
		fst_floor += (*lhs) + (*rhs) + (2 * sizeof(short));
	}
	return (0);
}

void				rip_file(int packet_num, char packet_type, char *proto, u_char *tcp)
{
	short			*opnum;
	u_int			*alloch;
	const char		*opstr = NULL;
	char			*filename;
	char			*olddir = getcwd(NULL, 0);
	int			fd;

	alloch = (u_int *)(tcp + sizeof(struct tcphdr) + LEN2ALLOCH);
	if (packet_type == RPC_REQ)
	  opnum =  (short *)(tcp + sizeof(struct tcphdr) + LEN2ALLOCH + 6);
	else
	  opnum = (short *)(&last_opnum);
	if (strcmp(proto, "Nspi") == 0)
		opstr = GetNspiOpStr(*opnum);
	if (strcmp(proto, "Mapi") == 0)
		opstr = GetMapiOpStr(*opnum);
	asprintf(&filename, "%05d_%s_%s_%s", packet_num, (packet_type == RPC_REQ)?"in":"out", proto, opstr);
	if (opts.verbose)
		printf("Extracting data into file %s (size = %d bytes)\n", filename, *alloch);
	if (opts.outdir)
	  {
	    if (chdir(opts.outdir) < 0)
	      perror("chdir");	    
	  }
	fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0666);
	if (fd < 0)
	{
		perror("open");
		exit(FAILURE_ERROR);
	}
	write(fd, (char *)(tcp + sizeof(struct tcphdr) + LEN2ALLOCH + 8), *alloch);
	close(fd);
	if (opts.outdir)
		chdir(olddir);
	if (opts.verbose)
		printf("Extraction accomplished\n");
	last_opnum = *opnum;
}

void				extr_cb(u_char *param, const struct pcap_pkthdr *header, const u_char * data)
{
	struct ether_header	*packet_ether;
	struct ip		*packet_ip;
	struct tcphdr		*packet_tcp;
	static int		i = 1;
	static int		init = 0;
	static int		endp_bind = 0;
	static int		endp_deco = 0;
	static char		*proto = NULL;
	static int		binded = -1;
	static int		fin = 0;
	static char		ip_src[IP_LEN + 1];
	static char		ip_dst[IP_LEN + 1];
	char			cur_ip_src[IP_LEN + 1];
	char			cur_ip_dst[IP_LEN + 1];
	u_int			size;
	u_int			src_port;
	u_int			dst_port;
	u_int			no_hands = 0;
	u_int			ack;
	u_int			syn;
	u_int			ffin;
	u_int			psh;
	char			*packet_type;
	bport_t			*bport;

	ip_src[IP_LEN] = 0;
	ip_dst[IP_LEN] = 0;
	cur_ip_src[IP_LEN] = 0;
	cur_ip_dst[IP_LEN] = 0;

	i++;
  
/* Packet Check */

	packet_ether = (struct ether_header *)(data);
	if (packet_ether->ether_type != ETHERTYPE_IPINV)
	  {
	    if (opts.verbose)
	      printf("%d - Non Ip Packet.. Skiped..\n", i - 1);
	    return ;
	  }

	size = (header->caplen - sizeof(struct ether_header));
	if (size < sizeof(struct ip))
	{
		printf("Packet %d : Uncomplete IP Header..\n", i - 1);
		return ;
	}
	packet_ip = (struct ip *)(data + sizeof(struct ether_header));
	if ((packet_ip->ip_hl * 4) < sizeof (struct ip))
	{
		printf("Packet %d : IP Header error..\n", i - 1);
		return ;
	}
	size = (header->caplen - sizeof(struct ether_header) - (packet_ip->ip_hl * 4));
	if (size < sizeof(struct tcphdr))
	{
		printf("Packet %d : Uncomplete TCP Header..\n", i - 1);
		return ;
	}

/* Tcp Work */

	packet_tcp = (struct tcphdr *)(data + sizeof(struct ether_header) + (packet_ip->ip_hl * 4));
	src_port = ntohs(packet_tcp->SPORT);
	dst_port = ntohs(packet_tcp->DPORT);

#ifndef	__BSD_VISIBLE
	ack = packet_tcp->ack;
	syn = packet_tcp->syn;
	ffin = packet_tcp->fin;
	psh = packet_tcp->psh;
#else
	ack = packet_tcp->th_flags & TH_ACK;
	syn = packet_tcp->th_flags & TH_SYN;
	ffin = packet_tcp->th_flags & TH_FIN;
	psh = packet_tcp->th_flags & TH_PUSH;
#endif

	if (packet_ip->ip_p == IPPROTO_TCP)
	{
		strncpy(cur_ip_src, (char *)inet_ntoa(packet_ip->ip_src), IP_LEN);
		strncpy(cur_ip_dst, (char *)inet_ntoa(packet_ip->ip_dst), IP_LEN);
		if (init && (endp_bind == 0) && FLWD_TSTRM && (FLWD_DPORT || FLWD_SPORT) && ffin)
		  endp_deco++;
		if (fin)
		{
			no_hands = 1;
			fin = 0;
		}
		if (ffin)
			fin = 1;
		if (bports)
		{
			if (FLWD_TSTRM && (seek_port(&bport, dst_port, src_port) == 1) && PKT_GOT_DATA)
			{
				packet_type = (char *)((char *)packet_tcp + sizeof(struct tcphdr) + 2);
				if ((*packet_type == RPC_REQ) || (*packet_type == RPC_RESP))
					rip_file(i - 1, *packet_type, bport->proto, (u_char *)packet_tcp);
			}
		}
		if ((endp_bind) && FLWD_TSTRM && FLWD_DPORT)
		{
		manage_epm_req:
			packet_type = (char *)((char *)packet_tcp + sizeof(struct tcphdr) + 2);
			if (*packet_type == RPC_REQ)
			{
				if (opts.verbose)
					printf("Got endpoint mapper request on packet %d\n", i - 1);
				proto = rip_proto((u_char *)(const u_char *)data);
				if (strcmp(proto, UNKNOWN_PROTO) == 0)
					return ;
				if (opts.verbose)
					printf("Proto Asked : %s\n", proto);
			}
		}
		if ((endp_bind) && FLWD_TSTRM && FLWD_SPORT)
		{
			packet_type = (char *)((char *)packet_tcp + sizeof(struct tcphdr) + 2);
			if (*packet_type == RPC_RESP)
			{
				if (opts.verbose)
					printf("Got endpoint mapper response on packet %d\n", i - 1);
				binded = rip_port((u_char *)(const u_char *)data);
				if (binded == 0)
					return ;
				endp_bind = 0;
				if (opts.verbose)
				  printf("Endpoint mapper response : %d\n", binded);
				if (strcmp(proto, UNKNOWN_PROTO) == 0)
					return ;
				if (opts.filters)
				{
					if (is_in_filters(proto))
						add_binded_port(proto, binded);
					else
					{
						if (opts.verbose)
							printf("Protocol %s fetched but not followed because filtered..\n", proto);
					}
				}
				else
					add_binded_port(proto, binded);
			}
			else
				return ;
		}
		if (endp_bind)
			return ;
		if (init && (!no_hands) && FLWD_TSTRM && FLWD_DPORT && ACK_ONLY)
		{
			if (opts.verbose)
				printf("Got end of handshake on packet %d\n", i - 1);
			endp_bind = 1;
			endp_deco = 0;
			goto manage_epm_req;
		}
		if ((init == 0) && FLWD_DPORT)
		{
			init = 1;
			strncpy(ip_src, (char *)inet_ntoa(packet_ip->ip_src), IP_LEN);
			strncpy(ip_dst, (char *)inet_ntoa(packet_ip->ip_dst), IP_LEN);
		}
	}
}

int				main(int argc, char **argv)
{
	char			err_buff[PCAP_ERRBUF_SIZE];
	pcap_t			*desc_pqt;

	bports = NULL;
	last_opnum = -1;
	parse_options(argc, argv);
	if (opts.device == NULL)
	  {
	    if ((desc_pqt = pcap_open_offline(opts.infile, err_buff)) == NULL)
	      {
		printf("Error opening pcap file\n%s..\nExiting..\n", err_buff);
		exit(FAILURE_ERROR);
	      }
	  }
	else
	  {
	    if ((desc_pqt = pcap_open_live(opts.device, SNAP_LEN, IFF_PROMISC, TIMEOUT, err_buff)) == NULL)
	      {
		printf("Error opening network device\n%s..\nExiting..\n",err_buff);
		exit(FAILURE_ERROR);
	      }
	    printf("Listening on device %s..\n", opts.device); 
	  }
	if (opts.outdir)
	{
		if ((mkdir(opts.outdir, 0777) < 0) && (errno != EEXIST))
		{
			perror("mkdir");
			opts.outdir = NULL;
		}
	}
	if (pcap_loop(desc_pqt, -1, extr_cb, NULL) < 0)
	{
		printf("Error entering capture loop\n%s..\nExiting..\n", pcap_geterr(desc_pqt));
		exit(FAILURE_ERROR);
	}
	pcap_close(desc_pqt);

	return (0);
}
