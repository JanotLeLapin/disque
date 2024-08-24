struct DisqueContext {
  const char *token;
  void *curl;
  void *curlm;
  int running;
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

enum DisqueEventType {
  DQ_HELLO,
};

struct DisqueEvent {
  enum DisqueEventType type;
  union {
    struct {
      int heartbeat_interval;
    } hello;
  } data;
};

struct DisqueUser {
  char username[32];
};

void disque_global_init();

struct DisqueGatewayResponse *disque_get_gateway(struct DisqueContext *ctx);
void disque_connect_gateway(struct DisqueContext *ctx, char *url);
struct DisqueEvent *disque_recv(struct DisqueContext *ctx);
void disque_free_gateway(struct DisqueContext *ctx);

struct DisqueUser *disque_get_current_user(struct DisqueContext *ctx);
