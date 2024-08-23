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
  CURLcode res;

  curl = curl_easy_init();
  if (!curl)
    return;

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 2L);

  res = curl_easy_perform(curl);

  if (CURLE_OK != res)
    return;

  ctx->curl = curl;
}

struct DisqueEvent *
disque_recv(struct DisqueContext *ctx)
{
  size_t len;
  const struct curl_ws_frame *meta;
  char buffer[2048];
  char *packet;
  cJSON *json, *d;
  struct DisqueEvent *event = NULL;

  curl_ws_recv(ctx->curl, buffer, sizeof(buffer), &len, &meta);
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

  return event;
}

void
disque_free_gateway(struct DisqueContext *ctx)
{
  curl_easy_cleanup(ctx->curl);
}
