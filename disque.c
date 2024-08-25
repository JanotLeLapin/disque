#include "disque.h"

#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum DisqueCode
disque_global_init()
{
  return curl_global_init(CURL_GLOBAL_ALL) ? DQC_ERROR : DQC_OK;
}

enum DisqueCode
disque_connect_gateway(struct DisqueContext *ctx, char *url)
{
  CURL* curl;
  CURLM *curlm;

  curl = curl_easy_init();
  if (!curl)
    return DQC_ERROR;

  curlm = curl_multi_init();
  if (!curlm)
    return DQC_ERROR;

  curl_multi_add_handle(curlm, curl);

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 2L);

  ctx->curl = curl;
  ctx->curlm = curlm;

  return DQC_OK;
}

enum DisqueCode
disque_recv(struct DisqueContext *ctx, struct DisqueEvent *res)
{
  CURLMcode mc;
  size_t len;
  const struct curl_ws_frame *meta;
  char buffer[2048];
  int result;
  char *packet;
  cJSON *json, *d, *s;
  char *etype;

  mc = curl_multi_perform(ctx->curlm, &ctx->running);
  if (ctx->running)
    mc = curl_multi_poll(ctx->curlm, NULL, 0, 1000, NULL);

  if (mc)
    return DQC_ERROR;

  result = curl_ws_recv(ctx->curl, buffer, sizeof(buffer), &len, &meta);
  if (result)
    /* we might want to find out whether curl_ws_recv failed */
    return DQC_AGAIN;

  packet = malloc(len + 1);
  strncpy(packet, buffer, len);
  packet[len] = '\0';

  json = cJSON_Parse(packet);
  d = cJSON_GetObjectItemCaseSensitive(json, "d");
  cJSON *foo = cJSON_GetObjectItemCaseSensitive(json, "op");
  switch (cJSON_GetObjectItemCaseSensitive(json, "op")->valueint) {
    case 0: {
      etype = cJSON_GetObjectItemCaseSensitive(json, "t")->valuestring;
      if (!strcmp("READY", etype)) {
        cJSON *user= cJSON_GetObjectItemCaseSensitive(d, "user");
        res->type = DQE_READY;
        strcpy(res->data.ready.user.username, cJSON_GetObjectItemCaseSensitive(user, "username")->valuestring);
        strcpy(res->data.ready.user.discriminator, cJSON_GetObjectItemCaseSensitive(user, "discriminator")->valuestring);
      } else {
        res->type = DQE_UNKNOWN;
      }
      break;
    }
    case 10:
      res->type = DQE_HELLO;
      res->data.hello.heartbeat_interval = cJSON_GetObjectItemCaseSensitive(d, "heartbeat_interval")->valueint;
      break;
    case 11:
      res->type = DQE_ACK;
      break;
    default:
      res->type = DQE_UNKNOWN;
      break;
  }

  s = cJSON_GetObjectItemCaseSensitive(json, "s");
  if (cJSON_IsNumber(s))
    ctx->seq = s->valueint;

  cJSON_Delete(json);
  free(packet);

  return DQC_OK;
}

enum DisqueCode
disque_send_heartbeat(struct DisqueContext *ctx)
{
  char payload[256];
  char seq[16];
  size_t sent;

  if (ctx->seq)
    sprintf(seq, "%d", ctx->seq);
  else
    strcpy(seq, "null");
  
  strcpy(payload, "{\"op\":1,\"d\":");
  strcat(payload, seq);
  strcat(payload, "}");

  return curl_ws_send(ctx->curl, payload, strlen(payload), &sent, 0, CURLWS_TEXT) ? DQC_ERROR : DQC_OK;
}

enum DisqueCode
disque_send_identify(struct DisqueContext *ctx, int intents)
{
  char payload[512];
  char intents_s[9];
  size_t sent;

  sprintf(intents_s, "%d", intents);

  strcpy(payload, "{\"op\":2,\"d\":{\"token\":\"");
  strcat(payload, ctx->token);
  strcat(payload, "\",\"properties\":{\"os\":\"linux\",\"browser\":\"disque\",\"device\":\"disque\"},\"intents\":");
  strcat(payload, intents_s);
  strcat(payload, "}}");

  return curl_ws_send(ctx->curl, payload, strlen(payload), &sent, 0, CURLWS_TEXT) ? DQC_ERROR : DQC_OK;
}

void
disque_free_gateway(struct DisqueContext *ctx)
{
  curl_multi_remove_handle(ctx->curlm, ctx->curl);
  curl_multi_cleanup(ctx->curlm);
  curl_easy_cleanup(ctx->curl);
}
