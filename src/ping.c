/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/03 20:12:50 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/04 12:14:14 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#include "main.h"

int		ping__process_res(t_ping *opts, t_echo_res *res, t_host *host,
	t_stopwatch stopwatch)
{
	float time;

	if (res->hdr.icmp_id == getpid() && res->hdr.icmp_type == ICMP_ECHOREPLY)
		opts->rec++;
	time = (float)stopwatch.then->tv_sec * 1000 -
		(float)stopwatch.now->tv_sec * 1000 +
		(float)stopwatch.then->tv_nsec / 1000000 -
		(float)stopwatch.now->tv_nsec / 1000000;
	if (opts->rec == 1 || time < opts->min_tm)
		opts->min_tm = time;
	if (time > opts->max_tm)
		opts->max_tm = time;
	opts->avg_tm = (opts->avg_tm * (opts->rec - 1) + time) / opts->rec;
	if (opts->opts & OPT_V)
		ping__print_res_v(res, host, time);
	else if ((res->hdr.icmp_id == getpid() && res->hdr.icmp_type ==
		ICMP_ECHOREPLY) || res->hdr.icmp_type == ICMP_TIMXCEED)
		ping__print_res(res, host, time);
	return (0);
}

int		ping__flood(t_ping *opts, t_icmp_sock sfd, t_host *host)
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
	while (g_loop)
	{
		if (icmp_sock__send(&req, sfd, host))
			return (1);
		opts->seq++;
	}
	printf("\n--- %s ping statistics ---\n", host->hostname);
	printf("%d packets transmitted\n", opts->seq);
	return (0);
}

int		ping__init(t_ping *opts, t_echo_req *req, t_host *host)
{
	g_loop = 1;
	signal(SIGINT, sig_int);
	opts->seq = 0;
	if (echo__new(req))
		return (1);
	ping__print_header(opts, host, req);
	return (0);
}

void	ping__proc_h(t_ping *opts, t_echo_res *res, t_host *res_host,
	t_stopwatch stopwatch)
{
	clock_gettime(CLOCK_MONOTONIC, stopwatch.then);
	ping__process_res(opts, res, res_host, stopwatch);
	sleep(PING_SLEEP);
}

int		ping(t_ping *opts, t_icmp_sock sfd, t_host *host)
{
	t_echo_req		req;
	t_echo_res		res;
	t_host			res_host;
	struct timespec	now;
	struct timespec	then;

	if (ping__init(opts, &req, host))
		return (1);
	while (g_loop)
	{
		if ((opts->opts & OPT_C) && opts->seq + 1 > opts->count)
			break ;
		clock_gettime(CLOCK_MONOTONIC, &now);
		echo__set_seq(&req, opts->seq++);
		echo__set_checksum(&req);
		if (icmp_sock__send(&req, sfd, host))
			return (1);
		ft_memset(&res_host, 0, sizeof(res_host));
		if (!icmp_sock__rec(&res, sfd, &res_host))
			ping__proc_h(opts, &res, &res_host, (t_stopwatch){&now, &then});
		else
			printf("Request timeout for icmp_seq %d\n", opts->seq);
	}
	icmp_sock__close(sfd);
	return (ping__print_summary(opts, host));
}
