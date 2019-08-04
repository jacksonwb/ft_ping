/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   host.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/03 20:11:13 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/04 10:09:45 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HOST_H
# define HOST_H

# include <stdlib.h>
# include <netinet/ip.h>
# include <netdb.h>
# include <stdio.h>
# include "libft.h"

typedef struct			s_host
{
	char				*hostname;
	char				*ipstring;
	char				*fqdn;
	struct sockaddr_in	addr;
}						t_host;

int						host__init(t_host *host, char *hoststring);

#endif
