#include "net.h"

URL parse_url(char* remote_url) {
    URL url;
    int i;

    // Copy whole url into struct
    strncpy(url.url, remote_url, 2048);

    // Detect if URL protocol is present
    char* proto_separator = strstr(remote_url, "://");
    if (proto_separator) {
        // Extract protocol specifier
        for (i = 0; (remote_url + i) < proto_separator; i++) {
            url.proto[i] = remote_url[i];
        }
        url.proto[i + 1] = '\0';
    }
    else {
        url.proto[0] = '\0';
    }

    // If there is a protocol separator, trim it off and extract the hostname
    char* protoless_url = remote_url;
    if (proto_separator) {
        protoless_url = (proto_separator + 3); // 3 is length of '://'
    }

    int has_port = 0;
    int has_path = 0;
    for (i = 0; i < strlen(protoless_url) && i < 2048; i++) {
        if (protoless_url[i] == ':') {
            has_port = 1;
            break;
        }
        else if (protoless_url[i] == '/') {
            has_path = 1;
            break;
        }
        else {
            url.hostname[i] = protoless_url[i];
        }
    }
    if (i < 2048)
        url.hostname[i] = '\0';

    char* hostless_url = protoless_url + i + (has_port || has_path);
    char* endptr = hostless_url;
    if (has_port) {
        for (i = 0; i < strlen(hostless_url) && i < 2048; i++) {
            if (!isdigit(hostless_url[i])) {
                has_path = (hostless_url[i] == '/');
                break;
            }
        }

        endptr += i;
        long int lport = strtol(hostless_url, &endptr, 10);
        url.port = (short int) lport & 0xFFFF;
        endptr++;
    }
    else
        url.port = 0;

    if (has_path) {
        strncpy(url.path, endptr, 2048);
    }
    else
        url.path[0] = '\0';

    return url;
}

void unparse_url(URL url, char* url_buf, int buf_len) {
    char proto[2048] = { 0 };
    char port[2048] = { 0 };

    if (*url.proto)
        snprintf(proto, 2048, "%s://", url.proto);
    
    if (url.port)
        snprintf(port, 2048, ":%d", url.port);

    snprintf(url_buf, 2048, "%s%s%s/%s", proto, url.hostname, port, url.path);
}

void sanitize_url(URL* url) {
    if (!url->port)
        url->port = 1965;
    if (!*url->proto)
        strcpy(url->proto, "gemini");

    char url_buf[2048];
    unparse_url(*url, url_buf, 2048);
    strncpy(url->url, url_buf, 2048);
}

int create_socket(BIO* bio_stdout, URL url) {
    struct hostent* host;
    struct sockaddr_in dest;

    if ((host = gethostbyname(url.hostname)) == NULL) {
        BIO_printf(bio_stdout, "Cannot resolve hostname: %s\n", url.hostname);
        abort();
    }

    remote_socket = socket(AF_INET, SOCK_STREAM, 0);
    dest.sin_family = AF_INET;
    dest.sin_port = htons(url.port);
    dest.sin_addr.s_addr = *(long*) host->h_addr;
    memset(&(dest.sin_zero), '\0', 8);

    char* ip_addr = inet_ntoa(dest.sin_addr);
    if (connect(remote_socket, (struct sockaddr*) &dest, sizeof(struct sockaddr)) == -1) {
        BIO_printf("Cannot connect to host %s [%s} on port %d.\n", url.hostname, ip_addr, url.port);
    }

    return remote_socket;
}