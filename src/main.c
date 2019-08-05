/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/30 21:23:54 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/04 17:25:10 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include "ping.h"
#include "host.h"
#include "icmp_sock.h"
#include "echo.h"
#include "opts.h"
#include "parse.h"

volatile int	g_loop;

void			err_exit(char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}

void			usage(void)
{
	PRINT("./ft_ping [-hvf] -[c count] -[t ttl] host\n");
	exit(EXIT_SUCCESS);
}

int				main(int ac, char **av)
{
	t_icmp_sock	sfd;
	t_ping		opts;
	t_host		host;

	if (ac < 2)
		usage();
	ft_memset(&opts, 0, sizeof(opts));
	parse__args(ac, av, &opts);
	if (opts.opts & OPT_H)
	{
		usage();
		exit(EXIT_SUCCESS);
	}
	sfd = icmp_sock__new(opts.opts & OPT_T ? opts.ttl : PING_TTL);
	if (host__init(&host, av[optind]))
		exit(EXIT_FAILURE);
	return (opts.opts & OPT_F ? ping__flood(&opts, sfd, &host) :
		ping(&opts, sfd, &host));
}
