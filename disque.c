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

struct DisqueUser *
disque_get_current_user(struct DisqueContext *ctx)
{
  CURL *curl;
  char *response;
  struct DisqueUser *user;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (!curl)
    return NULL;
  curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/v10/users/@me");
  response = disque_request(curl, ctx);

  cJSON *json = cJSON_Parse(response);
  cJSON *username = cJSON_GetObjectItemCaseSensitive(json, "username");

  user = malloc(sizeof(struct DisqueUser));
  strcpy(user->username, username->valuestring);

  cJSON_Delete(json);
  free(response);

  return user;
}
