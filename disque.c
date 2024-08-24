#include "disque.h"

#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdlib.h>
#include <string.h>

void disque_global_init()
{
  curl_global_init(CURL_GLOBAL_ALL);
}

void
disque_connect_gateway(struct DisqueContext *ctx, char *url)
{
  CURL* curl;
  CURLM *curlm;

  curl = curl_easy_init();
  if (!curl)
    return;

  curlm = curl_multi_init();
  if (!curlm)
    return;

  curl_multi_add_handle(curlm, curl);

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 2L);

  ctx->curl = curl;
  ctx->curlm = curlm;
}

struct DisqueEvent *
disque_recv(struct DisqueContext *ctx)
{
  CURLMcode mc;
  size_t len;
  const struct curl_ws_frame *meta;
  char buffer[2048];
  int res;
  char *packet;
  cJSON *json, *d;
  struct DisqueEvent *event = NULL;

  mc = curl_multi_perform(ctx->curlm, &ctx->running);
  if (ctx->running) {
    mc = curl_multi_poll(ctx->curlm, NULL, 0, 1000, NULL);
  }

  if (mc) {
    return NULL;
  }

  res = curl_ws_recv(ctx->curl, buffer, sizeof(buffer), &len, &meta);
  if (CURLE_AGAIN == res || CURLE_OK != res) {
    return NULL;
  }

  packet = malloc(len + 1);
  strncpy(packet, buffer, len);
  packet[len] = '\0';

  json = cJSON_Parse(packet);
  d = cJSON_GetObjectItemCaseSensitive(json, "d");
  cJSON *foo = cJSON_GetObjectItemCaseSensitive(json, "op");
  switch (cJSON_GetObjectItemCaseSensitive(json, "op")->valueint) {
    case 10:
      event = malloc(sizeof(struct DisqueEvent));
      event->type = DQ_HELLO;
      event->data.hello.heartbeat_interval = cJSON_GetObjectItemCaseSensitive(d, "heartbeat_interval")->valueint;
      break;
    default:
      break;
  }

  cJSON_Delete(json);
  free(packet);

  return event;
}

void
disque_free_gateway(struct DisqueContext *ctx)
{
  curl_multi_remove_handle(ctx->curlm, ctx->curl);
  curl_multi_cleanup(ctx->curlm);
  curl_easy_cleanup(ctx->curl);
}
