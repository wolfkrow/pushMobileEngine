#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <curl/curl.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};


static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory == NULL) {
    /* out of memory! */ 
    syslog (LOG_ERR, "not enough memory (realloc returned NULL)\n");
    exit(EXIT_FAILURE);
  }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}


char *getAuthTocken (char *user, char*pass, char* source ){

 	struct curl_httppost *formpost=NULL;
  	struct curl_httppost *lastptr=NULL;
    struct curl_slist *headerlist=NULL;
    struct MemoryStruct chunk;
    CURL *curl_handle;

  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  chunk.size = 0;    /* no data at this point */ 
 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
 
 /*Post VARS */
  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "Email",
               CURLFORM_COPYCONTENTS, user,
               CURLFORM_END);
  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "Passwd",
               CURLFORM_COPYCONTENTS, pass,
               CURLFORM_END);
 
 curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "accountType",
               CURLFORM_COPYCONTENTS, "GOOGLE",
               CURLFORM_END);

 curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "source",
               CURLFORM_COPYCONTENTS, source,
               CURLFORM_END);
               
               
 curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerlist);
 curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, formpost);
  
  /* specify URL to get */ 
  curl_easy_setopt(curl_handle, CURLOPT_URL, "https://www.google.com/accounts/ClientLogin");
  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "PMEngine-agent/1.0");
  curl_easy_perform(curl_handle);
 
  curl_easy_cleanup(curl_handle);
 
 
  printf("%lu bytes retrieved\n", (long)chunk.size);

  
//  if(chunk.memory)
//    free(chunk.memory);
 
  /* we're done with libcurl, so clean it up */ 
curl_global_cleanup();
return  chunk.memory;
}
 
