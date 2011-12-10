#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <beanstalkclient.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "Helper/GoogleClientLogin.h"
#include "Helper/RemoteNotification.h"
#include "Helper/iniparser.h"
#include "pushtest.h"
#include "Helper/global.h"
#include "listener.h"


char spawn_cmd[200], kill_cmd[200];
bool bury_flag;


void 
write_to_pipe (char *stringToSend)
{
    
  /*  if (pipe (APNSpipe))
    {
      syslog (LOG_NOTICE, "[Main Process] ERR Create pipe APNS failed.\n");
    }
    
	stream = fdopen (APNSpipe[1], "w");
	
   if (!stream)
   {
    syslog (LOG_CRIT,"[Main Process] ERR  We have no stream to write to pipe"); 

	return;
   }
  syslog (LOG_NOTICE, " =====>Sending string to other process=======");
  fprintf (stream, stringToSend);
  fclose(stream);
*/

  return;
}

int main(int argc, char **argv)
{
	/*before we fork we need to check config file*/
	dictionary	*	ini ;
	char *s;
	char *tube;
	int dev;

	if (argc<2) {
        fprintf(stderr, "Usage: PMEngine <config file>\n", ini_name);
		return -1 ;

	} else {
		ini_name = argv[1] ;
	}

	ini = iniparser_load(ini_name);
	if (ini==NULL) {
		fprintf(stderr, "Cannot parse config file: %s\n", ini_name);
		return -1 ;
	}
	//iniparser_dump(ini, stderr);
	s = iniparser_getstring(ini, "server:host", NULL);
	host = s ? s :"localhost";
	
	if (!s)
		fprintf (stderr, "Warning inifile : Need host in server section, will use default\n");

	s = iniparser_getstring(ini, "server:port", NULL);
	port = s ? s : "11300";
	
	if (!s)
		fprintf (stderr, "Warning inifile : Need port in server section, will use default\n");
	
	s = iniparser_getstring(ini, "server:tube", NULL);
	tube = s ? s : "default";
	
	if (!s)
		fprintf (stderr, "Warning inifile : Need tube in server section, will use default\n");
	
	
	
	
	/* INI APNS VALUES */
	
	globalApns = (Apns*)malloc(sizeof(Apns));
	init_apns(globalApns);
	
	dev=iniparser_getint( ini,"APNS:dev-mode", 0 );
	
	if (dev)
	 globalApns->host = APPLE_SANBOX_HOST;
	else
	 globalApns->host = APPLE_RELEASE_HOST;
	 
	s = iniparser_getstring(ini, "APNS:rsa_key", NULL);
	
	if (!s) {
	   fprintf (stderr, "Failed to read ini file : No rsa key defined in [APNS]\n");
	   exit(0);
	  }
	  
	globalApns->rsa_key=s;
	  
	s = iniparser_getstring(ini, "APNS:rsa_cert", NULL);
	
	if (!s) {
	   fprintf (stderr, "Failed to read ini file : No rsa cert defined in [APNS]\n");
	   exit(0);
	  }
	  
	 globalApns->rsa_cert=s;
	 globalApns->port = APPLE_PORT;
	  
	  
 /*  */
	googleTockeniD = getAuthTocken ("", "", "PMEngine" );
	
	syslog (LOG_NOTICE, "[Google Process] %s", googleTockeniD);
	
    syslog (LOG_NOTICE, "[Main Process] Contacting host %s port %s tube %s", host, port, tube);

  //*//   
   
   
   listenToTube(tube); //This will loop forever
    

	syslog (LOG_CRIT, "[Main Process]  Stoping engine");
    closelog();
    exit(EXIT_SUCCESS);

}

