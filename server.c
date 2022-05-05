#include "mongoose.h"
#include "client.h"

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    char buf[100];
    mg_ntoa(&c->peer, buf, sizeof(buf));
    mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "Hello, %s\n", buf);
  }
}

int main(int argc, char *argv[]) {
//   struct mg_mgr mgr;
//   mg_mgr_init(&mgr);                                        // Init manager
//   mg_http_listen(&mgr, "http://0.0.0.0:8080", fn, &mgr);  // Setup listener
//   for (;;) mg_mgr_poll(&mgr, 1000);                         // Event loop
//   mg_mgr_free(&mgr);                                        // Cleanup
//   return 0;
    fetch_time("http://ipwho.is/83.5.163.168");
    return 0;
}