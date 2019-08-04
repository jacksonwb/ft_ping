/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/03 20:13:46 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/04 12:16:45 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

# include <signal.h>
# include "icmp_sock.h"
# include "host.h"
# include "echo.h"
# include "opts.h"

typedef	struct	s_ping
{
	int			opts;
	int			count;
	int			ttl;
	int			seq;
	int			rec;
	float		min_tm;
	float		max_tm;
	float		avg_tm;
}				t_ping;

typedef struct	s_stopwatch
{
	struct timespec	*now;
	struct timespec *then;
}				t_stopwatch;

int				ping(t_ping *opts, t_icmp_sock sfd, t_host *host);
int				ping__flood(t_ping *opts, t_icmp_sock sfd, t_host *host);
void			sig_int(int s);
void			ping__print_header(t_ping *opts, t_host *host, t_echo_req *req);
void			ping__print_res(t_echo_res *res, t_host *host, float time);
void			ping__print_res_v(t_echo_res *res, t_host *host, float time);
int				ping__print_summary(t_ping *opts, t_host *host);

#endif
