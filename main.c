#include <uv.h>
#include <string.h>
#include "logger.h"

int main(int argc, char **argv)
{
    if(argc < 2) {
        slogf(DEBUG, "Running client\n");
        udp_mcast_cli("239.254.0.1", 5566);
    } else {
        slogf(DEBUG, "Running serv\n");
        udp_mcast_serv("239.254.0.1", 5566);
        int max = atoi(argv[1]);
        for(int i = 0; i < max; ++i)
            udp_mcast_send("Hello", 6);
    }

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
