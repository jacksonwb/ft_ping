/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/30 21:23:54 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/31 21:40:00 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

void	err_exit(char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}

void parse_args(int ac, char **av, t_ping *opts)
{
	int opt;
	long count;
	char *end;

	while ((opt = getopt(ac, av, "vhc:")) != -1)
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
		else if (opt == '?')
			exit(EXIT_FAILURE);
	}
}

int main(int ac, char **av)
{
	int sfd;
	t_ping opts;

	parse_args(ac, av, &opts);
	if ((sfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
		err_exit("socket");
	printf("Success!\n");

}
