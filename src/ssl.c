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

    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
    ssl = SSL_new(ctx);
}

void ssl_connect(URL url) {
    create_socket(bio_stdout, url);
    if (remote_socket != 0)
        BIO_printf(bio_stdout, "TCP Connected to %s\n", url.hostname);

    SSL_set_fd(ssl, remote_socket);
    if (SSL_connect(ssl) != 1)
        BIO_printf(bio_stdout, "Failed to start SSL connection to %s\n", url.hostname);
    else
        BIO_printf(bio_stdout, "Started SSL connection to %s\n", url.hostname);

    cert = SSL_get_peer_certificate(ssl);
    if (cert == NULL)
        BIO_printf(bio_stdout, "Failed to get certificate from remote host.\n");
    else
        BIO_printf(bio_stdout, "Got certificate from remote host.\n");

    certname = X509_NAME_new();
    certname = X509_get_subject_name(cert);

    BIO_printf(bio_stdout, "Certificate CN: ");
    X509_NAME_print_ex(bio_stdout, certname, 0, 0);
    BIO_printf(bio_stdout, "\n");
}