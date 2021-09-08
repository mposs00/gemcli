#include <stdio.h>
#include <unistd.h>

#include "net.h"
#include "ssl.h"
#include "protocol.h"
#include "gemtext.h"

int main() {
    init_openssl();
    URL url = parse_url("gemini://gemini.bortzmeyer.org:1965/software/lupa/stats.gmi");
    sanitize_url(&url);
    printf("URL: %s\nProtocol: %s\nHostname: %s\nPort: %d\nPath: %s\n", url.url, url.proto, url.hostname, url.port, url.path);

    gemini_response gre = request(url);
    gemtext_page page = parse_gemtext(gre);
    for (int i = 0; i < page.num_displayed_lines; i++) {
        printf("%s\n", page.displayed_lines[i]);
    }

    //for (int i = 0; i < page.num_links; i++) {
    //    printf("%s\n", page.links[i].ref.url);
    //}

    while (page.num_links) {
        printf("Taking first link (to %s)...\n", page.links[0].ref.url);
        gre = request(page.links[0].ref);
        page = parse_gemtext(gre);
        printf("Response code: %d (%s)\n", gre.code, gre.status_meta);
        for (int i = 0; i < page.num_displayed_lines; i++) {
            printf("%s\n", page.displayed_lines[i]);
        }
        sleep(2);
    }
    //printf("%.*s\n", gre.body_len, gre.body);

    return 0;
}
