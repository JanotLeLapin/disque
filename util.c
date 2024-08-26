#include "disque.h"

#include <stdio.h>

void
disque_cdn_avatar(long id, const char *hash, char *buffer, size_t size)
{
  snprintf(buffer, size, "https://cdn.discordapp.com/avatars/%ld/%s", id, hash);
}

void
disque_cdn_banner(long id, const char *hash, char *buffer, size_t size)
{
  snprintf(buffer, size, "https://cdn.discordapp.com/banners/%ld/%s", id, hash);
}

