/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/30 21:23:54 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/02 20:29:02 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

void	err_exit(char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}

void	usage(void)
{
	PRINT("./ft_ping [-hv] -[c count] host\n");
	exit(EXIT_SUCCESS);
}

void parse__args(int ac, char **av, t_ping *opts)
{
	int opt;
	long count;
	char *end;

	while ((opt = getopt(ac, av, "vhc:t:")) != -1)
	{
		if (opt == 'v')
			opts->opts |= OPT_V;
		else if (opt == 'h')
			opts->opts |= OPT_H;
		else if (opt == 'c')
		{
			opts->opts |= OPT_C;
			count = strtol(optarg, &end, 10);
			if (count > INT_MAX || count < 1 || end == optarg)
				die("ft_ping : invalid count");
			opts->count = (int)count;
		}
		else if (opt == 't')
		{
			opts->opts |= OPT_T;
			count = strtol(optarg, &end, 10);
			if (count > 255 || count < 1 || end == optarg)
				die("ft_ping : invalid ttl");
			opts->ttl = (int)count;
		}
		else if (opt == '?')
			exit(EXIT_FAILURE);
	}
}

t_icmp_sock icmp_sock__new(int ttl)
{
	t_icmp_sock sfd;
	struct timeval tvout;

	if ((sfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
		err_exit("socket");
	if(setsockopt(sfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)))
		err_exit("setsockopt");
	tvout = (struct timeval){
		.tv_sec = PING_TIMEOUT,
		.tv_usec = 0
	};
	if(setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (const void*)&tvout, sizeof(tvout)))
		err_exit("setsockopt");
	return (sfd);
}

int icmp_sock__close(t_icmp_sock sfd)
{
	if (close(sfd) < 0)
		return (-1);
	return (0);
}

int icmp_sock__send(t_echo_req *req, t_icmp_sock sfd, t_host *host)
{
	ssize_t ret;

	ret = sendto(sfd, req, sizeof(t_echo_req), 0, (struct sockaddr*)&(host->addr), sizeof(host->addr));
	if (ret < 0 || ret != sizeof(t_echo_req))
	{
		printf("error: send failed\n");
		return (-1);
	}
	return(0);
}

int icmp_sock__rec(t_echo_res *res, t_icmp_sock sfd, t_host *host)
{
	ssize_t ret;
	struct sockaddr_in address;
	socklen_t addr_len;
	static char buf[INET_ADDRSTRLEN + 1];
	static char name_buf[NI_MAXHOST + 1];

	addr_len = sizeof(address);
	ft_memset(buf, 0, sizeof(buf));
	ft_memset(&address, 0, addr_len);
	if((ret = recvfrom(sfd, res, sizeof(t_echo_res), 0, (struct sockaddr*)&address, &addr_len)) <= 0)
		return (-1);
	res->size_rec = ret;
	host->ipstring = (char*)inet_ntop(AF_INET, (void*)&(&address)->sin_addr, buf, sizeof(buf));
	if(!getnameinfo((const struct sockaddr *)&(address), addr_len, name_buf, sizeof(name_buf), NULL, 0, NI_NAMEREQD))
		host->fqdn = name_buf;
	else
		host->fqdn = host->ipstring;
	return (0);
}

int host__init(t_host *host, char *hoststring)
{
	struct addrinfo hints;
	struct addrinfo *addrs;
	char hostlookup[NI_MAXHOST + 1];
	int ret;

	ft_memset(&hints, 0, sizeof(hints));
	ft_memset(hostlookup, 0, sizeof(hostlookup));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_CANONNAME;
	if ((ret = getaddrinfo(hoststring, NULL, &hints, &addrs)))
	{
		printf("ft_ping: %s: %s\n", hoststring, gai_strerror(ret));
		return (-1);
	}
	ft_memcpy(&(host->addr), addrs->ai_addr, sizeof(struct sockaddr));
	host->hostname = hoststring;
	freeaddrinfo(addrs);
	if ((ret = getnameinfo((const struct sockaddr *)&(host->addr), sizeof(host->addr), hostlookup, sizeof(hostlookup), NULL, 0, NI_NUMERICHOST)))
	{
		printf("ft_ping: %s: %s\n", hoststring, gai_strerror(ret));
		return (-1);
	}
	host->ipstring = ft_strdup(hostlookup);
	ft_memset(hostlookup, 0, sizeof(hostlookup));
	if ((ret = getnameinfo((const struct sockaddr *)&(host->addr), sizeof(host->addr), hostlookup, sizeof(hostlookup), NULL, 0, NI_NAMEREQD)))
	{
		printf("ft_ping: %s: %s\n", hoststring, gai_strerror(ret));
		return (-1);
	}
	host->fqdn = ft_strdup(hostlookup);
	printf("%s\n", host->fqdn);
	return (0);
}

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

