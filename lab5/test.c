#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* thread_function(void* arg)
{
    
    return NULL;
}

int main()
{
    pthread_t thread;
    int result;

    // 创建线程
    result = pthread_create(&thread, NULL, thread_function, NULL);
    if (result != 0) {
        perror("pthread_create");
        return 1;
    }

    // 等待线程执行完成或超时
    sleep(1); // 等待 1 秒钟

    // 取消线程
    result = pthread_cancel(thread);
    if (result != 0) {
        perror("pthread_cancel");
        return 1;
    }

    // 等待线程结束
    result = pthread_join(thread, NULL);
    if (result != 0) {
        perror("pthread_join");
        return 1;
    }

    return 0;
}