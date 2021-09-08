#include "protocol.h"
#include "net.h"

gemini_response parse_response(char* response, int response_len) {
    gemini_response parsed;

    char status_line[1028]; // 2 bytes for status code, 1 byte for whitespace, 1024 bytes for meta, null terminator
    int i = 0;

    while (!(response[i] == '\r' && response[i + 1] == '\n')) {
        status_line[i] = response[i];
        if (++i >= 1027) break;
    }
    status_line[i] = '\0';

    char status_str[3];
    i = 0;
    while (status_line[i] != ' ') {
        status_str[i] = status_line[i];
        if (++i >= 2) break;
    }
    status_str[i] = '\0';
    parsed.code = atoi(status_str);

    strncpy(parsed.status_meta, status_line + strlen(status_str) + 1, 1024);

    parsed.body_len = response_len - strlen(status_line) - 2; // leave one extra space for null terminator
    parsed.body = malloc(parsed.body_len);
    memcpy(parsed.body, response + strlen(status_line) + 2, parsed.body_len);

    return parsed;
}

gemini_response request(URL url) {
    char* response;
    int response_len = request_raw(url, &response);

    gemini_response resp = parse_response(response, response_len);
    resp.url = url;

    free(response);
    return resp;
}
