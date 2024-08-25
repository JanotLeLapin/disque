#include "parse.h"

#include <cjson/cJSON.h>
#include <stdlib.h>
#include <string.h>

void
disque_parse_user(void *json, struct DisqueUser *res)
{
  cJSON *tmp;

  res->id = strtol(cJSON_GetObjectItemCaseSensitive(json, "id")->valuestring, NULL, 10);
  strcpy(res->username, cJSON_GetObjectItemCaseSensitive(json, "username")->valuestring);
  strcpy(res->discriminator, cJSON_GetObjectItemCaseSensitive(json, "discriminator")->valuestring);
  res->is_bot = cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(json, "bot"));
  res->is_system = cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(json, "system"));

  tmp = cJSON_GetObjectItemCaseSensitive(json, "global_name");
  strcpy(res->global_name, cJSON_IsString(tmp) ? tmp->valuestring : "\0");

  tmp = cJSON_GetObjectItemCaseSensitive(json, "avatar");
  strcpy(res->avatar, cJSON_IsString(tmp) ? tmp->valuestring : "\0");

  tmp = cJSON_GetObjectItemCaseSensitive(json, "banner");
  strcpy(res->banner, cJSON_IsString(tmp) ? tmp->valuestring : "\0");
}

void
disque_parse_message(void *json, struct DisqueMessage *res)
{
  cJSON *tmp;

  res->id = strtol(cJSON_GetObjectItemCaseSensitive(json, "id")->valuestring, NULL, 10);
  res->channel_id = strtol(cJSON_GetObjectItemCaseSensitive(json, "channel_id")->valuestring, NULL, 10);
  strcpy(res->content, cJSON_GetObjectItemCaseSensitive(json, "content")->valuestring);

  tmp = cJSON_GetObjectItemCaseSensitive(json, "author");
  if (cJSON_IsObject(tmp)) disque_parse_user(tmp, &res->author);
}
