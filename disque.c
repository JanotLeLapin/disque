#include "disque.h"

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
disque_request(CURL *curl, struct DisqueContext *ctx)
{
  CURLcode code;
  struct curl_slist *headers = NULL;
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

struct DisqueGatewayResponse *
disque_get_gateway_response(struct DisqueContext *ctx)
{
  CURL *curl;
  char *res;
  struct DisqueGatewayResponse *gateway;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (!curl)
    return NULL;
  curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/v10/gateway/bot");
  res = disque_request(curl, ctx);
  printf("%s\n", res);

  gateway = malloc(sizeof(struct DisqueGatewayResponse));
  cJSON *json = cJSON_Parse(res);
  cJSON *limit = cJSON_GetObjectItemCaseSensitive(json, "session_start_limit");
  strcpy(gateway->url, cJSON_GetObjectItemCaseSensitive(json, "url")->valuestring);
  gateway->shards = cJSON_GetObjectItemCaseSensitive(json, "shards")->valueint;
  gateway->session_start_limit.total = cJSON_GetObjectItemCaseSensitive(limit, "total")->valueint;
  gateway->session_start_limit.remaining = cJSON_GetObjectItemCaseSensitive(limit, "remaining")->valueint;
  gateway->session_start_limit.reset_after = cJSON_GetObjectItemCaseSensitive(limit, "reset_after")->valueint;
  gateway->session_start_limit.max_concurrency = cJSON_GetObjectItemCaseSensitive(limit, "max_concurrency")->valueint;

  cJSON_Delete(json);
  free(res);

  return gateway;
}

struct DisqueUser *
disque_get_current_user(struct DisqueContext *ctx)
{
  CURL *curl;
  char *res;
  struct DisqueUser *user;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (!curl)
    return NULL;
  curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/v10/users/@me");
  res = disque_request(curl, ctx);

  cJSON *json = cJSON_Parse(res);
  cJSON *username = cJSON_GetObjectItemCaseSensitive(json, "username");

  user = malloc(sizeof(struct DisqueUser));
  strcpy(user->username, username->valuestring);

  cJSON_Delete(json);
  free(res);

  return user;
}
