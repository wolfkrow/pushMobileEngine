#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include "Helper/RemoteNotification.h"
#include "Helper/cJSON.h"
#include "Helper/global.h"

#define NUM_THREADS	4


int PushMessage(char *jSONMessage){

	char *out;
	cJSON *json;	
	char *message;
	char *deviceToken;
	int badge;
	int rx;
	
	
	json=cJSON_Parse(jSONMessage);
	if (!json) {
		syslog (LOG_WARNING, "[APNS Process] Error before: [%s] Won't push this message",cJSON_GetErrorPtr());
		return 0;
	}
	else
	{
	int siz = cJSON_GetArraySize(json);
	cJSON* tocken =	cJSON_GetArrayItem (json, 0);
	cJSON* jMessage =  cJSON_GetArrayItem (json, 1);

	message = jMessage->valuestring;
	badge = 1;
	deviceToken = tocken->valuestring;
		syslog (LOG_NOTICE, "[APNS Process] ====TOCKEN====%s",deviceToken);
		syslog (LOG_NOTICE, "[APNS Process] ====message====%s",message);

	Payload *payload = (Payload*)malloc(sizeof(Payload));
	init_payload(payload);
	payload->message = message;
	payload->badgeNumber =badge;
	payload->actionKeyCaption = "Caption of the second Button";
	
	syslog (LOG_NOTICE, "[APNS Process] Sending APN to Device with UDID: %s\n", deviceToken);
	rx = send_remote_notification(deviceToken, payload);
	
	cJSON_Delete(json);
	free(payload);

	return rx;
	}
}



void
     read_from_pipe (int file)
     {
       FILE *locStream;
       int c;
       

       locStream = fdopen (file, "r");
       if (!locStream){
       syslog (LOG_NOTICE,"[APNS Process]  No pipe");
       }
       
       
      /* char *s;
       while ((c = fgetc (locStream)) != EOF){
       	s = malloc(i);
		memcpy(s,&c,i);
		i++;
		}*/
		
		/*int i=0;
    	char buffer[4096 * sizeof(char)];    	
    	char byte;

    	while ((byte = fgetc (locStream)) != '\0') {
       	//syslog (LOG_ERR,"[APNS Process] %c", byte);
		
		buffer[i]= &byte;       	  
       	if (i<5)
			       	 syslog (LOG_ERR,"[APNS Process] -------%s", buffer);
		i++;
       	if (i>4095)
       	 syslog (LOG_ERR,"[APNS Process] APNS Too Long");
       	}
 
   		 buffer[i-1]='\0'; 
 		*/
 		
	 	
     }
     

int TestAPNSProcess(){


	/**********************************/
    /*create another child process for APNS listener */
      	pid_t APNSpid;
 	/**********************************/
      	
  

    APNSpid = fork ();
  
  if (APNSpid == (pid_t) 0)
    {
      
      /* This is the child process. */
      syslog (LOG_NOTICE, "[APNS Process] Waiting for APNS query");
      
	  while(1){
 	     	read_from_pipe (APNSpipe[0]);      
      }
      syslog (LOG_NOTICE, "[APNS Process] Closing APNS Process");
      return EXIT_SUCCESS;
    }
  else if (APNSpid < (pid_t) 0)
    {
      /* The fork failed. */
     syslog (LOG_NOTICE, "[APNS Process] Critical ! APNS Fork Failed !");
	 return EXIT_FAILURE;
    }
  else
    {
    	/* give hand back to main daemon*/
         return EXIT_SUCCESS;
    }
	return  0;
}	    


