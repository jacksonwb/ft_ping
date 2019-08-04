/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/04 10:41:26 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/04 11:00:02 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

void			parse__set_c(t_ping *opts)
{
	long count;
	char *end;

	opts->opts |= OPT_C;
	count = strtol(optarg, &end, 10);
	if (count > INT_MAX || count < 1 || end == optarg)
		die("ft_ping : invalid count");
	opts->count = (int)count;
}

void			parse__set_t(t_ping *opts)
{
	long count;
	char *end;

	opts->opts |= OPT_T;
	count = strtol(optarg, &end, 10);
	if (count > 255 || count < 1 || end == optarg)
		die("ft_ping : invalid ttl");
	opts->ttl = (int)count;
}

void			parse__args(int ac, char **av, t_ping *opts)
{
	int		opt;

	while ((opt = getopt(ac, av, "vfhc:t:")) != -1)
	{
		if (opt == 'v')
			opts->opts |= OPT_V;
		else if (opt == 'h')
			opts->opts |= OPT_H;
		else if (opt == 'c')
			parse__set_c(opts);
		else if (opt == 't')
			parse__set_t(opts);
		else if (opt == 'f')
			opts->opts |= OPT_F;
		else if (opt == '?')
			exit(EXIT_FAILURE);
	}
}
