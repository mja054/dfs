#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MSGLENGTH 1024

#define Error(str) do {                         \
                perror (#str);                  \
        } while(0)

int
max (int a, int b)
{
        return a > b ? a : b;
}

int main(int argc, char *argv[])
{
        int fd, readfd, maxfdp1, ret, bytes_left;
        char s[MSGLENGTH], *ptr, r[MSGLENGTH];
        fd_set rset, wset;
        struct sockaddr_in addr;
        FILE *fp;

        if (argc != 2) {
                fprintf (stderr, "usage: %s <name>\n", argv[0]);
                return -1;
        }

        strcpy (s, argv[1]);
        strcat (s, ":");

        bytes_left = MSGLENGTH - strlen (s) - 1;

        // Hey, check this next line, you 'll appreciate when u
        // understand how it's helping us in the code
        ptr = s + strlen (s);

        // Create  new client socket
        fd=socket (AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
                Error ("socket failed");

        addr.sin_family = AF_INET;
        addr.sin_port   = htons (10000);

        //  inet_pton(AF_INET,"127.0.0.1",&(addr.sin_addr));

//      addr.sin_addr.s_addr = inet_addr ("128.97.93.163");
        addr.sin_addr.s_addr = inet_addr ("127.0.0.1");

        if(connect(fd, (struct sockaddr*) &addr, sizeof (addr)) == 0)
                printf ("The connection is accepted\n");

        FD_ZERO (&rset);
        FD_ZERO (&wset);

        // readfd = dup(STDIN_FILENO);
        readfd = fcntl (STDIN_FILENO, F_DUPFD, 0);
        fp = fdopen(readfd, "r+");
        maxfdp1 = max (readfd, fd) + 1;

        for (;;) {
                FD_SET (fd, &rset);
                FD_SET (readfd, &rset);

                printf ("%s:", argv[1]);
                fflush (NULL);
                ret = select (maxfdp1, &rset, NULL, NULL, NULL);
                if (FD_ISSET (fd, &rset)) {
                        do {
                                ret = recv (fd, &r, bytes_left, 0);
                                if (ret == -1) {
                                        perror("recv:");
                                        goto out;
                                } else if (ret == 0) {
                                        printf ("Server closed the read-end of "
                                                "the socket, shutting down.\n");
                                        goto out;
                                }
                                r[ret] = '\0';
                                printf ("\n");
                                write (STDOUT_FILENO, r, ret);
                        } while (ret >= bytes_left);
                }

                //TODO: loop over read to read the complete buffer
                if (FD_ISSET (readfd, &rset)) {
                        do {
                                ret = read (readfd, ptr, bytes_left);
                                ptr[ret] = '\0';
                                if (!strncmp (ptr, "quit", 4)) {
                                        shutdown (fd, SHUT_RDWR);
                                        printf ("shutting down\n");
                                        goto out;
                                }
                                send (fd, s, ret + strlen(argv[1]) + 1, 0);
                        } while (ret >= bytes_left);
                        printf ("\n");
                }
        }
out:
        return 0;
}
