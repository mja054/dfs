#include <stdio.h>
#include <sys/time.h> 
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/select.h>

#define Error(str) do {                         \
                perror (#str);                  \
        } while(0)

int
max (int a, int b)
{
        return a > b ? a : b;
}

// the global structure that stores the list of clients
// TODO: maintain it as a linked list, so that we can
//  * client disconnects
//  * unlimited client connections(what a lie, select supports only
//    supports 1024 fd, wtf :) )

// for now we are supporting 20 clients

int client_cnt = 0;

struct client_ls {
        int fd;
};
struct client_ls cl[20];
        
pthread_mutex_t mutex;

void
_write(int fd, char *s, int len)
{
        int i, ret;

        for (i = 0; i < 20; i++) {
                if (cl[i].fd != -1 && cl[i].fd != fd) {
                        ret = send (cl[i].fd, s, len, 0);
                }
        }
}
                
void *
chat (void *data)
{
        char s[1024];
        fd_set rset;
        int t, ret, i;
        struct timeval tm;

        tm.tv_sec = 2;
        tm.tv_usec = 0;

        FD_ZERO (&rset);

        for (;;) {
                // wait for clients to connect
                for (;!client_cnt && sleep (2);)
                        ;

                // to be thread safe, create a new list from the existing
                // one and perform operations on that. this is easy and
                // good programming.
                t = 0;
                for (i = 0; i < 20; i++)
                        if (cl[i].fd != -1) {
                                FD_SET (cl[i].fd, &rset);
                                t = max (cl[i].fd, t);
                        }

                ret = select (t + 1, &rset, NULL, NULL, &tm);
                if (ret == -1)
                        continue;

                for (i = 0; i < 20; i++) {
                        if (cl[i].fd != -1)
                                if (FD_ISSET (cl[i].fd, &rset)) {
                                        ret = recv (cl[i].fd, s, 1023, 0);
                                        s[ret] = '\0';
                                        write (stdout, s, ret);
                                        _write (cl[i].fd, s, ret);
                                }
                }
        }
}

int main()
{
        int fd1, fd2, len, ret, i;
        char s[1024];
        struct sockaddr_in addr, addr1;
        pthread_t th;

        fd1 = socket (AF_INET, SOCK_STREAM, 0);
        if (fd1 == -1)
                Error ("socket failed");
  
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port        = htons (10000);
 
        if(bind (fd1, (struct sockaddr*) &addr, sizeof (addr)) == -1)
                Error ("bind failed");

        printf("\nbinding socket successful");
 
        if(listen (fd1, 3) < 0)
                Error ("listen failed");

        len = sizeof(struct sockaddr);

        for (i = 0; i < 20; i++)
                cl[i].fd = -1;

        // Create a thread
        pthread_create (&th, NULL, chat, NULL);

        for (;;) {
                fd2 = accept(fd1, (struct sockaddr*) &addr1, &len);
                if(fd2 == -1)
                        Error ("accept failed");

                cl[client_cnt].fd = fd2;
                client_cnt ++;

                printf("The client is connected\n");
        }

        return 0;
}
