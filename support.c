#include "main.h"

void *FileToBuffer(char *file)
{
  if(access(file, F_OK) != -1)
  {
    char *buffer = NULL;
    unsigned long buff_size = 0;

    FILE *config_file = fopen(file, "r");
    fseek(config_file, 0, SEEK_END);
    buff_size = (unsigned long)ftell(config_file);
    rewind(config_file);
    buffer = malloc((buff_size + 1) * sizeof(*buffer));
    fread(buffer, buff_size, 1, config_file);
    buffer[buff_size] = '\0';
    fclose(config_file);

    return buffer;
  }
  else
    printf("-> Error: cant find config file (%s)\n", file);

  return NULL;
}

void *GetRandomString(int length)
{
  srand((unsigned int)time(NULL));

  char *random_string = malloc(sizeof(char) * (length + 1));
  char text[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int size = length, len = size - 1, st0 = 0;

  for(st0 = 0; st0 < len; ++st0)
    random_string[st0] = text[rand() % (sizeof text - 1)];

  random_string[st0] = '\0';

  return random_string;
}

void *SysQuery(char *query)
{
  char *result = NULL;
  char *temp = NULL;
  char buffer[1024];
  unsigned long size = 1;
  unsigned long length;

  FILE *application;
  application = popen(query, "r");
  if(application)
  {
    while(fgets(buffer, sizeof(buffer), application))
    {
      length = strlen(buffer);
      temp = realloc(result, size + length);
      if(temp)
        result = temp;
      strcpy(result + size -1, buffer);
      size += length;
    }

    pclose(application);
  }
  else
    printf("-> Error: cant execute system query\n");

  return result;
}
