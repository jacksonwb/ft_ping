/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/30 21:23:54 by jbeall            #+#    #+#             */
/*   Updated: 2019/08/01 21:49:18 by jbeall           ###   ########.fr       */
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
	PRINT("./ft_ping [-huv] host\n");
	exit(EXIT_SUCCESS);
}

void parse_args(int ac, char **av, t_ping *opts)
{
	int opt;
	long count;
	char *end;

	while ((opt = getopt(ac, av, "vhc:")) != -1)
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

int icmp_socket__send(t_echo_req *req, t_icmp_sock sfd, t_host *host)
{
	ssize_t ret;

	ret = sendto(sfd, req, sizeof(t_echo_req), 0, (struct sockaddr*)&(host->addr), sizeof(host->addr));
	if (ret < 0 || ret != sizeof(t_echo_req))
		return (-1);
	return(0);
}

int host__init(t_host *host, char *hoststring)
{
	struct addrinfo hints;
	struct addrinfo *addrs;
	char hostlookup[NI_MAXHOST];
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

	if ((ret = getnameinfo((const struct sockaddr *)&(host->addr), sizeof(host->addr), hostlookup, sizeof(hostlookup), NULL, 0, NI_NUMERICHOST)))
	{
		printf("ft_ping: %s: %s\n", hoststring, gai_strerror(ret));
		return (-1);
	}
	host->ipstring = ft_strdup(hostlookup);
	printf("%s\n", host->ipstring);
	return (0);
}

int echo__new(t_echo_req *req)
{
	ft_memset(req, 0, sizeof(t_echo_req));
	req->hdr = (struct icmp){
		.icmp_type = 0,
		.icmp_cksum = 0,
		.icmp_id = htonl(getpid())
	};
	ft_memset(req->msg, 0, sizeof(req->msg));
	return (0);
}

int echo__set_seq(t_echo_req *req, int seq)
{
	if (seq > SHRT_MAX)
		return (-1);
	req->hdr.icmp_seq = htons((short)seq);
	return (0);
}

void echo__set_checksum(t_echo_req *req)
{
	register long sum;
	int count;
	void *addr;

	req->hdr.icmp_cksum = 0;
	count = sizeof(req->hdr);
	addr = (void*)(&(req->hdr));
	sum = 0;
	while( count > 1 )  {
			sum += *(unsigned short*)addr++;
			count -= 2;
	}
	if( count > 0 )
			sum += * (unsigned char *) addr;
	while (sum>>16)
		sum = (sum & 0xffff) + (sum >> 16);
	req->hdr.icmp_cksum = ~sum;
}


void sig_int(int s)
{
	(void)s;
	printf("Recieved SIGINT\n");
	g_loop = 0;
}


int ping(t_ping *opts, t_icmp_sock sfd, t_host *host)
{
	t_echo_req req;
	struct timespec now;

	(void)opts;
	(void)sfd;
	(void)host;

	g_loop = 1;
	signal(SIGINT, sig_int);
	opts->seq = 0;
	if (echo__new(&req))
		return (-1);
	if (echo__set_seq(&req, 0))
		return (-1);
	echo__set_checksum(&req);
	while(g_loop)
	{
		if (clock_gettime(CLOCK_MONOTONIC, &now))
			return (-1);
	}
	return (0);
}

int main(int ac, char **av)
{
	t_icmp_sock sfd;
	t_ping opts;
	t_host host;

	if (ac < 2)
		usage();
	parse_args(ac, av, &opts);
	sfd = icmp_sock__new(PING_TTL);

	if (host__init(&host, av[optind]))
		exit(EXIT_FAILURE);

	return (ping(&opts, sfd, &host));
}
