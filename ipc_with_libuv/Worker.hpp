#ifndef WORKER_HPP
#define WORKER_HPP

#include "common.hpp"
#include "ChannelSocket.hpp"
#include "ChannelRequest.hpp"
#include <string>

class Worker : public Channel::ChannelSocket::Listener
{
public:
    explicit Worker(Channel::ChannelSocket *channel);
    ~Worker();

private:
    void Close();

public:
    void OnChannelRequest(Channel::ChannelSocket *channel, Channel::ChannelRequest *request) override;
    void OnChannelClosed(Channel::ChannelSocket *channel) override;

private:
    // Passed by argument.
    Channel::ChannelSocket *channel{nullptr};
    // Others.
    bool closed{false};
};

#endif
