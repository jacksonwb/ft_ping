/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/03 20:05:30 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/03 21:27:17 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "echo.h"

int echo__new(t_echo_req *req)
{
	ft_memset(req, 0, sizeof(t_echo_req));
	req->hdr.icmp_type = ICMP_ECHO;
	req->hdr.icmp_id = getpid();
	ft_memset(req->msg, 0, sizeof(req->msg));
	ft_memcpy(req->msg, "PING!", ft_strlen("PING!"));
	return (0);
}

int echo__set_seq(t_echo_req *req, int seq)
{
	if (seq > SHRT_MAX)
		return (-1);
	req->hdr.icmp_seq = seq;
	return (0);
}

uint16_t cksum(void *vbuf, size_t bufsize) {
    uint16_t *data = (uint16_t*)vbuf;
    uint32_t sum = 0;
    size_t limit = bufsize / 2;
    char *buf = (char*)vbuf;
    for (size_t i = 0; i < limit; i++) {
        sum += ntohs(data[i]);
        if (sum > UINT16_MAX) {
            sum -= UINT16_MAX;
        }
    }
    if (bufsize & 1) {
        sum += ntohs((uint8_t)(buf[bufsize-1]));
        if (sum > UINT16_MAX) {
            sum -= UINT16_MAX;
        }
    }
    return htons(~sum);
}

void echo__set_checksum(t_echo_req *req)
{
	req->hdr.icmp_cksum = 0;
	req->hdr.icmp_cksum = cksum(req, sizeof(t_echo_req));
}