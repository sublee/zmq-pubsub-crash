#include "zmq.h"
#include <stdio.h>

void poll(void *socket)
{
    zmq_pollitem_t items[1];
    items[0].socket = socket;
    items[0].events = ZMQ_POLLIN;
    zmq_poll(items, 1, 1);
}

int main()
{
    printf("%d.%d.%d\n", ZMQ_VERSION_MAJOR, ZMQ_VERSION_MINOR, ZMQ_VERSION_PATCH);

    int hwm = 1;
    int n = 10000;
    int i, j;

    char addr[256]; size_t addr_len = 256;
    char opt[256];  size_t opt_len  = 256;
    char topic[9];

    void *context = zmq_ctx_new();
    void *pub, *sub;

    pub = zmq_socket(context, ZMQ_PUB);
    zmq_setsockopt(pub, ZMQ_RCVHWM, &hwm, sizeof(hwm));

    for (i = 0; i < 100; ++i)
    {
        sub = zmq_socket(context, ZMQ_SUB);
        zmq_setsockopt(sub, ZMQ_SNDHWM, &hwm, sizeof(hwm));

        zmq_bind(sub, "tcp://127.0.0.1:*");
        zmq_getsockopt(sub, ZMQ_LAST_ENDPOINT, addr, &addr_len);

        for (j = 0; j < n; ++j)
        {
            sprintf(topic, "%08x", j);
            zmq_setsockopt(sub, ZMQ_SUBSCRIBE, &topic, 8);
            zmq_getsockopt(sub, ZMQ_EVENTS, opt, &opt_len);
        }

        zmq_connect(pub, addr);
        poll(sub);

        for (j = 0; j < n; ++j)
        {
            sprintf(topic, "%08x", j);
            zmq_setsockopt(sub, ZMQ_UNSUBSCRIBE, &topic, 8);
            zmq_getsockopt(sub, ZMQ_EVENTS, opt, &opt_len);
        }
    }
}
