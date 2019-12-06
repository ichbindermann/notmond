#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <json-c/json.h>
#include <curl/curl.h>
#include <pthread.h>

typedef struct curl_callback_data
{
  char *resp;
  size_t size;
}curl_callback_data_t;

typedef struct telebot_config
{
  char *token;
  char *master;
  char *proxy;
  char *api_url;
  char *json;
}telebot_config_t;

typedef struct telebot_getme
{
  char *id;
  char *is_bot;
  char *first_name;
  char *username;
}telebot_getme_t;

typedef struct telebot_method
{
  telebot_getme_t *getme;
}telebot_method_t;

typedef struct telebot
{
  telebot_config_t *config;
  telebot_method_t *method;
  int run;
}telebot_t;

typedef struct sysquery
{
  char *last_query;
  char *buff_query;
}sysquery_t;

typedef struct config
{
  char *locate;
  char *tmp_dir;
  int interval;
  int line_count;
  int terminal_enable;
  int telebot_enable;
}config_t;

typedef struct notmon
{
  config_t *config;
  sysquery_t *squery;
  telebot_t *telebot;

  int run;
}notmon_t;

typedef enum ret
{
  none, success, failure,
}ret_e;

void *SysQuery(char *query);
void *GetRandomString(int length);
void *FileToBuffer(char *file);
void *CurlTest(char *url);
void *TelebotPerformQuery(notmon_t *notmon, struct curl_httppost *post, char *method);

notmon_t *Construct(notmon_t *notmon);
notmon_t *Destruct(notmon_t *notmon);
notmon_t *CheckConfigures(notmon_t *notmon);
notmon_t *DaemonStart(notmon_t *notmon);

config_t *ConfigPrepare(notmon_t *notmon);

telebot_config_t *TelebotConfigParse(notmon_t *notmon, const char *json);
telebot_getme_t *TelebotParseGetMe(notmon_t *notmon, const char *json);
telebot_t *TelebotMethodGetMe(notmon_t *notmon);
telebot_t *TelebotMethodSendMessage(notmon_t *notmon, char *chat_id, char *text, char *reply_to_message_id, char *callback_query_data);
telebot_t *TelebotStart(notmon_t *notmon);
