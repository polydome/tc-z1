#include "mongoose.h"
#include "client.h"

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    char ip[100];
    mg_ntoa(&c->peer, ip, sizeof(ip));
    char time[100];
    size_t len = fetch_time(ip, time);
    mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "%s, %s\n", ip, time);
  }
}

int main(int argc, char *argv[]) {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);                                        // Init manager
    mg_http_listen(&mgr, "http://0.0.0.0:8080", fn, &mgr);  // Setup listener
    for (;;) mg_mgr_poll(&mgr, 1000);                         // Event loop
    mg_mgr_free(&mgr); 
    return 0;
}