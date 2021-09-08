#ifndef GEMTEXT_H
#define GEMTEXT_H

#include "net.h"
#include "data.h"
#include "protocol.h"

typedef struct {
    int index;
    char text[2048];
    URL ref;
} gemtext_link;

typedef struct {
    gemtext_link* links;
    char** displayed_lines;
    int num_links;
    int num_displayed_lines;
} gemtext_page;

gemtext_link parse_link(char* line, int index, URL page_url);
gemtext_page parse_gemtext(gemini_response gresp);

#endif