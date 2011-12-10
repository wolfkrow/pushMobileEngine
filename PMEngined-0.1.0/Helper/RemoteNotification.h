#ifndef __REMOTE_NOTIFICATION_H
#define __REMOTE_NOTIFICATION_H

#include "SSLHelper.h"


    #define APPLE_SANBOX_HOST          "gateway.sandbox.push.apple.com"
    #define APPLE_PORT          2195

    #define APPLE_RELEASE_HOST          "gateway.push.apple.com"

#define DEVICE_BINARY_SIZE  32
#define MAXPAYLOAD_SIZE     256


typedef struct {
	char *host;
	int port;
    char *rsa_cert;
    char *rsa_key;
} Apns;


Apns *globalApns;


typedef struct {
    /* The Message that is displayed to the user */
    char *message;

    /* The name of the Sound which will be played back */
    char *soundName;

    /* The Number which is plastered over the icon, 0 disables it */
    int badgeNumber;

    /* The Caption of the Action Key the user needs to press to launch the Application */
    char *actionKeyCaption;

    /* Custom Message Dictionary, which is accessible from the Application */
    char* dictKey[5];
    char* dictValue[5];
} Payload;

/* Initialize the payload with zero values */
void init_payload(Payload *payload);

void init_apns(Apns *apns);


/* Send a Notification to a specified iPhone */
int send_remote_notification(const char *deviceTokenHex, Payload *payload);

#endif
