#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        perror("Invalid arguments");
        return 1;
    }

    char *file_name = argv[1];
    char *recv_ip = argv[2];
    int recv_port = atoi(argv[3]);
    char *sender_name = argv[4];

    int sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sender < 0)
    {
        perror("socket() failed");
        return 1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(recv_ip);
    addr.sin_port = htons(recv_port);

    // Truyen ten file
    char file_sender[64];
    sprintf(file_sender, "%s_%s", sender_name, file_name);

    if (sendto(sender, file_sender, strlen(file_sender), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("sendto() failed");
        return 1;
    }

    // Truyen noi dung file
    FILE *f = fopen(file_name, "r");
    if (f == NULL)
    {
        perror("fopen() failed");
        return 1;
    }

    char buf[256];
    int read_bytes;
    int count = 0;
    int delay = 100000;

    while ((read_bytes = fread(buf, 1, sizeof(buf), f)) > 0)
    {
        if (sendto(sender, buf, read_bytes, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("sendto() error");
            return 1;
        }
        count++;
        if (count % 10 == 0)
        {
            usleep(delay);
        }
    }

    // Send EOF message
    if (sendto(sender, "exit", strlen("exit"), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("sendto() error");
        return 1;
    }

    fclose(f);
    close(sender);

    return 0;
}