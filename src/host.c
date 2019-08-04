/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   host.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/03 20:11:05 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/04 10:32:08 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "host.h"

int	host__set_ipstring(t_host *host, char *hoststring)
{
	int		ret;
	char	hostlookup[NI_MAXHOST + 1];

	ft_memset(hostlookup, 0, sizeof(hostlookup));
	if ((ret = getnameinfo((const struct sockaddr *)&(host->addr),
		sizeof(host->addr), hostlookup, sizeof(hostlookup), NULL, 0,
		NI_NUMERICHOST)))
	{
		printf("ft_ping: %s: %s\n", hoststring, gai_strerror(ret));
		return (-1);
	}
	host->ipstring = ft_strdup(hostlookup);
	return (0);
}

int	host__set_fqdn(t_host *host, char *hoststring)
{
	int		ret;
	char	hostlookup[NI_MAXHOST + 1];

	ft_memset(hostlookup, 0, sizeof(hostlookup));
	if ((ret = getnameinfo((const struct sockaddr *)&(host->addr),
		sizeof(host->addr), hostlookup, sizeof(hostlookup), NULL, 0,
		NI_NAMEREQD)))
	{
		printf("ft_ping: %s: %s\n", hoststring, gai_strerror(ret));
		return (-1);
	}
	host->fqdn = ft_strdup(hostlookup);
	return (0);
}

int	host__init(t_host *host, char *hoststring)
{
	struct addrinfo	hints;
	struct addrinfo	*addrs;
	int				ret;

	ft_memset(&hints, 0, sizeof(hints));
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
	if (host__set_ipstring(host, hoststring) < 0)
		return (-1);
	if (host__set_fqdn(host, hoststring) < 0)
		return (-1);
	return (0);
}
