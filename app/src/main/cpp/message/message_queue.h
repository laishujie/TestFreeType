

//
// Created by wlanjie on 2019/4/13.
//

#ifndef TRINITY_MESSAGE_QUEUE_H
#define TRINITY_MESSAGE_QUEUE_H

#include <pthread.h>
#include "buffer_pool.h"
#define MESSAGE_QUEUE_LOOP_QUIT_FLAG        19900909

class Handler;

class Message {
 public:
    Message();
    ~Message();

    int Execute();
    int GetWhat() {
        return what;
    }
    int GetArg1() {
        return arg1;
    }
    int GetArg2() {
        return arg2;
    }
    int GetArg3() {
        return arg3;
    }
    void* GetObj() {
        return obj;
    }
    Handler* handler_;

    int what;
    int arg1;
    int arg2;
    int arg3;
    int arg4;
    int arg5;
    int arg6;
    void* obj;
};

typedef struct MessageNode {
    Message *msg;
    struct MessageNode *next;
    MessageNode() {
        msg = NULL;
        next = NULL;
    }
} MessageNode;

class MessageQueue {
 private:
    MessageNode* first_;
    MessageNode* last_;
    int packet_size_;
    bool abort_request_;
    pthread_mutex_t lock_;
    pthread_cond_t condition_;
    const char* queue_name_;

 public:
    MessageQueue();
    explicit MessageQueue(const char* queueNameParam);
    ~MessageQueue();

    void Init();
    void Flush();
    int EnqueueMessage(Message *msg);
    int DequeueMessage(Message **msg, bool block);
    int Size();
    void Abort();
};


#endif  // TRINITY_MESSAGE_QUEUE_H
