#include "ChannelRequest.hpp"

namespace Channel
{
    ChannelRequest::ChannelRequest(Channel::ChannelSocket *channel, const char *request, size_t len)
    {
        this->channel = channel;
    }

    ChannelRequest::~ChannelRequest()
    {
    }

    void ChannelRequest::Accept()
    {
    }

    void ChannelRequest::Error(const char *reason) {}

}