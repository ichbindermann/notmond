#include "main.h"

static size_t CallBack(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  curl_callback_data_t *data = (curl_callback_data_t*)userp;

  char *ptr = realloc(data->resp, data->size + realsize + 1);
  if(ptr == NULL)
  {
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  data->resp = ptr;
  memcpy(&(data->resp[data->size]), contents, realsize);
  data->size += realsize;
  data->resp[data->size] = 0;

  return realsize;
}

void *CurlTest(char *url)
{
  CURL *curl = curl_easy_init();
  CURLcode res;

  curl_callback_data_t data;
  data.resp = malloc(1);
  data.size = 0;

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CallBack);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NO_REVOKE);

  while(1)
  {
    res = curl_easy_perform(curl);
    if(res == CURLE_OK)
      break;
    else
      printf("-> Error: curl request failure\n");
  }

  char *answer = malloc(sizeof(char) * (strlen(data.resp) + 1));
  strcpy(answer, data.resp);

  curl_easy_cleanup(curl);
  free(data.resp);

  return (void*)answer;
}

void *TelebotPerformQuery(notmon_t *notmon, struct curl_httppost *post, char *method)
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
  CURL *curl = curl_easy_init();
  CURLcode res;

  curl_callback_data_t data;
  data.resp = malloc(1);
  data.size = 0;

  char url[512];
  sprintf(url, "%s%s/%s", notmon->telebot->config->api_url, notmon->telebot->config->token, method);

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CallBack);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
  curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);

  if(post != NULL)
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

  if(notmon->telebot->config->proxy)
    curl_easy_setopt(curl, CURLOPT_PROXY, notmon->telebot->config->proxy);

  while(1)
  {
    res = curl_easy_perform(curl);
    if(res == CURLE_OK)
      break;
    else
      printf("-> Error: curl request is failure\n");
  }

  char *json = malloc(sizeof(char) * (strlen(data.resp) + 1));
  strcpy(json, data.resp);

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  curl_formfree(post);
  free(data.resp);

  return json;
}
