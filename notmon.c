#include "main.h"

static void *DaemonTerminal(void *this);
static void *DaemonListener(void *this);

static void *DaemonTerminal(void *this)
{
  notmon_t *notmon = (notmon_t*)this;

  if(notmon->config->terminal_enable == 1)
  {
    printf("-> Info: terminal is enabled\n");
    printf("# ");
    while(notmon->run == 1)
    {
      char buffer[64];
      fgets(buffer, 64, stdin);

      unsigned long buffer_size = strlen(buffer) - 1;
      buffer[buffer_size] = 0;

      if(buffer_size > 0)
      {
        if(strcmp(buffer, "exit") == 0)
        {
          printf("# daemon is shuting down\n");
          notmon->run = 0;
        }
        else
          printf("# %s: unknown command\n# ", buffer);
      }
      else
        printf("# ");
    }
  }
  else
    printf("-> Info: terminal is disabled\n");

  printf("-> Info: daemon terminal is shuting down\n");
  pthread_exit(NULL);
  return NULL;
}

static void *DaemonListener(void *this)
{
  notmon_t *notmon = (notmon_t*)this;

  printf("-> Info: auditor is enabled\n");
  while(notmon->run == 1)
  {
    time_t now;
    struct tm *now_tm;
    now = time(NULL);
    now_tm = localtime(&now);
    int hour = now_tm->tm_hour;
    int minute = now_tm->tm_min;
    int sec = now_tm->tm_sec;
    int new_sec;

    if((sec - notmon->config->interval) < 0)
    {
      new_sec = 60 - notmon->config->interval - -sec;
      if((minute - 1) < 0)
      {
        minute = 59;
        if((hour - 1) < 0)
          hour = 23;
      }
    }
    else
      new_sec = sec - notmon->config->interval;

    char *query = malloc(sizeof(char*) * 256);
    sprintf(query, "journalctl -u sshd -n %d -S %d:%d:%d | grep Accepted | awk {'print $9, $11'}",
            notmon->config->line_count, hour, minute, new_sec);

    char *sys_row = SysQuery(query);
    free(query);
    if(sys_row)
    {
      char *new_row = malloc(sizeof(char*) * (strlen(sys_row) + 1));
      strcpy(new_row, sys_row);

      char *stage0 = strtok(new_row, " ");
      if(stage0)
      {
        char *username = malloc(sizeof(char*) * (strlen(stage0) + 1));
        strcpy(username, stage0);
        stage0 = strtok(NULL, " ");
        char *hostname = malloc(sizeof(char*) * (strlen(stage0) + 1));
        strcpy(hostname, stage0);
        unsigned long message_size = sizeof(char*) * ((strlen(username) + 1) + (strlen(hostname) + 1));
        char *message = malloc(message_size + 128);
        sprintf(message, "SSH AUTH\n\nUsername: %s\nHostname: %s", username, hostname);
        TelebotMethodSendMessage(notmon, notmon->telebot->config->master, message, NULL, NULL);

        free(username);
        free(hostname);
        free(message);
      }

      free(new_row);
      free(sys_row);
    }

    sleep(notmon->config->interval);
  }

  printf("-> Info: daemon listener is shuting down\n");
  pthread_exit(NULL);
  return NULL;
}

notmon_t *Construct(notmon_t *notmon)
{
  if(notmon)
  {
    notmon->config = malloc(sizeof(config_t));
    if(notmon->config)
    {
      notmon->config->locate = NULL;
      notmon->config->tmp_dir = NULL;
    }
    else
      printf("-> Error: cant allocate memory for notmon config\n");

    notmon->telebot = malloc(sizeof(telebot_t));
    if(notmon->telebot)
    {
      notmon->telebot->config = malloc(sizeof(telebot_config_t));
      if(notmon->telebot->config)
      {
        notmon->telebot->config->json = NULL;
        notmon->telebot->config->proxy = NULL;
        notmon->telebot->config->token = NULL;
        notmon->telebot->config->master = NULL;
        notmon->telebot->config->api_url = NULL;
      }
      else
        printf("-> Error: cant allocate memory for telebot config\n");

      notmon->squery = malloc(sizeof(sysquery_t));
      if(notmon->squery)
      {
        notmon->squery->buff_query = NULL;
        notmon->squery->last_query = NULL;
      }
      else
        printf("-> Error: cant allocate memory for system query\n");
    }
    else
      printf("-> Error: cant allocate memory for telebot\n");

    return notmon;
  }

  return NULL;
}

