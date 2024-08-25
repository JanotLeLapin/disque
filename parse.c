#include "parse.h"

#include <cjson/cJSON.h>
#include <string.h>

void
disque_parse_user(void *json, struct DisqueUser *res)
{
  strcpy(res->username, cJSON_GetObjectItemCaseSensitive(json, "username")->valuestring);
  strcpy(res->discriminator, cJSON_GetObjectItemCaseSensitive(json, "discriminator")->valuestring);
}
