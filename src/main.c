#include <stdio.h>

#include "net.h"
#include "ssl.h"
#include "protocol.h"
#include "gemtext.h"

int main() {
    URL url = parse_url("gemini.circumlunar.space");
    sanitize_url(&url);
    printf("URL: %s\nProtocol: %s\nHostname: %s\nPort: %d\nPath: %s\n", url.url, url.proto, url.hostname, url.port, url.path);

    init_openssl();
    ssl_connect(url);

    gemini_response gre = request(url);
    gemtext_page page = parse_gemtext(gre);
    for (int i = 0; i < page.num_displayed_lines; i++) {
        printf("%s\n", page.displayed_lines[i]);
    }
    for (int i = 0; i < page.num_links; i++) {
        printf("Link %d: %s\n", i + 1, page.links[i].ref.url);
    }
    printf("Taking first link (to %s)...\n", page.links[0].ref.url);
    ssl_connect(page.links[0].ref);
    gre = request(page.links[0].ref);
    page = parse_gemtext(gre);
    for (int i = 0; i < page.num_displayed_lines; i++) {
        printf("%s\n", page.displayed_lines[i]);
    }
    //printf("%.*s\n", gre.body_len, gre.body);

    return 0;
}
