#include "client.h"
#include "mjson.h"
#include "stdio.h"

// The very first web page in history. You can replace it from command line
static char s_url[50] = "http://info.cern.ch/";
static const uint64_t s_timeout_ms = 1500;  // Connect timeout in milliseconds
static char result[50];
static size_t result_len = 0;

void handle_response(int length, const char *data) {
    struct mg_http_message *msg = malloc(sizeof(struct mg_http_message));
    mg_http_parse(data, length, msg);

    result_len = mjson_get_string(msg->body.ptr, strlen(msg->body.ptr), "$.timezone.current_time", result, 50);

    int success = 0;
    mjson_get_bool(msg->body.ptr, strlen(msg->body.ptr), "$.success", success);

    if (!success) {
        strcpy(result, "Unknown time\0");
        result_len = strlen(result);
    }
}

// Print HTTP response and signal that we're done
static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_OPEN) {
    // Connection created. Store connect expiration time in c->label
    *(uint64_t *) c->label = mg_millis() + s_timeout_ms;
  } else if (ev == MG_EV_POLL) {
    if (mg_millis() > *(uint64_t *) c->label &&
        (c->is_connecting || c->is_resolving)) {
      mg_error(c, "Connect timeout");
    }
  } else if (ev == MG_EV_CONNECT) {
    // Connected to server. Extract host name from URL
    struct mg_str host = mg_url_host(s_url);

    // Send request
    mg_printf(c,
              "%s %s HTTP/1.0\r\n"
              "Host: %.*s\r\n"
              "Content-Type: octet-stream\r\n"
              "Content-Length: %d\r\n"
              "\r\n",
              "GET", mg_url_uri(s_url), (int) host.len,
              host.ptr, 0);
    mg_send(c, NULL, 0);
  } else if (ev == MG_EV_HTTP_MSG) {
    // Response is received. Print it
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    handle_response((int) hm->message.len, hm->message.ptr);
    c->is_closing = 1;         // Tell mongoose to close this connection
    *(bool *) fn_data = true;  // Tell event loop to stop
  } else if (ev == MG_EV_ERROR) {
    *(bool *) fn_data = true;  // Error, tell event loop to stop
  }
}

// Returns response length
size_t fetch_time(char *ip, char *to) {
  struct mg_mgr mgr;              // Event manager
  bool done = false;              // Event handler flips it to true
  snprintf(s_url, 50, "http://ipwho.is/%s", ip);
  mg_log_set("0");                // Set to 0 to disable debug
  mg_mgr_init(&mgr);              // Initialise event manager
  mg_http_connect(&mgr, s_url, fn, &done);  // Create client connection
  while (!done) mg_mgr_poll(&mgr, 50);      // Infinite event loop
  mg_mgr_free(&mgr);                        // Free resources
  strcpy(to, result);
  return result_len;
}