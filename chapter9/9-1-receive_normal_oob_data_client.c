//
// Created by linlh on 22-6-20.
//

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <libgen.h>

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

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("connection failed");
    } else {
        printf("send normal data and oob data\n");
        const char * normal_data = "123";
        const char * oob_data = "abc";
        send(sockfd, normal_data, strlen(normal_data), 0);
        send(sockfd, oob_data, strlen(oob_data), MSG_OOB);
        send(sockfd, normal_data, strlen(normal_data), 0);
    }
    close(sockfd);
    return 0;
}