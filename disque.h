struct DisqueContext {
  const char *token;
};

struct DisqueGatewayResponse {
  char url[128];
  char shards;
  struct {
    int total;
    int remaining;
    int reset_after;
    int max_concurrency;
  } session_start_limit;
};

struct DisqueUser {
  char username[32];
};

void disque_global_init();

struct DisqueGatewayResponse disque_get_gateway(struct DisqueContext *ctx);
struct DisqueUser *disque_get_current_user(struct DisqueContext *ctx);
