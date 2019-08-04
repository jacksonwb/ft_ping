/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_print.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/04 12:10:11 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/04 12:16:51 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

void	sig_int(int s)
{
	(void)s;
	g_loop = 0;
}

void	ping__print_header(t_ping *opts, t_host *host, t_echo_req *req)
{
	(void)opts;
	(void)req;
	printf("PING %s (%s) %lu(%lu) bytes of data\n", host->hostname,
		host->ipstring, sizeof(t_echo_req) - 8,
		sizeof(t_echo_req) + sizeof(struct ip));
}

void	ping__print_res(t_echo_res *res, t_host *host, float time)
{
	printf("%zd bytes from %s (%s): icmp_seq=%u ", res->size_rec -
		sizeof(struct ip), host->fqdn, host->ipstring, res->hdr.icmp_seq);
	if (res->hdr.icmp_type == ICMP_ECHOREPLY)
		printf("ttl=%hhu time=%.3f ms\n", res->ip_hdr.ip_ttl, time);
	else
		printf("Time to live exceeded\n");
}

void	ping__print_res_v(t_echo_res *res, t_host *host, float time)
{
	printf("%zd bytes from %s (%s): icmp_seq=%u --icmp_id=%hhu-- ",
		res->size_rec - sizeof(struct ip), host->fqdn, host->ipstring,
		res->hdr.icmp_seq, res->hdr.icmp_type);
	if (res->hdr.icmp_type == ICMP_ECHOREPLY)
		printf("ttl=%hhu time=%.3f ms\n", res->ip_hdr.ip_ttl, time);
	else
		printf("Time to live exceeded\n");
}

int		ping__print_summary(t_ping *opts, t_host *host)
{
	float loss;

	loss = opts->rec ? 100 * (1 - (float)opts->seq / (float)opts->rec) : 100.0;
	printf("\n--- %s ping statistics ---\n", host->hostname);
	printf("%d packets transmitted, %d packets received, %.0f%% packet loss\n",
		opts->seq, opts->rec, loss);
	if (opts->rec)
		printf("rount-trip min/avg/max = %.3f/%.3f/%.3f ms\n", opts->min_tm,
			opts->avg_tm, opts->max_tm);
	return (0);
}
