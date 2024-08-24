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

struct DisqueUser {
  char username[33];
  char discriminator[5];
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
  DQE_UNKNOWN = 0,
  DQE_READY,
  DQE_HELLO,
  DQE_ACK,
};

struct DisqueEvent {
  enum DisqueEventType type;
  union {
    struct {
      struct DisqueUser user;
    } ready;
    struct {
      int heartbeat_interval;
    } hello;
  } data;
};

enum DisqueIntents {
  DQI_GUILDS = 1 << 0,
  DQI_GUILD_MEMBERS = 1 << 1,
  DQI_GUILD_MODERATION = 1 << 2,
  DQI_GUILD_EMOJI_AND_STICKERS = 1 << 3,
  DQI_GUILD_INTEGRATIONS = 1 << 4,
  DQI_GUILD_WEBHOOKS = 1 << 5,
  DQI_GUILD_INVITES = 1 << 6,
  DQI_GUILD_VOICE_STATES = 1 << 7,
  DQI_GUILD_PRESENCES = 1 << 8,
  DQI_GUILD_MESSAGES = 1 << 9,
  DQI_GUILD_MESSAGE_REACTIONS = 1 << 10,
  DQI_GUILD_MESSAGE_TYPING = 1 << 11,
  DQI_DIRECT_MESSAGES = 1 << 12,
  DQI_DIRECT_MESSAGE_REACTIONS = 1 << 13,
  DQI_DIRECT_MESSAGE_TYPING = 1 << 14,
  DQI_MESSAGE_CONTENT = 1 << 15,
  DQI_GUILD_SCHEDULED_EVENTS = 1 << 16,
  DQI_AUTO_MODERATION_CONFIGURATION = 1 << 20,
  DQI_AUTO_MODERATION_EXECUTION = 1 << 21,
  DQI_GUILD_MESSAGE_POLLS = 1 << 24,
  DQI_DIRECT_MESSAGE_POLLS = 1 << 25,
};

enum DisqueCode disque_global_init();

enum DisqueCode disque_connect_gateway(struct DisqueContext *ctx, char *url);
enum DisqueCode disque_recv(struct DisqueContext *ctx, struct DisqueEvent *res);
enum DisqueCode disque_send_heartbeat(struct DisqueContext *ctx);
enum DisqueCode disque_send_identify(struct DisqueContext *ctx, int intents);
void disque_free_gateway(struct DisqueContext *ctx);

enum DisqueCode disque_get_gateway(struct DisqueContext *ctx, struct DisqueGatewayResponse *res);
enum DisqueCode disque_get_current_user(struct DisqueContext *ctx, struct DisqueUser *res);
