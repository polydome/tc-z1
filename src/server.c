#include "mongoose.h"
#include "client.h"
#include <stdio.h>
#include <time.h>

#define AUTHOR "Łukasz Burzak"
#define PORT "8080"

const char* server_address = "http://0.0.0.0:"PORT;

static void on_http_message(struct mg_connection *c) {
  char ip[100];
  mg_ntoa(&c->peer, ip, 100);

  char time[100];
  size_t len = fetch_time(ip, time, 100);

  mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "%s, %s\n", ip, time);
}

static void on_http_event(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG)
    on_http_message(c);
}

void log_info() {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

    printf("Author: %s\n", AUTHOR);
    printf(
      "Server started at: %d-%02d-%02d %02d:%02d:%02d\n",
      tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec
    );
    printf("Listening on %s\n", PORT);
}

void start_server() {
    struct mg_mgr mgr;

    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, server_address, on_http_event, &mgr);

    for (;;)
      mg_mgr_poll(&mgr, 1000);

    mg_mgr_free(&mgr);
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    log_info();
    
    start_server();

    return EXIT_SUCCESS;
}