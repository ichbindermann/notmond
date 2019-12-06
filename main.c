#include "main.h"

int main(int argc, char *argv[])
{
  notmon_t *notmon = malloc(sizeof(notmon_t));
  if(notmon)
  {
    Construct(notmon);

    for(int st0 = 1; st0 < argc; ++st0)
    {
      if(argv[st0] != NULL)
      {
        char *save_str = malloc(sizeof(char) * (strlen(argv[st0]) + 1));
        strcpy(save_str, argv[st0]);

        char *stage0 = strtok(save_str, "=");
        if(strcmp(stage0, "-config") == 0)
        {
          stage0 = strtok(NULL, "=");
          if(stage0)
          {
            notmon->config->locate = malloc(sizeof(char) * (strlen(stage0) + 1));
            strcpy(notmon->config->locate, stage0);
          }
        }

        if(strcmp(stage0, "-terminal") == 0)
        {
          stage0 = strtok(NULL, "=");
          if(stage0 && strcmp(stage0, "enable") == 0)
            notmon->config->terminal_enable = 1;
          else
            notmon->config->terminal_enable = 0;
        }

        free(save_str);
      }
    }

    DaemonStart(notmon);
    Destruct(notmon);
  }
  else
    printf("-> Error: cant allocate memory for this pattern\n");

  return 0;
}
