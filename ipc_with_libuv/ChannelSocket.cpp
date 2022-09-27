#include "error.hpp"
#include "ChannelSocket.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>

namespace Channel
{
    /* Static methods for UV callbacks. */
    inline static void onAsync(uv_handle_t *handle)
    {
        while (static_cast<ChannelSocket *>(handle->data)->CallbackRead())
        {
            // Read while there are new messages.
        }
    }

    inline static void onClose(uv_handle_t *handle)
    {
        delete handle;
    }

    // Binary length for a 4194304 bytes payload.
    static constexpr size_t MessageMaxLen{4194308};
    static constexpr size_t PayloadMaxLen{4194304};

    /* Instance methods. */
    ChannelSocket::ChannelSocket(int consumerFd, int producerFd)
        : consumerSocket(new ConsumerSocket(consumerFd, MessageMaxLen, this)),
          producerSocket(new ProducerSocket(producerFd, MessageMaxLen)),
          writeBuffer(static_cast<uint8_t *>(std::malloc(MessageMaxLen)))
    {
    }

    ChannelSocket::ChannelSocket(
        ChannelReadFn channelReadFn,
        ChannelReadCtx channelReadCtx,
        ChannelWriteFn channelWriteFn,
        ChannelWriteCtx channelWriteCtx)
        : channelReadFn(channelReadFn), channelReadCtx(channelReadCtx), channelWriteFn(channelWriteFn),
          channelWriteCtx(channelWriteCtx)
    {
        int err;

        this->uvReadHandle = new uv_async_t;
        this->uvReadHandle->data = static_cast<void *>(this);

        err =
            uv_async_init(uv_default_loop(), this->uvReadHandle, reinterpret_cast<uv_async_cb>(onAsync));

        if (err != 0)
        {
            delete this->uvReadHandle;
            this->uvReadHandle = nullptr;

            THROW_ERROR_STD("uv_async_init() failed: %s", uv_strerror(err));
        }

        err = uv_async_send(this->uvReadHandle);

        if (err != 0)
        {
            delete this->uvReadHandle;
            this->uvReadHandle = nullptr;

            THROW_ERROR_STD("uv_async_send() failed: %s", uv_strerror(err));
        }
    }

    ChannelSocket::~ChannelSocket()
    {
        std::free(this->writeBuffer);

        if (!this->closed)
            Close();

        delete this->consumerSocket;
        delete this->producerSocket;
    }

    void ChannelSocket::Close()
    {
        if (this->closed)
            return;

        this->closed = true;

        if (this->uvReadHandle)
        {
            uv_close(reinterpret_cast<uv_handle_t *>(this->uvReadHandle), static_cast<uv_close_cb>(onClose));
        }

        if (this->consumerSocket)
        {
            this->consumerSocket->Close();
        }

        if (this->producerSocket)
        {
            this->producerSocket->Close();
        }
    }

    void ChannelSocket::SetListener(Listener *listener)
    {
        this->listener = listener;
    }

    void ChannelSocket::Send(std::string message)
    {

        if (this->closed)
            return;

        if (message.length() > PayloadMaxLen)
        {
            ERROR_STD("message too big");

            return;
        }

        SendImpl(
            reinterpret_cast<const uint8_t *>(message.c_str()), static_cast<uint32_t>(message.length()));
    }

    bool ChannelSocket::CallbackRead()
    {
        if (this->closed)
            return false;

        uint8_t *message{nullptr};
        uint32_t messageLen;
        size_t messageCtx;

        auto free = this->channelReadFn(
            &message, &messageLen, &messageCtx, this->uvReadHandle, this->channelReadCtx);

        if (free)
        {
            auto *request = new Channel::ChannelRequest(this, reinterpret_cast<const char *>(message), messageLen);

            // Notify the listener.
            try
            {
                this->listener->OnChannelRequest(this, request);
            }
            catch (const Error &error)
            {
                request->Error(error.what());
            }

            // Delete the Request.
            delete request;

            free(message, messageLen, messageCtx);
        }

        return free != nullptr;
    }

    inline void ChannelSocket::SendImpl(const uint8_t *payload, uint32_t payloadLen)
    {
        // Write using function call if provided.
        if (this->channelWriteFn)
        {
            this->channelWriteFn(payload, payloadLen, this->channelWriteCtx);
        }
        else
        {
            std::memcpy(this->writeBuffer, &payloadLen, sizeof(uint32_t));

            if (payloadLen != 0)
            {
                std::memcpy(this->writeBuffer + sizeof(uint32_t), payload, payloadLen);
            }

            size_t len = sizeof(uint32_t) + payloadLen;

            this->producerSocket->Write(this->writeBuffer, len);
        }
    }

    void ChannelSocket::OnConsumerSocketMessage(ConsumerSocket * /*consumerSocket*/, char *msg, size_t msgLen)
    {
        auto *request = new Channel::ChannelRequest(this, reinterpret_cast<const char *>(msg), msgLen);

        // Notify the listener.
        try
        {
            this->listener->OnChannelRequest(this, request);
        }
        catch (const Error &error)
        {
            request->Error(error.what());
        }

        // Delete the Request.
        delete request;
    }

    void ChannelSocket::OnConsumerSocketClosed(ConsumerSocket * /*consumerSocket*/)
    {
        this->listener->OnChannelClosed(this);
    }

    /* ConsumerSocket methods */
    ConsumerSocket::ConsumerSocket(int fd, size_t bufferSize, Listener *listener)
        : ::UnixStreamSocket(fd, bufferSize, ::UnixStreamSocket::Role::CONSUMER), listener(listener)
    {
    }

    void ConsumerSocket::UserOnUnixStreamRead()
    {
        size_t msgStart{0};

        // Be ready to parse more than a single message in a single chunk.
        while (true)
        {
            if (IsClosed())
                return;

            size_t readLen = this->bufferDataLen - msgStart;

            if (readLen < sizeof(uint32_t))
            {
                // Incomplete data.
                break;
            }

            uint32_t msgLen;
            // Read message length.
            std::memcpy(&msgLen, this->buffer + msgStart, sizeof(uint32_t));

            if (readLen < sizeof(uint32_t) + static_cast<size_t>(msgLen))
            {
                // Incomplete data.
                break;
            }

            this->listener->OnConsumerSocketMessage(
                this,
                reinterpret_cast<char *>(this->buffer + msgStart + sizeof(uint32_t)),
                static_cast<size_t>(msgLen));

            msgStart += sizeof(uint32_t) + static_cast<size_t>(msgLen);
        }

        if (msgStart != 0)
        {
            this->bufferDataLen = this->bufferDataLen - msgStart;

            if (this->bufferDataLen != 0)
            {
                std::memmove(this->buffer, this->buffer + msgStart, this->bufferDataLen);
            }
        }
    }

    void ConsumerSocket::UserOnUnixStreamSocketClosed()
    {
        // Notify the listener.
        this->listener->OnConsumerSocketClosed(this);
    }

    ProducerSocket::ProducerSocket(int fd, size_t bufferSize)
        : ::UnixStreamSocket(fd, bufferSize, ::UnixStreamSocket::Role::PRODUCER)
    {
    }
}