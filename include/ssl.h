#ifndef SSL_H
#define SSL_H

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#include "net.h"

BIO* bio_stdout;
BIO* bio_stderr;
BIO* bio_cert;
const SSL_METHOD* method;
SSL_CTX* ctx;
SSL* ssl;
X509* cert;
X509_NAME* certname;

void init_bio();
void init_openssl();
void ssl_connect(URL url);
int read_data(char* buffer, int buffer_len);
int send_data(char* data, int data_len);

#endif