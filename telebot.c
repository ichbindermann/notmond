#include "main.h"

static void *TelebotListener(void *this);

static void *TelebotListener(void *this)
{
  notmon_t *notmon = (notmon_t*)this;
  //  not yet //
  free(notmon);

  pthread_exit(NULL);
  return NULL;
}

telebot_getme_t *TelebotParseGetMe(notmon_t *notmon, const char *json)
{
  if(json)
  {
    json_object *obj = json_tokener_parse(json);
    json_object *result;
    json_object_object_get_ex(obj, "result", &result);
    if(json_object_get_string(result))
    {
      json_object *id, *is_bot, *first_name, *username;
      json_object_object_get_ex(result, "id", &id);
      if(json_object_get_string(id))
      {
        notmon->telebot->method->getme->id = malloc(sizeof(char) * (strlen(json_object_get_string(id)) + 1));
        strcpy(notmon->telebot->method->getme->id, json_object_get_string(id));
      }

      json_object_object_get_ex(result, "is_bot", &is_bot);
      if(json_object_get_string(is_bot))
      {
        notmon->telebot->method->getme->is_bot = malloc(sizeof(char) * (strlen(json_object_get_string(is_bot)) + 1));
        strcpy(notmon->telebot->method->getme->is_bot, json_object_get_string(is_bot));
      }

      json_object_object_get_ex(result, "first_name", &first_name);
      if(json_object_get_string(first_name))
      {
        notmon->telebot->method->getme->first_name = malloc(sizeof(char) * (strlen(json_object_get_string(first_name)) + 1));
        strcpy(notmon->telebot->method->getme->first_name, json_object_get_string(first_name));
      }

      json_object_object_get_ex(result, "username", &username);
      if(json_object_get_string(username))
      {
        notmon->telebot->method->getme->username = malloc(sizeof(char) * (strlen(json_object_get_string(username)) + 1));
        strcpy(notmon->telebot->method->getme->username, json_object_get_string(username));
      }
    }

    json_object_put(obj);
  }
  else
    printf("-> Error: input json is failure\n");

  return notmon->telebot->method->getme;
}

telebot_config_t *TelebotConfigParse(notmon_t *notmon, const char *json)
{
  if(json)
  {
    json_object *obj = json_tokener_parse(json);

    json_object *telebot_enable;
    json_object_object_get_ex(obj, "enable", &telebot_enable);
    if(json_object_get_string(telebot_enable))
    {
      if(strcmp(json_object_get_string(telebot_enable), "true") == 0)
      {
        json_object *telebot_token;
        json_object_object_get_ex(obj, "token", &telebot_token);
        if(json_object_get_string(telebot_token))
        {
          notmon->telebot->config->token = malloc(sizeof(char) * (strlen(json_object_get_string(telebot_token)) + 1));
          strcpy(notmon->telebot->config->token, json_object_get_string(telebot_token));
        }

        json_object *telebot_master;
        json_object_object_get_ex(obj, "master", &telebot_master);
        if(json_object_get_string(telebot_master))
        {
          notmon->telebot->config->master = malloc(sizeof(char) * (strlen(json_object_get_string(telebot_master)) + 1));
          strcpy(notmon->telebot->config->master, json_object_get_string(telebot_master));
        }

        json_object *telebot_proxy;
        json_object_object_get_ex(obj, "proxy", &telebot_proxy);
        if(json_object_get_string(telebot_proxy))
        {
          notmon->telebot->config->proxy = malloc(sizeof(char) * (strlen(json_object_get_string(telebot_proxy)) + 1));
          strcpy(notmon->telebot->config->proxy, json_object_get_string(telebot_proxy));
        }

        json_object *telebot_api_url;
        json_object_object_get_ex(obj, "api_url", &telebot_api_url);
        if(json_object_get_string(telebot_api_url))
        {
          notmon->telebot->config->api_url = malloc(sizeof(char) * (strlen(json_object_get_string(telebot_api_url)) + 1));
          strcpy(notmon->telebot->config->api_url, json_object_get_string(telebot_api_url));
        }
      }
    }

    json_object_put(obj);
  }
  else
    printf("-> Error: telebot config json is failure\n");

  return notmon->telebot->config;
}

telebot_t *TelebotMethodGetMe(notmon_t *notmon)
{
  notmon->telebot->method = malloc(sizeof(telebot_method_t));
  if(notmon->telebot->method)
  {
    notmon->telebot->method->getme = malloc(sizeof(telebot_getme_t));
    if(notmon->telebot->method->getme)
    {
      char *resp = TelebotPerformQuery(notmon, NULL, "getMe");
      TelebotParseGetMe(notmon, resp);

      printf("-> Info: telebot id (%s)\n-> Info: telebot first name (%s)\n-> Info: telebot username (%s)\n",
             notmon->telebot->method->getme->id,
             notmon->telebot->method->getme->first_name,
             notmon->telebot->method->getme->username);

      free(resp);
      free(notmon->telebot->method->getme->id);
      free(notmon->telebot->method->getme->is_bot);
      free(notmon->telebot->method->getme->first_name);
      free(notmon->telebot->method->getme->username);
      free(notmon->telebot->method->getme);
    }
    else
      printf("-> Error: cant allocate memory for telebot method getMe\n");

    free(notmon->telebot->method);
  }
  else
    printf("-> Error: cant allocate memory for telebot method\n");

  return notmon->telebot;
}

telebot_t *TelebotMethodSendMessage(notmon_t *notmon, char *chat_id, char *text, char *reply_to_message_id, char *callback_query_data)
{
  struct curl_httppost *post = NULL, *last = NULL;

  curl_formadd(&post, &last, CURLFORM_COPYNAME, "chat_id", CURLFORM_COPYCONTENTS, chat_id, CURLFORM_END);
  curl_formadd(&post, &last, CURLFORM_COPYNAME, "text", CURLFORM_COPYCONTENTS, text, CURLFORM_END);

  if(reply_to_message_id != NULL)
    curl_formadd(&post, &last, CURLFORM_COPYNAME, "reply_to_message_id", CURLFORM_COPYCONTENTS, reply_to_message_id, CURLFORM_END);

  if(callback_query_data != NULL)
    curl_formadd(&post, &last, CURLFORM_COPYNAME, "reply_markup", CURLFORM_COPYCONTENTS, callback_query_data, CURLFORM_END);

  void *result = TelebotPerformQuery(notmon, post, "sendMessage");
  if(result)
    free(result);


  return notmon->telebot;
}

telebot_t *TelebotStart(notmon_t *notmon)
{
  if(notmon)
  {
    pthread_t pListener;
    pthread_attr_t aListener;

    int ret;
    notmon->telebot->run = 1;

    ret = pthread_attr_init(&aListener);
    if(ret != 0)
      printf("-> Error: cant init telebot listener attr\n");

    ret = pthread_attr_setdetachstate(&aListener, PTHREAD_CREATE_JOINABLE);
    if(ret != 0)
      printf("-> Error: cant create telebot detach listener\n");

    ret = pthread_create(&pListener, &aListener, TelebotListener, notmon);
    if(ret != 0)
      printf("-> Error: cant create telebot listener\n");

    sleep(1);

    ret = pthread_join(pListener, NULL);
    if(ret != 0)
      printf("-> Error: cant join to telebot listener thread\n");
  }
  else
    printf("-> Error: notmon pattern is failure\n");

  return notmon->telebot;
}
