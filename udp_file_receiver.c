#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        perror("Invalid arguments");
        return 1;
    }

    int port = atoi(argv[1]);

    int receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (receiver < 0)
    {
        perror("socket() failed");
        return 1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(receiver, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind() error");
        return 1;
    }

    char buf[256];
    int recv_bytes = 0, total_bytes = 0;
    char big_buf[2560] = {0};

    // Nhan ten file
    recv_bytes = recvfrom(receiver, buf, sizeof(buf), 0, NULL, NULL);
    buf[recv_bytes] = '\0';

    char *sender_name = strtok(buf, "_");
    char *file_name = strtok(NULL, "_");

    char new_name[64];
    sprintf(new_name, "%s/new_%s", sender_name, file_name);

    FILE *f = fopen(new_name, "wb");
    if (f == NULL)
    {
        perror("fopen() failed");
        return 1;
    }

    memset(buf, 0, sizeof(buf));

    // Nhan noi dung file
    while ((recv_bytes = recvfrom(receiver, buf, sizeof(buf), 0, NULL, NULL)) > 0)
    {
        if (strcmp(buf, "exit") == 0)
        {
            // Received EOF message, terminate while loop
            break;
        }
        total_bytes += recv_bytes;
        buf[recv_bytes] = '\0';
        strcat(big_buf, buf);
        memset(buf, 0, sizeof(buf));
    }
    fwrite(big_buf, 1, total_bytes, f);

    fclose(f);
    close(receiver);
    return 0;
}