/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/30 20:16:05 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/02 20:30:20 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

# include <unistd.h>
# include <stdio.h>
# include <signal.h>
# include <sys/socket.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <limits.h>
# include <time.h>
# include "libft.h"

# define OPT_V 1
# define OPT_H 2
# define OPT_C 4
# define OPT_T 8

volatile int g_loop;

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

# define PING_TTL 64
# define PING_TIMEOUT 1
# define PING_SLEEP 1

typedef int t_icmp_sock;

typedef struct			s_host
{
	char				*hostname;
	char				*ipstring;
	char				*fqdn;
	struct sockaddr_in	addr;
}						t_host;

# define PKT_SIZE 64

typedef struct 	s_echo_req
{
	struct icmp hdr;
	char msg[PKT_SIZE - sizeof(struct icmp)];
}				t_echo_req;

typedef struct	s_echo_res
{
	struct ip ip_hdr;
	struct icmp hdr;
	char msg[PKT_SIZE - sizeof(struct icmp)];
	ssize_t	size_rec;
}				t_echo_res;


# endif