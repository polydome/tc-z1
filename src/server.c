#include "mongoose.h"
#include "client.h"
#include <stdio.h>
#include <time.h>

#define AUTHOR "Łukasz Burzak"
#define PORT "8080"

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    char ip[100];
    mg_ntoa(&c->peer, ip, sizeof(ip));
    char time[100];
    size_t len = fetch_time(ip, time);
    mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "%s, %s\n", ip, time);
  }
}

void log_info() {
    printf("Author: %s\n", AUTHOR);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("Server started at: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf("Listening on %s\n", PORT);
}

int main(int argc, char *argv[]) {
    log_info();
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, "http://0.0.0.0:"PORT, fn, &mgr);  // Setup listener
    for (;;) mg_mgr_poll(&mgr, 1000);                         // Event loop
    mg_mgr_free(&mgr); 
    return 0;
}