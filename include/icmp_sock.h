/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_sock.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/03 19:58:42 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/04 10:10:35 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICMP_SOCK_H
# define ICMP_SOCK_H

# include <netinet/ip.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <time.h>
# include "echo.h"
# include "host.h"
# include "main.h"

typedef int	t_icmp_sock;

# define PING_TTL 64
# define PING_TIMEOUT 1
# define PING_SLEEP 1

t_icmp_sock	icmp_sock__new(int ttl);
int			icmp_sock__close(t_icmp_sock sfd);
int			icmp_sock__send(t_echo_req *req, t_icmp_sock sfd, t_host *host);
int			icmp_sock__rec(t_echo_res *res, t_icmp_sock sfd, t_host *host);
#endif
