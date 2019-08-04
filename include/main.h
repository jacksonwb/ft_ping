/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/30 20:16:05 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/04 10:58:14 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

# include <unistd.h>
# include <limits.h>
# include <stdio.h>
# include "libft.h"

extern volatile int	g_loop;

void				err_exit(char *str);

#endif
