/*
 * tcping.c
 *
 * Copyright (c) 2002-2008 Marc Kirchner <mail(at)marc(dash)kirchner(dot)de>
 *
 * Modified by Yan MA at BUPT to support IPv6 service testing in 2009.
 *
 * tcping is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tcping is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ms++. If not, see <http://www.gnu.org/licenses/>.
 *
 * tcping does a nonblocking connect to test if a port is reachable.
 * Its exit codes are:
 *     -1 an error occured
 *     0  port is open
 *     1  port is closed
 *     2  user timeout
 */

#define VERSION 1.3.5

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#include <string.h>
#endif
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>

void usage();

int main (int argc, char *argv[]) {

    int sockfd;
    struct sockaddr_in addr;
    struct hostent *host;
    struct addrinfo hints, *res0;
    int error = 0;
    int ret;
    socklen_t errlen;
    struct timeval timeout;
    fd_set fdrset, fdwset;
    int verbose=1;
    int c;
    char *cptr;
    long timeout_sec=0, timeout_usec=0;
    int port=0;
    struct timeval start, end;
    struct timezone tz;
    double dstart, dend;
    double ms;

    if (argc < 3)  {
        usage(argv[0]);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;

    while((c = getopt(argc, argv, "qt:u:p:")) != -1) {
        switch(c) {
            case 'q':
                verbose = 0;
                 break;
            case 't':
                cptr = NULL;
                timeout_sec = strtol(optarg, &cptr, 10);
                if (cptr == optarg)
                    usage(argv[0]);
                 break;
            case 'u':
                cptr = NULL;
                timeout_usec = strtol(optarg, &cptr, 10);
                if (cptr == optarg)
                    usage(argv[0]);
                 break;
            case 'p':
                 if(strcmp(optarg, "4") ){
                      hints.ai_family = AF_INET6;
                }
                else if(strcmp(optarg, "6")){
                     hints.ai_family = AF_INET;
                }
                break;
            default:
                usage(argv[0]);
                break;
        }
    }
   
    memset(&addr, 0, sizeof(addr));

    error = getaddrinfo(argv[optind], argv[optind+1], &hints, &res0);
    if (error) {
        fprintf(stderr, "%s %s: %s\n", argv[1], argv[1],
            gai_strerror(error));
        exit(1);
        /*NOTREACHED*/
    }
    memcpy(&hints, res0->ai_addr, res0->ai_addrlen);
    addr.sin_family = hints.ai_family; /*  AF_INET, AF_INET6, AP_UNSPEC */
    if (argv[optind+1]) {
        cptr = NULL;
        port = strtol(argv[optind+1], &cptr, 10);
        if (cptr == argv[optind+1])
            usage(argv[0]);
    } else {
        usage(argv[0]);
    }
    addr.sin_port = htons(port);

    if( res0->ai_addr->sa_family == AF_INET6 ) {
        sockfd = socket (PF_INET6, SOCK_STREAM, 0);
    } else {
        sockfd = socket (PF_INET, SOCK_STREAM, 0);
    }

    if (gettimeofday(&start, &tz) == -1) {
        fprintf(stderr, "gettimeofday");
        exit(1);
    }

    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    if ((ret = connect(sockfd, res0->ai_addr, res0->ai_addrlen)) != 0) {
        if (errno != EINPROGRESS) {
#ifdef HAVE_SOLARIS
            /* solaris immediately returns ECONNREFUSED on local ports */
            if (errno == ECONNREFUSED) {
                if (verbose)
                    fprintf(stdout, "%s port %s closed.\n", argv[optind], argv[optind+1]);
                close(sockfd);
                return(1);
            } else {
#endif   
                if (verbose)
                    fprintf(stderr, "error: %s port %s: %s\n", argv[optind], argv[optind+1], strerror(errno));
                return (-1);
#ifdef HAVE_SOLARIS
            }
#endif   
        }

        FD_ZERO(&fdrset);
        FD_SET(sockfd, &fdrset);
        fdwset = fdrset;

        timeout.tv_sec=timeout_sec + timeout_usec / 1000000;
        timeout.tv_usec=timeout_usec % 1000000;

        if ((ret = select(sockfd+1, &fdrset, &fdwset, NULL, timeout.tv_sec+timeout.tv_usec > 0 ? &timeout : NULL)) == 0) {
            /* timeout */
            close(sockfd);
            if (verbose)
                fprintf(stdout, "%s port %s stetus:timeout\n", argv[optind], argv[optind+1]);
            return(2);
        }
        if (FD_ISSET(sockfd, &fdrset) || FD_ISSET(sockfd, &fdwset)) {
            errlen = sizeof(error);
            if ((ret=getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &errlen)) != 0) {
                /* getsockopt error */
                if (verbose)
                    fprintf(stderr, "error: %s port %s: getsockopt: %s\n", argv[optind], argv[optind+1], strerror(errno));
                close(sockfd);
                return(-1);
            }
            if (error != 0) {
                if (verbose)
                    fprintf(stdout, "%s port %s status:closed\n", argv[optind], argv[optind+1]);
                close(sockfd);
                return(1);
            }
        } else {
            if (verbose)
                fprintf(stderr, "error: select: sockfd not set\n");
            exit(-1);
        }
    }
    /* OK, connection established */
    close(sockfd);
    if (gettimeofday(&end, &tz) == -1) {
        fprintf(stderr, "gettimeofday");
        exit(1);
    }
    dstart = (((double)start.tv_sec) + ((double)start.tv_usec)/1000000.0);
    dend = (((double)end.tv_sec) + ((double)end.tv_usec)/1000000.0);
    ms = (dend - dstart) * 1000.0;
    
    if (verbose)
        fprintf(stdout, "%s port %s status:open rtt:%.1f\n", argv[optind], argv[optind+1], ms);
    return 0;
}

void usage(char *prog) {
    fprintf(stderr, "error: Usage: %s [-q] [-t timeout_sec] [-u timeout_usec] [-p4|6] <host> <port>\n", prog);
        exit(-1);
}
   
