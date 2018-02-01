#include <uv.h>
#include <assert.h>
#include "buffer.h"

static uv_udp_t udp_sock;
static struct sockaddr_in mcast_addr;
static int actual_read_num = 0;

static void on_udp_recv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
    if(nread == 0) return;

    if(flags & UV_UDP_PARTIAL) {
        slogf(WARN, "Drop partial\n");
        return;
    }

    actual_read_num++;

    if(actual_read_num % 1000 == 0) {
        slogf(DEBUG, "actual read num = %d\n", actual_read_num);
    }
}

/* host == multicast group */
int udp_mcast_cli(const char* host, int port)
{
    const char* bind_host = "0.0.0.0";
    struct sockaddr_in bind_addr;
    uv_udp_init(uv_default_loop(), &udp_sock);
    assert(0 == uv_ip4_addr(bind_host, port, &bind_addr));
    assert(0 == uv_udp_bind(&udp_sock, &bind_addr, 0));
    /* bind before any operation so that we got a valid sock fd. */

    assert(0 == uv_udp_set_membership(&udp_sock, host, "0.0.0.0", UV_JOIN_GROUP));

    udp_sock.data = buffer_init();

    int rc = uv_udp_recv_start(&udp_sock, buffer_alloc, on_udp_recv);
    if(rc < 0)
    {
        slogf(ERR, "udp_recv_start = %s\n", uv_strerror(rc));
        return 1;
    }

    return 0;
}

/* */
int udp_mcast_send(char* buf, int len)
{
    uv_buf_t bufs = uv_buf_init(buf, len);
    int rc = uv_udp_try_send(&udp_sock, &bufs, 1, &mcast_addr);
    if(rc < 0)
    {
        slogf(ERR, "udp_try_send = %s\n", uv_strerror(rc));
        return 1;
    }

    return 0;
}

/* for server, we are going to send packet to a specific address which is multicast addres. */
int udp_mcast_serv(const char *host, int port)
{
    uv_udp_init(uv_default_loop(), &udp_sock);
    assert(0 == uv_ip4_addr(host, port, &mcast_addr));
}
