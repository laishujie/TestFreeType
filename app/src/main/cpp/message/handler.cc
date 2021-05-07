
#include "handler.h"


Handler::Handler() : queue_(nullptr) {
}

Handler::Handler(MessageQueue* queue) {
    this->queue_ = queue;
}

Handler::~Handler() = default;

void Handler::InitMessageQueue(MessageQueue *queue) {
    this->queue_ = queue;
}

int Handler::PostMessage(Message *msg) {
    msg->handler_ = this;
    if (nullptr == queue_) {
        return 0;
    }
    return queue_->EnqueueMessage(msg);
}

void Handler::FlushMessage() {
    if (nullptr == queue_) {
        return;
    }
    queue_->Flush();
}

int Handler::GetQueueSize() {
    if (nullptr == queue_) {
        return 0;
    }
    return queue_->Size();
}

