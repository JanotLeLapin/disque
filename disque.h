struct DisqueContext {
  const char *token;
};

struct DisqueUser {
  char username[32];
};

struct DisqueUser *disque_get_current_user(struct DisqueContext *ctx);
