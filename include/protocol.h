#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "net.h"

typedef enum {
    INPUT = 10,
    SENSITIVE_INPUT = 11,
    SUCCESS = 20,
    REDIRECT_TEMPORARY = 30,
    REDIRECT_PERMANENT = 31,
    TEMPORARY_FAILURE = 40,
    SERVER_UNAVAIL = 41,
    CGI_ERROR = 42,
    PROXY_ERROR = 43,
    SLOW_DOWN = 44,
    PERMANENT_FAILURE = 50,
    NOT_FOUND = 51,
    GONE = 52,
    PROXY_REQUEST_REFUSED = 53,
    BAD_REQUEST = 59,
    CLIENT_CERTIFICATE_REQUIRED = 60,
    CERTIFICATE_NOT_AUTHORIZED = 61,
    CERTIFICATE_NOT_VALID = 62
} response_code;

typedef struct {
    URL url;
    response_code code;
    char status_meta[1024];
    char* body;
    int body_len;
} gemini_response;

gemini_response parse_response(char* response, int response_len);
gemini_response request(URL url);

#endif