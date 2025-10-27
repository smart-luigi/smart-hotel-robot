#if !defined(_SERVICE_IPC_H_)
#define _SERVICE_IPC_H_
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

    // ***************************************************************
    // ipc (inter process communication) message services
    // in contrast to SendMessage the following functions don't crash NT services

    // this is how you get notified about incoming ipc messages
    // you have to write a function which fits to this type definition
    // and then you give it into "CreateIpcQueue"
    // your callback function will then be called for each incoming message
    // CAUTION: each ipc message is handled by a seperate thread, as a result
    //          your callback will be called by a different thread each time
    typedef void(WINAPI* PIPC_CALLBACK_ROUTINE)(
        LPCSTR  ipc_name,
        LPCVOID ipc_context,
        LPCVOID message_buffer,
        DWORD   message_length,
        LPVOID  answer_buffer,
        DWORD   answer_length
        );

    // create an ipc queue
    // please choose a unique ipc name to avoid conflicts with other programs
    // only one ipc queue with the same name can be open at the same time
    // so if 2 programs try to create the same ipc queue, the second call will fail
    // you can specify how many threads may be created to handle incoming messages
    // if the order of the messages is crucial for you, set "max_thread_count" to "1"
    // in its current implementation "max_thread_count" only supports "1" or unlimited
    // the parameter "max_queue_length" is not yet implemented at all
    BOOL WINAPI CreateIpcQueueEx(
        LPCSTR                ipc_name,
        LPCVOID               ipc_context,
        PIPC_CALLBACK_ROUTINE ipc_callback,
#ifdef __cplusplus
        DWORD                 max_thread_count = 16,
        DWORD                 max_queue_length = 0x1000
#else
        DWORD                 max_thread_count,
        DWORD                 max_queue_length
#endif
    );

    BOOL WINAPI CreateIpcQueue(
        LPCSTR                ipc_name,
        LPCVOID               ipc_context,
        PIPC_CALLBACK_ROUTINE ipc_callback
    );

    // send an ipc message to whomever has created the ipc queue (doesn't matter)
    // if you only fill the first 3 parameters, SendIpcMessage returns at once
    // if you fill the next two parameters, too, SendIpcMessage will
    // wait for an answer of the ipc queue owner
    // you can further specify how long you're willing to wait for the answer
    // and whether you want SendIpcMessage to handle messages while waiting
    BOOL WINAPI SendIpcMessage(
        LPCSTR  ipc_name,
        LPCVOID message_buffer,
        DWORD   message_length,
#ifdef __cplusplus
        PVOID   answer_buffer = NULL,
        DWORD   answer_length = 0,
        DWORD   answer_timeout = INFINITE,
        BOOL    handle_message = TRUE
#else
        PVOID   answer_buffer,
        DWORD   answer_length,
        DWORD   answer_timeout
        BOOL    handle_message
#endif
    );

    // destroy the ipc queue again
    // when the queue owning process quits, the ipc queue is automatically deleted
    // only the queue owning process can destroy the queue
    BOOL WINAPI DestroyIpcQueue(
        LPCSTR  ipc_name
    );

#ifdef __cplusplus
   }
#endif

#endif