notmon_t *Destruct(notmon_t *notmon)
{
  if(notmon)
  {
    if(notmon->config)
    {
      if(notmon->config->locate)
        free(notmon->config->locate);

      if(notmon->config->tmp_dir)
        free(notmon->config->tmp_dir);

      free(notmon->config);
    }

    if(notmon->telebot)
    {
      if(notmon->telebot->config)
      {
        if(notmon->telebot->config->json)
          free(notmon->telebot->config->json);

        if(notmon->telebot->config->proxy)
          free(notmon->telebot->config->proxy);

        if(notmon->telebot->config->token)
          free(notmon->telebot->config->token);

        if(notmon->telebot->config->master)
          free(notmon->telebot->config->master);

        if(notmon->telebot->config->api_url)
          free(notmon->telebot->config->api_url);

        free(notmon->telebot->config);
      }

      free(notmon->telebot);
    }

    if(notmon->squery)
    {
      if(notmon->squery->buff_query)
        free(notmon->squery->buff_query);

      if(notmon->squery->last_query)
        free(notmon->squery->last_query);

      free(notmon->squery);
    }
    free(notmon);
    return notmon;
  }

  return NULL;
}

notmon_t *CheckConfigures(notmon_t *notmon)
{
  if(notmon->config->locate)
  {
    if(access(notmon->config->locate, F_OK))
      printf("-> Error: cant find config file (%s)\n", notmon->config->locate);
    else
      printf("-> Info: loading config file (%s)\n", notmon->config->locate);
  }
  else
    printf("-> Error: location of config file is not set\n");

  printf("-> Info: checking interval set (%d)\n", notmon->config->interval);
  printf("-> Info: count lines for checking sett (%d)\n", notmon->config->line_count);

  if(notmon->config->tmp_dir)
  {
    if(access(notmon->config->tmp_dir, F_OK))
      printf("-> Error: cant find temporary dir (%s)\n", notmon->config->tmp_dir);
    else
      printf("-> Info: temporary dir is (%s)\n", notmon->config->tmp_dir);
  }
  else
    printf("-> Error: temporary dir is not set\n");

  if(notmon->config->telebot_enable == 1)
  {
    printf("-> Info: telebot module is enabled\n");

    char *token = malloc(sizeof(char) * (strlen(notmon->telebot->config->token) + 1));

    for(int st0 = 0; st0 < (int)strlen(notmon->telebot->config->token); ++st0)
      token[st0] = '*';

    token[strlen(notmon->telebot->config->token)] = '\0';
    printf("-> Info: telebot token is (%s)\n", token);
    free(token);

    if(notmon->telebot->config->proxy)
      printf("-> Info: telebot proxy is (%s)\n", notmon->telebot->config->proxy);
    else
      printf("-> Info: telebot proxy is turned off\n");

    printf("-> Info: telebot master is (%s)\n", notmon->telebot->config->master);
  }

  return notmon;
}

notmon_t *DaemonStart(notmon_t *notmon)
{
  notmon->run = 1;

  ConfigPrepare(notmon);
  CheckConfigures(notmon);

  if(notmon->config->telebot_enable == 1)
    TelebotMethodGetMe(notmon);

  pthread_t pTerminal, pListener;

  int ret;

  ret = pthread_create(&pListener, NULL, DaemonListener, notmon);
  if(ret != 0)
    printf("-> Error: cant create thread for daemon listener\n");

  sleep(1);

  if(notmon->config->terminal_enable == 1)
  {
    ret = pthread_create(&pTerminal, NULL, DaemonTerminal, notmon);
    if(ret != 0)
      printf("-> Error: cant create thread for daemon terminal\n");

    sleep(1);

    ret = pthread_join(pTerminal, NULL);
    if(ret != 0)
      printf("-> Error: cant join to daemon terminal thread\n");
  }

  ret = pthread_join(pListener, NULL);
  if(ret != 0)
    printf("-> Error: cant join to daemon listen thread\n");

  return notmon;
}
