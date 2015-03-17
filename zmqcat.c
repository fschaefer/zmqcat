/*
 * Copyright 2014 Florian Schäfer <florian.schaefer@gmail.com>
 * Copyright 2012 Emiel Mols <emiel@paiq.nl>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of the copyright holders.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include <czmq.h>

#include "config.h"

void
zmqcat_recv(void* socket, int type, FILE *pipe, int verbose)
{
    if (type == ZMQ_PUSH || type == ZMQ_PUB) {
        return;
    }

    char *msg = NULL;
    int msg_len = 0;

    msg = zstr_recv(socket);

    if (msg) {
        msg_len = strlen(msg);

        if (verbose) {
            fprintf(stderr, "receiving %d bytes: %s\n", msg_len, msg);
        }

        fwrite(msg, sizeof(char), msg_len, pipe);
        fflush(pipe);
        free(msg);
    }
}

#define SEND_BUFFER_SIZE 8192

void
zmqcat_send(void* socket, int type, FILE *pipe, int verbose)
{
    if (type == ZMQ_PULL || type == ZMQ_SUB) {
        return;
    }

    char *msg = NULL;
    int msg_len = 0;
    char msg_part[SEND_BUFFER_SIZE] = {0};
    int msg_part_len = 0;

    while (!zctx_interrupted) {
        msg_part_len = fread(msg_part, sizeof(char), SEND_BUFFER_SIZE, pipe);

        if (msg_part_len) {
            msg = realloc(msg, (msg_len + msg_part_len + 1) * sizeof(char));
            memcpy(msg + msg_len, msg_part, msg_part_len);
            msg_len += msg_part_len;
        }

        if (msg_part_len != SEND_BUFFER_SIZE) {
            clearerr(pipe);
            break;
        }
    }

    if (msg_len) {

        msg[msg_len] = '\0';
        zstr_send(socket, "%s", msg);

        if (verbose) {
            fprintf(stderr, "sending %d bytes: %s\n", msg_len, msg);
        }
    }

    free(msg);
}

void
print_usage()
{
    fprintf(stderr, "%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
    fprintf(stderr, "\n");
    fprintf(stderr, "  usage: %s [-b] -e endpoint [-l 20] [-f|-r 1] [-s channel] [-t type] [-v]\n", PACKAGE_NAME);
    fprintf(stderr, "    -b  --bind      : bind instead of connect to endpoint\n");
    fprintf(stderr, "    -e  --endpoint  : endpoint, e.g. \"tcp://127.0.0.1:5000\"\n");
    fprintf(stderr, "    -f  --forever   : alias for --repeat -1\n");
    fprintf(stderr, "    -h  --help      : display this usage information\n");
    fprintf(stderr, "    -l  --linger    : linger period for socket shutdown in ms\n");
    fprintf(stderr, "    -r  --repeat    : repeat send and receive cycle X times (-1 = forever)\n");
    fprintf(stderr, "    -s  --subscribe : subscribe to channel for SUB type socket\n");
    fprintf(stderr, "    -t  --type      : PUSH | PULL | REQ | REP | PUB | SUB\n");
    fprintf(stderr, "    -v  --verbose   : verbose output to stderr\n");
}

int
main(int argc, char *argv[])
{
    int bind = 0;
    char *endpoint = NULL;
    int repeat = 1;
    char *subscribe = "";
    int type = ZMQ_PUSH;
    int linger = 100;
    int verbose = 0;

    zctx_t *ctx;
    void *socket;

    FILE *input = stdin;
    FILE *output = stdout;

    const char* const short_options = "be:fhl:r:s:t:v";
    const struct option long_options[] = {
        { "bind",      0, NULL, 'b' },
        { "endpoint",  1, NULL, 'e' },
        { "forever",   0, NULL, 'f' },
        { "help",      0, NULL, 'h' },
        { "linger",    1, NULL, 'l' },
        { "repeat",    1, NULL, 'r' },
        { "subscribe", 1, NULL, 's' },
        { "type",      1, NULL, 't' },
        { "verbose",   0, NULL, 'v' },
        { NULL,        0, NULL,  0  }
    };

    char c;
    while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (c) {
        case 'b':
            bind = 1;
            break;
        case 'e':
            endpoint = optarg;
            break;
        case 'h':
            print_usage();
            break;
        case 'l':
            linger = atoi(optarg);
            break;
        case 'f':
            repeat = -1;
            break;
        case 'r':
            repeat = atoi(optarg);
            if (repeat == 0) {
                repeat = 1;
            }
            break;
        case 's':
            subscribe = optarg;
            break;
        case 't':
            if (!strcasecmp(optarg, "PULL")) {
                type = ZMQ_PULL;
            }
            else if (!strcasecmp(optarg, "REQ")) {
                type = ZMQ_REQ;
            }
            else if (!strcasecmp(optarg, "REP")) {
                type = ZMQ_REP;
            }
            else if (!strcasecmp(optarg, "PUB")) {
                type = ZMQ_PUB;
            }
            else if (!strcasecmp(optarg, "SUB")) {
                type = ZMQ_SUB;
            }
            break;
        case 'v':
            verbose = 1;
            break;
        default:
            print_usage();
            return 1;
        }
    }

    if (!endpoint) {
        print_usage();
        return 1;
    }

    if ((ctx = zctx_new()) == NULL) {
        fprintf(stderr, "error %d: %s\n", errno, zmq_strerror(errno));
        return 1;
    }

    zctx_set_linger(ctx, linger);

    if ((socket = zsocket_new(ctx, type)) == NULL) {
        fprintf(stderr, "error %d: %s\n", errno, zmq_strerror(errno));
        return 1;
    }

    if (type == ZMQ_SUB) {
        zsocket_set_subscribe(socket, subscribe);
    }

    if (bind) {
        if (zsocket_bind(socket, endpoint) == -1) {
            fprintf(stderr, "error %d: %s\n", errno, zmq_strerror(errno));
            return 1;
        }
    }
    else {
        if (zsocket_connect(socket, endpoint) == -1) {
            fprintf(stderr, "error %d: %s\n", errno, zmq_strerror(errno));
            return 1;
        }
    }

    if (verbose) {
        fprintf(stderr, "%s to %s\n", (bind ? "bound" : "connecting"), endpoint);
    }

    do {
        if (type == ZMQ_REP) {
            zmqcat_recv(socket, type, output, verbose);
            zmqcat_send(socket, type, input, verbose);
        }
        else {
            zmqcat_send(socket, type, input, verbose);
            zmqcat_recv(socket, type, output, verbose);
        }
    } while ((repeat < 0 || --repeat != 0) != 0 && !zctx_interrupted);

    if (ctx && socket) {
        zsocket_destroy(ctx, socket);
    }

    if (ctx) {
        zctx_destroy(&ctx);
    }

    return 0;
}
