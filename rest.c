#include "parse.h"

#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ResponseBody {
  size_t size;
  char *content;
};

static size_t
write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct ResponseBody *body = (struct ResponseBody *) userp;

  char *ptr = realloc(body->content, body->size + realsize + 1);
  if (!ptr)
    return 0;

  body->content = ptr;
  memcpy(&(body->content[body->size]), contents, realsize);
  body->size += realsize;
  body->content[body->size] = 0;

  return realsize;
}

char *
disque_request(CURL *curl, struct curl_slist *headers, struct DisqueContext *ctx)
{
  CURLcode code;
  char header[128];
  struct ResponseBody res;

  res.content = malloc(1);
  res.size = 0;

  strcpy(header, "Authorization: Bot ");
  strcat(header, ctx->token);

  headers = curl_slist_append(headers, header);

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &res);

  code = curl_easy_perform(curl);

  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);

  if (CURLE_OK != code)
    return NULL;

  return res.content;
}

enum DisqueCode
disque_get_gateway(struct DisqueContext *ctx, struct DisqueGatewayResponse *res)
{
  CURL *curl;
  char *result;

  curl = curl_easy_init();
  if (!curl)
    return DQC_ERROR;
  curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/v10/gateway/bot");
  result = disque_request(curl, NULL, ctx);

  cJSON *json = cJSON_Parse(result);
  cJSON *limit = cJSON_GetObjectItemCaseSensitive(json, "session_start_limit");
  strcpy(res->url, cJSON_GetObjectItemCaseSensitive(json, "url")->valuestring);
  res->shards = cJSON_GetObjectItemCaseSensitive(json, "shards")->valueint;
  res->session_start_limit.total = cJSON_GetObjectItemCaseSensitive(limit, "total")->valueint;
  res->session_start_limit.remaining = cJSON_GetObjectItemCaseSensitive(limit, "remaining")->valueint;
  res->session_start_limit.reset_after = cJSON_GetObjectItemCaseSensitive(limit, "reset_after")->valueint;
  res->session_start_limit.max_concurrency = cJSON_GetObjectItemCaseSensitive(limit, "max_concurrency")->valueint;

  cJSON_Delete(json);
  free(result);

  return DQC_OK;
}

enum DisqueCode
disque_get_current_user(struct DisqueContext *ctx, struct DisqueUser *res)
{
  CURL *curl;
  char *result;
  struct DisqueUser *user;

  curl = curl_easy_init();
  if (!curl)
    return DQC_ERROR;
  curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/v10/users/@me");
  result = disque_request(curl, NULL, ctx);

  cJSON *json = cJSON_Parse(result);
  disque_parse_user(json, res);
  cJSON_Delete(json);
  free(result);

  return DQC_OK;
}

enum DisqueCode
disque_create_message(struct DisqueContext *ctx, long channel, char *message)
{
  CURL *curl;
  char *result;
  struct curl_slist *headers;

  char url[128];
  snprintf(url, 128, "https://discord.com/api/v10/channels/%ld/messages", channel);

  char body[1024];
  snprintf(body, 1024, "{\"content\":\"%s\"}", message);

  headers = curl_slist_append(NULL, "Content-Type: application/json");

  curl = curl_easy_init();
  if (!curl)
    return DQC_ERROR;
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);

  result = disque_request(curl, headers, ctx);
  printf("%s\n", result);
  free(result);

  return DQC_OK;
}
