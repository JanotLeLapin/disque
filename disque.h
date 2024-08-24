struct DisqueContext {
  const char *token;
  void *curl;
  void *curlm;
  int seq;
  int running;
};

enum DisqueCode {
  DQC_OK = 0,
  DQC_ERROR,
  DQC_AGAIN,
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
  DQE_HELLO,
  DQE_ACK,
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

enum DisqueCode disque_global_init();

enum DisqueCode disque_connect_gateway(struct DisqueContext *ctx, char *url);
enum DisqueCode disque_recv(struct DisqueContext *ctx, struct DisqueEvent *res);
enum DisqueCode disque_send_heartbeat(struct DisqueContext *ctx);
enum DisqueCode disque_send_identify(struct DisqueContext *ctx);
void disque_free_gateway(struct DisqueContext *ctx);

enum DisqueCode disque_get_gateway(struct DisqueContext *ctx, struct DisqueGatewayResponse *res);
enum DisqueCode disque_get_current_user(struct DisqueContext *ctx, struct DisqueUser *res);
