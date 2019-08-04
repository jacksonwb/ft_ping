/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/03 20:13:46 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/03 21:24:43 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

# include <signal.h>
#include "icmp_sock.h"
#include "host.h"
#include "echo.h"
#include "opts.h"
#include "main.h"

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

int ping(t_ping *opts, t_icmp_sock sfd, t_host *host);
int ping__flood(t_ping *opts, t_icmp_sock sfd, t_host *host);

#endif