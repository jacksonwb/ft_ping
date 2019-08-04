/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/03 20:12:50 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/03 21:25:26 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

void sig_int(int s)
{
	(void)s;
	g_loop = 0;
}

void ping__print_header(t_ping *opts, t_host *host, t_echo_req *req)
{
	(void)opts;
	(void)req;
	printf("PING %s (%s) %lu(%lu) bytes of data\n", host->hostname, host->ipstring, sizeof(t_echo_req) - 8, sizeof(t_echo_req) + sizeof(struct ip));
}

void ping__print_res(t_echo_res *res, t_host *host, float time)
{
	printf("%zd bytes from %s (%s): icmp_seq=%u ",  res->size_rec - sizeof(struct ip), host->fqdn, host->ipstring ,res->hdr.icmp_seq);
	if (res->hdr.icmp_type == ICMP_ECHOREPLY)
		printf("ttl=%hhu time=%.3f ms\n", res->ip_hdr.ip_ttl, time);
	else
		printf("Time to live exceeded\n");
}

void ping__print_res_v(t_echo_res *res, t_host *host, float time)
{
	printf("%zd bytes from %s (%s): icmp_seq=%u --icmp_id=%hhu-- ",  res->size_rec - sizeof(struct ip), host->fqdn, host->ipstring ,res->hdr.icmp_seq, res->hdr.icmp_type);
	if (res->hdr.icmp_type == ICMP_ECHOREPLY)
		printf("ttl=%hhu time=%.3f ms\n", res->ip_hdr.ip_ttl, time);
	else
		printf("Time to live exceeded\n");
}

int ping__process_res(t_ping *opts, t_echo_res *res, t_host *host, struct timespec *now, struct timespec *then)
{
	float time;

	if (res->hdr.icmp_id == getpid() && res->hdr.icmp_type == ICMP_ECHOREPLY)
		opts->rec++;
	time = (float)then->tv_sec * 1000 - (float)now->tv_sec * 1000 + (float)then->tv_nsec / 1000000 - (float)now->tv_nsec / 1000000;
	if (opts->rec == 1 || time < opts->min_tm) opts->min_tm = time;
	if (time > opts->max_tm)
		opts->max_tm = time;
	opts->avg_tm = (opts->avg_tm * (opts->rec - 1) + time) / opts->rec;
	if (opts->opts & OPT_V)
		ping__print_res_v(res, host, time);
	else if ((res->hdr.icmp_id == getpid() && res->hdr.icmp_type == ICMP_ECHOREPLY) || res->hdr.icmp_type == ICMP_TIMXCEED)
		ping__print_res(res, host, time);
	return (0);
}

void ping__print_summary(t_ping *opts, t_host *host)
{
	float loss;

	loss = opts->rec ? 100 * (1 - (float)opts->seq / (float)opts->rec) : 100.0;
	printf("\n--- %s ping statistics ---\n", host->hostname);
	printf("%d packets transmitted, %d packets received, %.0f%% packet loss\n", opts->seq, opts->rec, loss);
	if (opts->rec)
		printf("rount-trip min/avg/max = %.3f/%.3f/%.3f ms\n", opts->min_tm, opts->avg_tm, opts->max_tm);
}

int ping__flood(t_ping *opts, t_icmp_sock sfd, t_host *host)
{
	t_echo_req req;

	g_loop = 1;
	signal(SIGINT, sig_int);
	opts->seq = 0;
	if (echo__new(&req))
		return (1);
	echo__set_checksum(&req);
	ping__print_header(opts, host, &req);
	printf("--Flood mode enabled--\nflooding...\n");
	while(g_loop)
	{
		if (icmp_sock__send(&req, sfd, host))
			return(1);
		opts->seq++;
	}
	printf("\n--- %s ping statistics ---\n", host->hostname);
	printf("%d packets transmitted\n", opts->seq);
	return (0);
}

int ping(t_ping *opts, t_icmp_sock sfd, t_host *host)
{
	t_echo_req req;
	t_echo_res res;
	t_host res_host;
	struct timespec now;
	struct timespec then;

	g_loop = 1;
	signal(SIGINT, sig_int);
	opts->seq = 0;
	if (echo__new(&req))
		return (1);
	ping__print_header(opts, host, &req);
	while(g_loop)
	{
		if ((opts->opts & OPT_C) && opts->seq + 1 > opts->count)
			break;
		if (clock_gettime(CLOCK_MONOTONIC, &now))
			return (1);
		echo__set_seq(&req, opts->seq++);
		echo__set_checksum(&req);
		if (icmp_sock__send(&req, sfd, host))
			return(1);
		ft_memset(&res_host, 0, sizeof(res_host));
		if(!icmp_sock__rec(&res, sfd, &res_host))
		{
			clock_gettime(CLOCK_MONOTONIC, &then);
			ping__process_res(opts, &res, &res_host, &now, &then);
			sleep(PING_SLEEP);
		}
		else
			printf("Request timeout for icmp_seq %d\n", opts->seq);
	}
	icmp_sock__close(sfd);
	ping__print_summary(opts, host);
	return (0);
}