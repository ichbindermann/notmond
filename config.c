#include "main.h"

config_t *ConfigPrepare(notmon_t *notmon)
{
  if(notmon->config)
  {
    char *config_contain = FileToBuffer(notmon->config->locate);
    if(config_contain)
    {
      json_object *obj = json_tokener_parse(config_contain);

      json_object *interval;
      json_object_object_get_ex(obj, "interval", &interval);
      if(json_object_get_string(interval))
        notmon->config->interval = atoi(json_object_get_string(interval));
      else
        notmon->config->interval = 10;

      json_object *line_count;
      json_object_object_get_ex(obj, "line_count", &line_count);
      if(json_object_get_string(line_count))
        notmon->config->line_count = atoi(json_object_get_string(line_count));
      else
        notmon->config->line_count = 10;

      json_object *temporary_dir;
      json_object_object_get_ex(obj, "temporary_dir", &temporary_dir);
      if(json_object_get_string(temporary_dir))
      {
        notmon->config->tmp_dir = malloc(sizeof(char) * (strlen(json_object_get_string(temporary_dir)) + 1));
        strcpy(notmon->config->tmp_dir, json_object_get_string(temporary_dir));
      }

      json_object *telebot_module;
      json_object_object_get_ex(obj, "telebot_module", &telebot_module);

      if(telebot_module)
      {
        notmon->config->telebot_enable = 1;
        TelebotConfigParse(notmon, json_object_get_string(telebot_module));
      }

      if(obj)
        json_object_put(obj);

      free(config_contain);
    }
  }
  else
    printf("-> Error: config memory is failure\n");

  return notmon->config;
}
