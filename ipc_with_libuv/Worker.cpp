#include "Worker.hpp"

Worker::Worker(::Channel::ChannelSocket *channel)
    : channel(channel)
{
    // Set us as Channel's listener.
    this->channel->SetListener(this);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

Worker::~Worker()
{
    if (!this->closed)
        Close();
}

void Worker::Close()
{
    if (this->closed)
        return;

    this->closed = true;

    // Close the Channel.
    this->channel->Close();
}

inline void Worker::OnChannelRequest(Channel::ChannelSocket * /*channel*/, Channel::ChannelRequest *request)
{
}

inline void Worker::OnChannelClosed(Channel::ChannelSocket * /*socket*/)
{
    Close();
}