#ifndef NET_H
#define NET_H

#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <openssl/bio.h>

typedef struct {
    char url[2048];
    char hostname[2048];
    char proto[2048];
    char path[2048];
    short int port;
} URL;

int remote_socket;

URL parse_url(char* remote_url);
void unparse_url(URL url, char* buf, int buflen);
void sanitize_url(URL* url);
int create_socket(BIO* bio_stdout, URL url);
int request_raw(URL url, char** out_buffer);

#endif