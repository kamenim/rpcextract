/*
** parse_options.c for RPC Extract in /home/jmedegan/rpce
** 
** Made by Jerome MEDEGAN
** Login   j.medegan
** Mail   <j.medegan@openchange.org>
** 
** Started on  Sun May 14 20:51:04 2006 Jerome MEDEGAN
** Last update Fri Jun 16 19:17:54 2006 Pauline KHUN
*/

#include			"rpcextract.h"

extern	proto_t			protos[];

void				print_usage()
{
	printf(USAGE, opts.exe_file);
	exit(USAGE_ERROR);
}

void				init_opts(int argc, char **argv)
{
	opts.exe_file = argv[0];
	if (argc < 3)
		print_usage();
	opts.infile = NULL;
	opts.device = NULL;
	opts.outdir = NULL;
	opts.filters = NULL;
	opts.verbose = 0;
}

void				add_filter(filter_t **filters, char *filter)
{
	filter_t		*new_filter;
	int			i;
	int			found = 0;

	for (i = 0 ; protos[i].name; i++)
	{
		if (strcmp(filter, protos[i].name) == 0)
			found = 1;
	}
	if (!found)
	{
		if (opts.verbose)
			printf("Unknown Protocol %s specified as filter.. Ignored\n", filter);
		return ;
	}
	new_filter = (filter_t *)malloc(sizeof(filter_t));
	new_filter->filter = filter;
	new_filter->next_filter = *filters;
	*filters = new_filter;
}

void				parse_options(int argc, char **argv)
{
	char			c = 0;
	init_opts(argc, argv);
	while ((c = getopt(argc, argv, OPTIONS)) != -1)
	{
		switch (c)
		{
		case 'i':
			if (!opts.infile)
				opts.infile = strdup(optarg);
			break;
		case 'l':
			if (!opts.device)
				opts.device = strdup(optarg);
			break;
		case 'd':
			if (!opts.outdir)
				opts.outdir = strdup(optarg);
			break;
		case 'p':
			add_filter(&(opts.filters), strdup(optarg));
			break;
		case 'v':
			opts.verbose = 1;
			break;
		default:
			print_usage();
		}
	}
	if ((!opts.infile) && (!opts.device))
		print_usage();
	if ((opts.infile) && (opts.device))
		print_usage();
}
