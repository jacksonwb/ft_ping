/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/30 21:23:54 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/03 21:21:13 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include "ping.h"
#include "host.h"
#include "icmp_sock.h"
#include "echo.h"
#include "opts.h"

volatile int g_loop;

void	err_exit(char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}

void	usage(void)
{
	PRINT("./ft_ping [-hv] -[c count] host\n");
	exit(EXIT_SUCCESS);
}

void parse__args(int ac, char **av, t_ping *opts)
{
	int opt;
	long count;
	char *end;

	while ((opt = getopt(ac, av, "vfhc:t:")) != -1)
	{
		if (opt == 'v')
			opts->opts |= OPT_V;
		else if (opt == 'h')
			opts->opts |= OPT_H;
		else if (opt == 'c')
		{
			opts->opts |= OPT_C;
			count = strtol(optarg, &end, 10);
			if (count > INT_MAX || count < 1 || end == optarg)
				die("ft_ping : invalid count");
			opts->count = (int)count;
		}
		else if (opt == 't')
		{
			opts->opts |= OPT_T;
			count = strtol(optarg, &end, 10);
			if (count > 255 || count < 1 || end == optarg)
				die("ft_ping : invalid ttl");
			opts->ttl = (int)count;
		}
		else if (opt == 'f')
			opts->opts |= OPT_F;
		else if (opt == '?')
			exit(EXIT_FAILURE);
	}
}

int main(int ac, char **av)
{
	t_icmp_sock sfd;
	t_ping opts;
	t_host host;

	if (ac < 2)
		usage();
	ft_memset(&opts, 0, sizeof(opts));
	parse__args(ac, av, &opts);
	if(opts.opts & OPT_H)
	{
		usage();
		exit(EXIT_SUCCESS);
	}
	sfd = icmp_sock__new(opts.opts & OPT_T ? opts.ttl : PING_TTL);
	if (host__init(&host, av[optind]))
		exit(EXIT_FAILURE);

	return (opts.opts & OPT_F ? ping__flood(&opts, sfd, &host) : ping(&opts, sfd, &host));
}
