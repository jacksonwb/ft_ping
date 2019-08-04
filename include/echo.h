/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/03 20:04:33 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/03 21:27:01 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ECHO_H
# define ECHO_H

# include <limits.h>
# include <unistd.h>
# include <stdint.h>
# include <netinet/ip_icmp.h>
# include <netinet/ip.h>
# include "libft.h"

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

int echo__new(t_echo_req *req);
int echo__set_seq(t_echo_req *req, int seq);
void echo__set_checksum(t_echo_req *req);

#endif
