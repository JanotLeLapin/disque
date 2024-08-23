#include "disque.h"

#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdio.h>

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

char *
disque_recv(struct DisqueContext *ctx)
{
  size_t len;
  const struct curl_ws_frame *meta;
  char buffer[1024];

  curl_ws_recv(ctx->curl, buffer, sizeof(buffer), &len, &meta);
  printf("'%s'\n", buffer);
  return "teehee";
}
