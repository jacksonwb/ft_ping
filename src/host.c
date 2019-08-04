/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   host.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/03 20:11:05 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/03 21:14:20 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "host.h"

int host__init(t_host *host, char *hoststring)
{
	struct addrinfo hints;
	struct addrinfo *addrs;
	char hostlookup[NI_MAXHOST + 1];
	int ret;

	ft_memset(&hints, 0, sizeof(hints));
	ft_memset(hostlookup, 0, sizeof(hostlookup));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_CANONNAME;
	if ((ret = getaddrinfo(hoststring, NULL, &hints, &addrs)))
	{
		printf("ft_ping: %s: %s\n", hoststring, gai_strerror(ret));
		return (-1);
	}
	ft_memcpy(&(host->addr), addrs->ai_addr, sizeof(struct sockaddr));
	host->hostname = hoststring;
	freeaddrinfo(addrs);
	if ((ret = getnameinfo((const struct sockaddr *)&(host->addr), sizeof(host->addr), hostlookup, sizeof(hostlookup), NULL, 0, NI_NUMERICHOST)))
	{
		printf("ft_ping: %s: %s\n", hoststring, gai_strerror(ret));
		return (-1);
	}
	host->ipstring = ft_strdup(hostlookup);
	ft_memset(hostlookup, 0, sizeof(hostlookup));
	if ((ret = getnameinfo((const struct sockaddr *)&(host->addr), sizeof(host->addr), hostlookup, sizeof(hostlookup), NULL, 0, NI_NAMEREQD)))
	{
		printf("ft_ping: %s: %s\n", hoststring, gai_strerror(ret));
		return (-1);
	}
	host->fqdn = ft_strdup(hostlookup);
	return (0);
}