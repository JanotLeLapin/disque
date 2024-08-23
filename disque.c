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

struct DisqueUser *
disque_get_current_user(struct DisqueContext *ctx)
{
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers = NULL;
  char header[128];
  struct ResponseBody response;
  struct DisqueUser *user = malloc(sizeof(struct DisqueUser));

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if (!curl) return NULL;

  response.content = malloc(1);
  response.size = 0;

  strcpy(header, "Authorization: Bot ");
  strcat(header, ctx->token);

  headers = curl_slist_append(headers, header);

  curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/v10/users/@me");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);

  res = curl_easy_perform(curl);

  if (CURLE_OK != res) return NULL;

  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);

  cJSON *json = cJSON_Parse(response.content);
  cJSON *username = cJSON_GetObjectItemCaseSensitive(json, "username");

  strcpy(user->username, username->valuestring);

  cJSON_Delete(json);
  free(response.content);

  return user;
}
