#include "ssl.h"

void init_bio() {
    bio_stdout = BIO_new_fp(stdout, BIO_NOCLOSE);
    bio_stderr = BIO_new_fp(stderr, BIO_NOCLOSE);
    bio_cert = BIO_new(BIO_s_file());
}

void init_openssl() {
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    SSL_load_error_strings();

    init_bio();

    if (SSL_library_init() < 0)
        BIO_printf(bio_stdout, "Failed to initialize OpenSSL!\n");

    method = SSLv23_client_method();

    if ((ctx = SSL_CTX_new(method)) == NULL)
        BIO_printf(bio_stdout, "Failed to initialize OpenSSL context.\n");

    printf("%s\n", OPENSSL_VERSION_TEXT);
    printf("setting min version: %ld\n", SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION));
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
}

void ssl_connect(URL url) {
    ssl = SSL_new(ctx);
    printf("setting ssl min version: %ld\n", SSL_set_max_proto_version(ssl, SSL2_VERSION));

    create_socket(bio_stdout, url);
    if (remote_socket != 0)
        BIO_printf(bio_stdout, "TCP Connected to %s\n", url.hostname);

    SSL_set_fd(ssl, remote_socket);
    int err;
    if ((err = SSL_connect(ssl)) != 1)
        BIO_printf(bio_stdout, "Failed to start SSL connection to %s (error %d)\n", url.hostname, SSL_get_error(ssl, err));
    else
        BIO_printf(bio_stdout, "Started SSL connection to %s\n", url.hostname);

    cert = SSL_get_peer_certificate(ssl);
    if (cert == NULL)
        BIO_printf(bio_stdout, "Failed to get certificate from remote host.\n");
    else {
        BIO_printf(bio_stdout, "Got certificate from remote host.\n");

        certname = X509_NAME_new();
        certname = X509_get_subject_name(cert);

        BIO_printf(bio_stdout, "Certificate CN: ");
        X509_NAME_print_ex(bio_stdout, certname, 0, 0);
        BIO_printf(bio_stdout, "\n");
    }
}

int send_data(char* data, int data_len) {
    int size = SSL_write(ssl, data, data_len);
    if (size < 0) {
        ERR_print_errors(bio_stdout);
    }
    else
        printf("wrote %d bytes\n", size);
    return size;
}

int read_data(char* buffer, int buffer_len) {
    int size = SSL_read(ssl, buffer, buffer_len);
    if (size < 0) {
        ERR_print_errors(bio_stdout);
    }
    else
        printf("read %d bytes\n", size);
    return size;
}

void ssl_cleanup() {
    SSL_shutdown(ssl);
    SSL_free(ssl);
    X509_free(cert);
}