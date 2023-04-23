#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

int main()
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("connect() failed");
        return 1;
    }

    FILE *f = fopen("client_2.txt", "r");
    if (f == NULL)
    {
        perror("fopen() failed");
        return 1;
    }

    char buf[256];
    int send_bytes;

    while ((send_bytes = fread(buf, 1, sizeof(buf), f)) > 0)
    {
        if (send(client, buf, send_bytes, 0) < 0)
        {
            perror("send() failed");
            return 1;
        }
    }

    fclose(f);
    close(client);

    return 0;
}