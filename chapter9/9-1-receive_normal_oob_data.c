//
// Created by linlh on 22-6-20.
//

#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h> // 书上少了这个头文件

int main(int argc, char* argv[]) {
//    if (argc <= 2) {
//        printf("usage: %s ip_address port_number\n", basename(argv[0]));
//        return 1;
//    }
//    const char * ip = argv[1];
//    int port = atoi(argv[2]);

    // 为了测试的方便，就直接在程序中定义了
    const char * ip = "127.0.0.1";
    int port = 10001;

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);
    ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);
    ret = listen(listenfd, 5);
    assert(ret != -1);

    struct sockaddr_in client_address;
    socklen_t client_addr_length = sizeof(client_address);
    int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addr_length);
    if (connfd < 0){
        printf("errno is %d\n", errno);
        close(listenfd);
    }
    char buf[1024];
    fd_set read_fds;
    fd_set exception_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&exception_fds);
    while(1) {
        memset(buf, '\0', sizeof(buf));
        // 调用 select 之前要重置所有连接的信息
        FD_SET(connfd, &read_fds);
        FD_SET(connfd, &exception_fds);
        ret = select(connfd+1, &read_fds, NULL, &exception_fds, NULL);
        if (ret < 0) {
            printf("selection failed\n");
            break;
        }
        // 判断当前连接是否可读了
        if(FD_ISSET(connfd, &read_fds)) {
            ret = recv(connfd, buf, sizeof(buf)-1, 0);
            if (ret <= 0) {
                break;
            }
            printf("get %d bytes of normal data: %s\n", ret, buf);
        } else if (FD_ISSET(connfd, &exception_fds)) {
            ret = recv(connfd, buf, sizeof(buf)-1, MSG_OOB);
            if (ret <= 0) {
                break;
            }
            printf("get %d bytes of oob data:%s\n", ret, buf);
        }
    }
    close(connfd);
    close(listenfd);
    return 0;
}