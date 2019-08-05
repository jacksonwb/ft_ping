/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_sock.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/03 19:57:42 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/04 18:49:06 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "icmp_sock.h"

t_icmp_sock	icmp_sock__new(int ttl)
{
	t_icmp_sock		sfd;
	struct timeval	tvout;

	if ((sfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
		err_exit("socket");
	if (setsockopt(sfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)))
		err_exit("setsockopt");
	tvout = (struct timeval){
		.tv_sec = PING_TIMEOUT,
		.tv_usec = 0
	};
	if (setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (const void*)&tvout,
		sizeof(tvout)))
		err_exit("setsockopt");
	return (sfd);
}

int			icmp_sock__close(t_icmp_sock sfd)
{
	if (close(sfd) < 0)
		return (-1);
	return (0);
}

int			icmp_sock__send(t_echo_req *req, t_icmp_sock sfd, t_host *host)
{
	ssize_t ret;

	ret = sendto(sfd, req, sizeof(t_echo_req), 0,
		(struct sockaddr*)&(host->addr), sizeof(host->addr));
	if (ret < 0 || ret != sizeof(t_echo_req))
	{
		printf("error: send failed\n");
		return (-1);
	}
	return (0);
}

int			icmp_sock__rec(t_echo_res *res, t_icmp_sock sfd, t_host *host)
{
	ssize_t				ret;
	struct sockaddr_in	address;
	socklen_t			addr_len;
	static char			buf[INET_ADDRSTRLEN + 1];
	static char			name_buf[NI_MAXHOST + 1];

	addr_len = sizeof(address);
	ft_memset(buf, 0, sizeof(buf));
	ft_memset(&address, 0, addr_len);
	if ((ret = recvfrom(sfd, res, sizeof(t_echo_res), 0,
		(struct sockaddr*)&address, &addr_len)) <= 0)
		return (-1);
	res->size_rec = ret;
	host->ipstring = (char*)inet_ntop(AF_INET, (void*)&(&address)->sin_addr,
		buf, sizeof(buf));
	if (!getnameinfo((const struct sockaddr *)&(address), addr_len, name_buf,
		sizeof(name_buf), NULL, 0, NI_NAMEREQD))
		host->fqdn = name_buf;
	else
		host->fqdn = host->ipstring;
	return (0);
}
