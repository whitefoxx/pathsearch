#include <stdio.h>
#include <uv.h>

#include "UnixStreamSocket.hpp"
#include "ChannelSocket.hpp"
#include "ChannelRequest.hpp"
#include "error.hpp"
#include "Worker.hpp"

void create_worker()
{
    int consumerChannelFd{3};
    int producerChannelFd{4};
    Channel::ChannelSocket *channel = new Channel::ChannelSocket(consumerChannelFd, producerChannelFd);
    Worker worker(channel);
}

int main(int argc, char **argv)
{
    printf("Hello there.\n");
    create_worker();
    // ERROR_STD("print an error message: %s", "fake error");
    return 0;
}