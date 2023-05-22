#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define TIMEOUT 10  // 超时时间（秒）

char last_data[1024] = "";  // 上一次传输的数据

// 信号处理函数
void timeout_handler(int sig) {
    printf("客户端超时未传输数据\n");

    // 返回上一次传输的数据
    if (strlen(last_data) > 0) {
        printf("返回上一次传输的数据: %s\n", last_data);
    } else {
        printf("没有上一次传输的数据\n");
    }

    // 重置上一次传输的数据
    memset(last_data, 0, sizeof(last_data));
}

int main() {
    // 设置超时信号处理函数
    signal(SIGALRM, timeout_handler);

    while (1) {
        // 设置定时器
        alarm(TIMEOUT);

        // 等待客户端传输数据
        char data[1024] = "";
        printf("等待客户端传输数据...\n");
        scanf("%s", data);

        // 取消定时器
        alarm(0);

        if (strlen(data) > 0) {
            // 保存本次传输的数据
            strncpy(last_data, data, sizeof(last_data));
            printf("收到客户端传输的数据: %s\n", data);
        } else {
            printf("客户端未传输数据\n");
        }
    }

    return 0;
}