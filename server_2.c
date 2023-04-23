#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int main()
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5))
    {
        perror("listen() failed");
    }

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (client == -1)
    {
        perror("accept() failed");
        return 1;
    }

    char buf[256];
    int recv_bytes = 0;
    char big_buf[2560] = {0};
    while ((recv_bytes = recv(client, buf, sizeof(buf), 0)) > 0)
    {
        buf[recv_bytes] = '\0';
        strcat(big_buf, buf);
    }

    char find_str[] = "0123456789";
    char *find_ptr = big_buf;
    int count = 0;
    while ((find_ptr = strstr(find_ptr, find_str)) != NULL)
    {
        count++;
        find_ptr += strlen(find_str);
    }

    printf("The \"%s\" appears %d times\n", find_str, count);
    close(client);
    close(listener);
    return 0;
}