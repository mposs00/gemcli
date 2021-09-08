#include "gemtext.h"

gemtext_page parse_gemtext(gemini_response gresp) {
    if (gresp.code == NO_RESPONSE || gresp.body_len == 0) {
        gemtext_page empty_page;
        empty_page.num_links = 0;
        empty_page.num_displayed_lines = 0;
        return empty_page;
    }

    char* nullterm = malloc(gresp.body_len + 1);
    memcpy(nullterm, gresp.body, gresp.body_len);
    nullterm[gresp.body_len] = '\0';

    int link_count = 0;
    int printable_line_count = 0;
    int total_lines = 0;

    // First iteration to count links and printable lines
    char* line = strtok(nullterm, "\n");
    while (line != NULL) {
        // Awful bad horrible goto hell
        // Please rewrite this
        total_lines++;

        if (strlen(line) < 2) {
            printable_line_count++;
            goto next_tok;
        }

        if (line[0] == '=' && line[1] == '>')
            link_count++;
        
        if (strlen(line) > 2 && line[0] == '`' && line[1] == '`' && line[2] == '`')
            goto next_tok;

        printable_line_count++;

        next_tok:
            line = strtok(NULL, "\n");
            continue;
    }

    // Allocate space in output
    gemtext_page page;
    page.displayed_lines = malloc(printable_line_count * sizeof(char*));
    page.links = malloc(link_count * sizeof(gemtext_link));

    memcpy(nullterm, gresp.body, gresp.body_len);
    nullterm[gresp.body_len] = '\0';

    // Second iteration to pull data into allocated struct
    int current_link = 0;
    int current_line = 0;
    char* saveptr;
    line = strtok_r(nullterm, "\n", &saveptr);
    while (line != NULL) {
        int print_line = 0;
        if (strlen(line) < 2)
            print_line = 1;
        else if (strlen(line) > 2 && line[0] == '`' && line[1] == '`' && line[2] == '`')
            print_line = 0;
        else
            print_line = 1;

        if (print_line) {
            if (line[0] == '=' && line[1] == '>') {
                gemtext_link link = parse_link(line, current_link, gresp.url);
                memcpy((page.links + current_link), &link, sizeof(gemtext_link));
                char idx_str[512];
                snprintf(idx_str, 512, "%d. ", link.index + 1);
                int line_len = strlen(link.text) + strlen(idx_str);
                page.displayed_lines[current_line] = malloc(line_len + 1);
                snprintf(page.displayed_lines[current_line], line_len + 1, "%s%s", idx_str, link.text);
                current_link++;
            }
            else {
                page.displayed_lines[current_line] = malloc(strlen(line) + 1);
                strcpy(page.displayed_lines[current_line], line);
            }
            current_line++;
        }

        line = strtok_r(NULL, "\n", &saveptr);
    }

    page.num_displayed_lines = printable_line_count;
    page.num_links = link_count;
    printf("%d lines, %d links, %d printable lines\n", total_lines, page.num_links, page.num_displayed_lines);
    return page;
}

gemtext_link parse_link(char* line, int index, URL page_url) {
    gemtext_link link;

    (void) strtok(line, " \t");
    char* str_ref = strtok(NULL, " \t");
    URL ref = parse_url(str_ref);
    if (!*ref.proto) {
        memcpy(&ref, &page_url, sizeof(URL));
        strncat(ref.path, str_ref, 2048);
    }

    sanitize_url(&ref);
    link.ref = ref;
    char* link_text = strtok(NULL, "");
    strncpy(link.text, link_text, 2048);
    link.index = index;

    return link;
}