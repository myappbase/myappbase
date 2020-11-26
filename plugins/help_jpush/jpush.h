#ifndef jpush__h
#define jpush__h

#include "cJSON/cJSON.h"

#define J_PUSH_APP_KEY "412695020059b393e94d544d" // AppKey
#define J_PUSH_SECRET "bef1a7e56cf86357a7683497" // Secret
#define J_PUSH_API "https://api.jpush.cn/v3/push" // push interface

#define J_PUSH_PLATFORM_IOS 1
#define J_PUSH_PLATFORM_ANDROID 2
#define J_PUSH_PLATFORM_WINPHONE 4

#define J_PUSH_NOTIFICATION_IOS 1
#define J_PUSH_NOTIFICATION_ANDROID 2
#define J_PUSH_NOTIFICATION_WINPHONE 4

#define J_PUSH_AUDIENCE_ALL 0
#define J_PUSH_AUDIENCE_TAG 1
#define J_PUSH_AUDIENCE_TAGAND 2
#define J_PUSH_AUDIENCE_ALIAS 3
#define J_PUSH_AUDIENCE_REG 4

#define J_PUSH_INT_NULL -999
#define J_PUSH_STRING_NULL NULL

// ios apns
typedef struct {
    int badge;
    int contentAvailable;
    int mutableContent;
    int builderId;
    int style;
    int alertType;
    int priority;
    char *alert;
    char *title;
    char *sound;
    char *category;
    char *bigText;
    char *bigPicPath;
    char *openPage;
    char *extras;
} Apns;

// motification
typedef struct {
    Apns *all;
    Apns *ios;
    Apns *android;
    Apns *win;
} JPushNotification;

#ifdef __cplusplus
extern "C"
{
#endif

extern int jpushInit(cJSON **, JPushNotification **);
extern int setPlatform(cJSON *, int);
extern int setAudienceTarget(cJSON *, int, int, char *[]);
extern void setAllApns(JPushNotification *, char *);
extern void setAndroidApns(JPushNotification *, char *, char *, int, int, char *, int, int, char *, char *, char *);
extern void setIosApns(JPushNotification *, char *, char *, int, int, int, char *, char *);
extern void setWinApns(JPushNotification *, char *, char *, char *, char *);
extern void setMessage(cJSON *, char *, char *, char *, char *);
extern void setSmsMessage(cJSON *, char *, int);
extern void setOptions(cJSON *, int, int, int, int, int);
extern int setNotification(cJSON *, JPushNotification *);
extern char *jpushEval(cJSON *);
extern int jpush(char *);
extern int jpushClean(cJSON *);

#ifdef __cplusplus
}
#endif

#endif
