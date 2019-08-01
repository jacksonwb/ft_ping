/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/30 20:16:05 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/31 19:10:28 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

# include <unistd.h>
# include <stdio.h>
# include <sys/socket.h>
# include <netinet/ip.h>
# include <arpa/inet.h>
# include <limits.h>
# include "libft.h"

# define OPT_V 1
# define OPT_H 2
# define OPT_C 4

typedef	struct	s_ping
{
	int			opts;
	int			count;
}				t_ping;

# endif