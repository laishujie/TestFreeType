

#ifndef TRINITY_HANDLER_H
#define TRINITY_HANDLER_H

#include "handler.h"
#include "message_queue.h"

class Handler {
 private:
    MessageQueue* queue_;

 public:
    Handler();
    explicit Handler(MessageQueue* mQueue);
    ~Handler();

    void InitMessageQueue(MessageQueue *queue);
    int PostMessage(Message *msg);
    void FlushMessage();
    int GetQueueSize();
    virtual void HandleMessage(Message *msg){}
};

#endif  // TRINITY_HANDLER_H
