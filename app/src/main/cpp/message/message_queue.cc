
#include "message_queue.h"
#include "handler.h"
#include "../util/logUtil.h"


MessageQueue::MessageQueue()
    : queue_name_("")
    , first_(nullptr)
    , last_(nullptr)
    , packet_size_(0)
    , abort_request_(false)
    , lock_()
    , condition_() {
    Init();
}

MessageQueue::MessageQueue(const char* queueNameParam)
    : queue_name_(queueNameParam)
    , first_(nullptr)
    , last_(nullptr)
    , packet_size_(0)
    , abort_request_(false)
    , lock_()
    , condition_() {
    Init();
}

void MessageQueue::Init() {
    pthread_mutex_init(&lock_, nullptr);
    pthread_cond_init(&condition_, nullptr);
    packet_size_ = 0;
    first_ = nullptr;
    last_ = nullptr;
    abort_request_ = false;
}

MessageQueue::~MessageQueue() {
    LOGCATI("%s ~PacketQueue ....", queue_name_);
    Flush();
    pthread_mutex_destroy(&lock_);
    pthread_cond_destroy(&condition_);
}

int MessageQueue::Size() {
    pthread_mutex_lock(&lock_);
    int size = packet_size_;
    pthread_mutex_unlock(&lock_);
    return size;
}

void MessageQueue::Flush() {
    LOGCATI("\n %s Flush .... and this time the queue_ Size is %d \n", queue_name_, Size());
    MessageNode *curNode, *nextNode;
    pthread_mutex_lock(&lock_);
    for (curNode = first_; curNode != nullptr; curNode = nextNode) {
        nextNode = curNode->next;
        auto* msg = curNode->msg;
        if (nullptr != msg) {
            delete msg;
        }
        delete curNode;
        curNode = nullptr;
    }
    last_ = nullptr;
    first_ = nullptr;
    packet_size_ = 0;
    pthread_mutex_unlock(&lock_);
}

int MessageQueue::EnqueueMessage(Message *msg) {
    if (abort_request_) {
        return -1;
    }
    auto *node = new MessageNode();
    node->msg = msg;
    node->next = nullptr;
    pthread_mutex_lock(&lock_);
    if (last_ == nullptr) {
        first_ = node;
    } else {
        last_->next = node;
    }
    last_ = node;
    packet_size_++;
    pthread_cond_signal(&condition_);
    pthread_mutex_unlock(&lock_);
    return 0;
}

/* return < 0 if aborted, 0 if no packet_ and > 0 if packet_.  */
int MessageQueue::DequeueMessage(Message **msg, bool block) {
    MessageNode *node;
    int ret;
    pthread_mutex_lock(&lock_);
    for (;;) {
        if (abort_request_) {
            ret = -1;
            break;
        }
        node = first_;
        if (node) {
            first_ = node->next;
            if (!first_)
                last_ = nullptr;
            packet_size_--;
            *msg = node->msg;
            delete node;
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&condition_, &lock_);
        }
    }
    pthread_mutex_unlock(&lock_);
    return ret;
}

void MessageQueue::Abort() {
    pthread_mutex_lock(&lock_);
    abort_request_ = true;
    pthread_cond_signal(&condition_);
    pthread_mutex_unlock(&lock_);
}


Message::Message() {
    handler_ = nullptr;
    what = -1;
    arg1 = -1;
    arg2 = -1;
    arg3 = -1;
    obj = nullptr;
}

Message::~Message() = default;

int Message::Execute() {
    if (MESSAGE_QUEUE_LOOP_QUIT_FLAG == what) {
        return MESSAGE_QUEUE_LOOP_QUIT_FLAG;
    } else if (handler_) {
        handler_->HandleMessage(this);
        return 1;
    }
    return 0;
}