u_short cksum(void *vbuf, size_t bufsize) {
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

void sig_int(int s)
{
	(void)s;
	g_loop = 0;
}

void ping__print_header(t_ping *opts, t_host *host, t_echo_req *req)
{
	(void)opts;
	(void)req;
	printf("PING %s (%s) %lu(%lu) bytes of data\n", host->hostname, host->ipstring, sizeof(t_echo_req) - 8, sizeof(t_echo_req) + sizeof(struct ip));
}

void ping__print_res(t_echo_res *res, t_host *host, float time)
{
	printf("%zd bytes from %s (%s): icmp_seq=%u ttl=%hhu time=%.3f ms\n",  res->size_rec - sizeof(struct ip), host->fqdn, host->ipstring ,res->hdr.icmp_seq, res->ip_hdr.ip_ttl, time);
}

int ping__process_res(t_ping *opts, t_echo_res *res, t_host *host, struct timespec *now, struct timespec *then)
{
	float time;

	opts->rec++;
	time = (float)then->tv_sec * 1000 - (float)now->tv_sec * 1000 + (float)then->tv_nsec / 1000000 - (float)now->tv_nsec / 1000000;
	if (opts->rec == 1 || time < opts->min_tm) opts->min_tm = time;
	if (time > opts->max_tm)
		opts->max_tm = time;
	opts->avg_tm = (opts->avg_tm * (opts->rec - 1) + time) / opts->rec;
	ping__print_res(res, host, time);
	return (0);
}

void ping__print_summary(t_ping *opts, t_host *host)
{
	float loss;

	loss = opts->rec ? 100 * (1 - (float)opts->seq / (float)opts->rec) : 100.0;
	printf("\n--- %s ping statistics ---\n", host->hostname);
	printf("%d packets transmitted, %d packets received, %.0f%% packet loss\n", opts->seq, opts->rec, loss);
	printf("rount-trip min/avg/max = %.3f/%.3f/%.3f ms\n", opts->min_tm, opts->avg_tm, opts->max_tm);
}

int ping(t_ping *opts, t_icmp_sock sfd, t_host *host)
{
	t_echo_req req;
	t_echo_res res;
	t_host res_host;
	struct timespec now;
	struct timespec then;

	g_loop = 1;
	signal(SIGINT, sig_int);
	opts->seq = 0;
	if (echo__new(&req))
		return (1);
	echo__set_checksum(&req);
	ping__print_header(opts, host, &req);
	while(g_loop)
	{
		if (clock_gettime(CLOCK_MONOTONIC, &now))
			return (1);
		echo__set_seq(&req, opts->seq++);
		echo__set_checksum(&req);
		if (icmp_sock__send(&req, sfd, host))
			return(1);
		ft_memset(&res_host, 0, sizeof(res_host));
		if(!icmp_sock__rec(&res, sfd, &res_host))
		{
			clock_gettime(CLOCK_MONOTONIC, &then);
			ping__process_res(opts, &res, &res_host, &now, &then);
			sleep(PING_SLEEP);
		}
		else
			printf("Request timeout for icmp_seq %d\n", opts->seq);
	}
	ping__print_summary(opts, host);
	return (0);
}

int main(int ac, char **av)
{
	t_icmp_sock sfd;
	t_ping opts;
	t_host host;

	if (ac < 2)
		usage();
	parse__args(ac, av, &opts);
	if(opts.opts & OPT_H)
	{
		usage();
		exit(EXIT_SUCCESS);
	}
	sfd = icmp_sock__new(opts.opts & OPT_T ? opts.ttl : PING_TTL);

	if (host__init(&host, av[optind]))
		exit(EXIT_FAILURE);

	return (ping(&opts, sfd, &host));
}
