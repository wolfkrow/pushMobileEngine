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
#include "Helper/iniparser.h"
#include "pushtest.h"
#include "Helper/global.h"

static int reconnect_attempts = 5, finished = 0, put_finished;
char errorstr[BSC_ERRSTR_LEN];
static bsc_error_t bsc_error;
struct sigaction act;

static void reconnect(bsc *client, bsc_error_t error)
{
    char errorstr[BSC_ERRSTR_LEN];
    int i;

    
    switch (error) {
        case BSC_ERROR_INTERNAL:
            syslog (LOG_ERR, "[Main Process] critical error: recieved BSC_ERROR_INTERNAL, quitting\n");
            break;
        case BSC_ERROR_MEMORY:
            syslog (LOG_ERR, "[Main Process] critical error: recieved BSC_ERROR_MEMORY, quitting\n");
            break;
        case BSC_ERROR_SOCKET:
            syslog (LOG_ERR, "[Main Process] error: recieved BSC_ERROR_SOCKET, attempting to reconnect ...\n");
            for (i = 0; i < reconnect_attempts; ++i) {
                if ( bsc_reconnect(client, errorstr) ) {
                    printf("reconnect successful\n");
                    return;
                }
                else
                    syslog (LOG_ERR, "[Main Process]  error: reconnect attempt %d/%d - %s", i+1, reconnect_attempts, errorstr);
            }
            syslog (LOG_ERR, "[Main Process] critical error: maxed out reconnect attempts, quitting\n");
            break;
        default:
            syslog (LOG_ERR, "[Main Process] critical error: got unknown error (%d)\n", error);
    }
    exit(EXIT_FAILURE);
}


int client_poll(bsc *client, fd_set *readset, fd_set *writeset)
{
    FD_SET(client->fd, readset);
    FD_SET(client->fd, writeset);
    if (AQ_EMPTY(client->outq)) {
        if ( select(client->fd+1, readset, NULL, NULL, NULL) < 0) {
            syslog (LOG_ERR, "[Main Process]  critical error: select()");
            return EXIT_FAILURE;
        }
        if (FD_ISSET(client->fd, readset))
           {
            bsc_read(client);
            
            }
    }
    else {
        if ( select(client->fd+1, readset, writeset, NULL, NULL) < 0) {
            syslog (LOG_ERR, "[Main Process]  critical error: select()");
            return EXIT_FAILURE;
        }
        if (FD_ISSET(client->fd, readset))
            bsc_read(client);
        if (FD_ISSET(client->fd, writeset))
            bsc_write(client);
    }
}



void got_message_cb (bsc *client, struct bsc_reserve_info *info)
{
    syslog (LOG_DEBUG, "[Main Process]  got message cb");
}

void got_watch_cb (bsc *client, struct bsc_watch_info *info)
{
    syslog (LOG_DEBUG, "[Main Process]  got watch cb");

}

void ignore_cb(bsc *client, struct bsc_ignore_info *info)
{
    syslog (LOG_DEBUG, "[Main Process]  got ignore cb");
	
}

void bsc_delete_cb(bsc *client, struct bsc_delete_info *info){
    syslog (LOG_NOTICE, "[Main Process] DELETE job");

}

void reserve_cb(bsc *client, struct bsc_reserve_info *info)
{
    syslog (LOG_WARNING, "[Main Process] Reserved JOB %d %s", info->response.id , info->response.data);
	
	
	 int ret = PushMessage ( info->response.data );
   
   /***************************/
   
   if (ret){
    bsc_delete(client,
                       bsc_delete_cb ,
                       NULL,
                       info->response.id);
                       
    
    finished = true;
    }
    else
         syslog (LOG_WARNING, "[Main Process] Problem with push message, won't delete job");

}


void put_cb(bsc *client, struct bsc_put_info *info)
{
    put_finished = true;
}



void sighandler(int signum, siginfo_t *info, void *ptr)
{
    syslog (LOG_CRIT, "[Main Process] Will leave properly, signal %d received", signum);
    syslog (LOG_CRIT, "[Main Process] Signal originates from process %lu\n",
        (unsigned long)info->si_pid);
      closelog();
      free(googleTockeniD);
     //TODO : something else to free ?
     syslog (LOG_CRIT,"[Main Process]  Hasta la Vista Baby!"); 
     exit(EXIT_SUCCESS);

}




int listenToTube( char *tube){

    bsc *client;
    fd_set readset, writeset;

	  /* Our process ID and Session ID */
        pid_t pid, sid;
        
        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
                exit(EXIT_FAILURE);
        }
        /* If we got a good PID, then
           we can exit the parent process. */
        if (pid > 0) {
                exit(EXIT_SUCCESS);
        }

        /* Change the file mode mask */
        umask(0);
            
        
		/* Handler for cool end of life : SIGTERM */
    	memset(&act, 0, sizeof(act));
    	act.sa_sigaction = sighandler;
    	act.sa_flags = SA_SIGINFO;
    	sigaction(SIGTERM, &act, NULL);
    
     
       /* Open any logs here */      
        setlogmask (LOG_UPTO (LOG_NOTICE));
        openlog ("PMEngine",  LOG_CONS | LOG_PID | LOG_NDELAY, LOG_SYSLOG);       
		
		syslog (LOG_NOTICE, "[Main Process] Starting engine with config file %s…", ini_name);
        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }
                
        /* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);


 	 client = bsc_new(host, port, tube, reconnect , 16, 12, 4, errorstr);
              
    if(!client) {
        syslog (LOG_ERR, "[Main Process] Error, not able to contact server: %s", errorstr);
        return EXIT_FAILURE;
    }
  	
   		
	if (!bsc_connect(client, errorstr)){
        syslog (LOG_ERR, "[Main Process] Error, not able to connect to server %s", errorstr);
        return EXIT_FAILURE;
    }
	
	bsc_watch(client, got_watch_cb ,
                      NULL,
                      tube);
	syslog (LOG_WARNING, "[Main Process] Connected to beanstalkd, listening to tube : %s", tube);

	
	 bsc_error = bsc_ignore(client, ignore_cb, NULL, "default");
   		 if (bsc_error != BSC_ERROR_NONE){
        		syslog (LOG_ERR, "bsc_ignore failed (%d)", bsc_error );
    	   		 return EXIT_FAILURE;
       		}
 			

	while(1)
	{
		finished=false;
	 	bsc_error = bsc_reserve(client, reserve_cb, NULL, BSC_RESERVE_NO_TIMEOUT);
    	
    	if (bsc_error != BSC_ERROR_NONE){
        		syslog (LOG_ERR, "[Main Process]  bsc_reserve failed (%d)", bsc_error );
           		 return EXIT_FAILURE;
			}     
	 	FD_ZERO(&readset);
    	FD_ZERO(&writeset);


    while (!finished) {
        client_poll(client, &readset, &writeset); 
    }   
   
   
   }//loop forever   


}
