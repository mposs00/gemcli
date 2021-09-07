#include <stdio.h>

#include "net.h"
#include "ssl.h"

int main() {
    URL url = parse_url("gemini.circumlunar.space");
    sanitize_url(&url);
    printf("URL: %s\nProtocol: %s\nHostname: %s\nPort: %d\nPath: %s\n", url.url, url.proto, url.hostname, url.port, url.path);

    init_openssl();
    ssl_connect(url);

    char* response;
    int response_len = request_raw(url, &response);
    printf("%.*s\n", response_len, response);

    return 0;
}
