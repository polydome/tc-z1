#include "client.h"
#include "mjson.h"
#include "stdio.h"

#define URL_SIZE 100

static char s_url[URL_SIZE];
static const uint64_t TIMEOUT_MS = 1500;
static char s_result[50];
static size_t s_result_len = 0;

void handle_response(struct mg_http_message *message) {
    s_result_len = mjson_get_string(message->body.ptr, strlen(message->body.ptr), "$.timezone.current_time", s_result, 50);

    int success = 0;
    mjson_get_bool(message->body.ptr, strlen(message->body.ptr), "$.success", &success);

    if (!success) {
        strcpy(s_result, "Unknown time\0");
        s_result_len = strlen(s_result);
    }
}

void init_connection(struct mg_connection *c) {
  *(uint64_t *) c->label = mg_millis() + TIMEOUT_MS;
}

void check_timeout(struct mg_connection *c) {
  bool timed_out = mg_millis() > *(uint64_t *) c->label;
  bool is_active = c->is_connecting || c->is_resolving;
  if (timed_out && is_active) {
      mg_error(c, "Connect timeout");
    }
}

void send_request(struct mg_connection *c) {
  struct mg_str host = mg_url_host(s_url);

  mg_printf(c,
            "GET %s HTTP/1.0\r\n"
            "Host: %.*s\r\n"
            "Content-Type: octet-stream\r\n"
            "Content-Length: 0\r\n"
            "\r\n",
            mg_url_uri(s_url), (int) host.len, host.ptr);

  mg_send(c, NULL, 0);
}

// Print HTTP response and signal that we're done
static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  switch (ev) {
    case MG_EV_OPEN: init_connection(c); break;
    case MG_EV_POLL: check_timeout(c); break;
    case MG_EV_CONNECT: send_request(c); break;
    case MG_EV_HTTP_MSG:
      handle_response((struct mg_http_message *) ev_data);
      c->is_closing = 1;
      *(bool *) fn_data = true;
      break;
    case MG_EV_ERROR:
      *(bool *) fn_data = true;
      break;
  }
}

size_t fetch_time(char *ip, char *buf, size_t buf_size) {
  snprintf(s_url, URL_SIZE, "http://ipwho.is/%s", ip);

  struct mg_mgr mgr;
  bool done = false;

  mg_log_set("0");
  mg_mgr_init(&mgr);
  mg_http_connect(&mgr, s_url, fn, &done);

  while (!done)
    mg_mgr_poll(&mgr, 50);

  mg_mgr_free(&mgr);

  snprintf(buf, buf_size, "%s", s_result);
  return s_result_len;
}