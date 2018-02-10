#include <jni.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>

#include "log.h"
#include "jksocket.h"

#define PATH "/data/data/com.jared.jksocket/app_socket/localsocket"

JavaVM *g_jvm;
static jobject sCallbacksObj = NULL;
static JNIEnv *sCallbackEnv = NULL;

static jmethodID onCallbackID = NULL;
jboolean EXIT_THREAD = 0;

static inline void CheckExceptions(JNIEnv *env, const char *methodName) {
    if (!env->ExceptionCheck()) {
        return;
    }

    LOG_E("An exception was thrown by '%s'.", methodName);
    env->ExceptionClear();
}

static bool IsValidCallbackThreadEnvOnly() {
    JNIEnv *env;
    if (JNI_OK != g_jvm->GetEnv(reinterpret_cast<void **> (&env), JNI_VERSION_1_4)) {
        LOG_W("JNI_OnLoad could not get JNI env");
        return JNI_ERR;
    }

    if (sCallbackEnv == NULL || sCallbackEnv != env) {
        LOG_E("CallbackThread check fail: env=%p, expected=%p", env, sCallbackEnv);
        return false;
    }

    return true;
}

static bool IsValidCallbackThread() {
    if (sCallbacksObj == NULL) {
        LOG_E("Attempt to use blocked, because it hasn't been initialized.");
        return false;
    }

    return IsValidCallbackThreadEnvOnly();
}

static void callback(int32_t type) {
    if (!IsValidCallbackThread()) {
        return;
    }

    sCallbackEnv->CallVoidMethod(
            sCallbacksObj,
            onCallbackID,
            type
    );

    CheckExceptions(sCallbackEnv, __FUNCTION__);
}

void init(JNIEnv *env, jobject instance) {
    if (sCallbacksObj == NULL) {
        sCallbacksObj = env->NewGlobalRef(instance);
    }
}

void classInit(JNIEnv *env) {

    jclass clazz = env->GetObjectClass(sCallbacksObj);

    // get references to the Java provider methods
    onCallbackID = env->GetMethodID(
            clazz,
            "onCallback",
            "(I)V"
    );
}

void clean_client() {
    for (int i = 0; i < CLIENT_NUM; ++i) {
        client_map[i] = -1;
    }
}

void cleanup(JNIEnv *env) {
    EXIT_THREAD = 1;
    if (sCallbacksObj != NULL) {
        env->DeleteGlobalRef(sCallbacksObj);
        sCallbacksObj = NULL;
    }

    clean_client();
}

int socket() {
    //creat unix socket
    int connect_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connect_fd < 0) {
        LOG_D("cannot create communication socket");
        return 1;
    }
    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, PATH);
    //connect server
    int ret = connect(connect_fd, (struct sockaddr *) &address, sizeof(address));
    if (ret == -1) {
        LOG_D("cannot connect to the server");
        close(connect_fd);
        return 1;
    }
    char snd_buf[1024];
    memset(snd_buf, 0, 1024);
    strcpy(snd_buf, "message from client");
    //send info server
    for (int i = 0; i < 4; i++)
        write(connect_fd, snd_buf, sizeof(snd_buf));
    close(connect_fd);
    return 0;
}

void *runMethod(void *args) {

    JavaVMAttachArgs jvmArgs = {
            JNI_VERSION_1_6,
            /* group */ NULL
    };

    jint attachResult = g_jvm->AttachCurrentThread(&sCallbackEnv, &jvmArgs);
    if (attachResult != 0) {
        LOG_E("Callback thread attachment error: %d", attachResult);
        return NULL;
    }

    while (true) {
        if (EXIT_THREAD != 0) break;
        sleep(3);
        callback(6);
        socket();
    }

    //Detach主线程
    if (g_jvm->DetachCurrentThread() != JNI_OK) {
        LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
        return NULL;
    }
    //退出进程
    pthread_exit(0);
}

int find_fd_by_id(JNIEnv *env, jint src) {
    int i = 0;
    for (i = 1; i < CLIENT_NUM; i++) {
        if (client_map[i] == 0) continue;
        if (client_map[i] == src)
            return i;
    }
    return -1;
}

int socketSend(JNIEnv *env, jobject obj, jint src, jint dest, jint cmd, jstring data) {
    char snd_buf[1024];
    memset(snd_buf, 0, 1024);

    int connect_fd = find_fd_by_id(env, src);

    //const char *str1 = env->GetStringUTFChars(src, NULL);
    //LOG_D("dest: %s, connect_id: %d", str1, connect_fd);

    const char *str = env->GetStringUTFChars(data, NULL);
    LOG_D("data: %s, connect_id: %d", str, connect_fd);

    strcpy(snd_buf, str);

    ssize_t ret = write(connect_fd, snd_buf, strlen(snd_buf));
    if (ret < 0) {
        LOG_E("%s: write error", __FUNCTION__);
        return -1;
    }
    return 0;
}

int create_socket() {
    //creat unix socket
    int connect_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connect_fd < 0) {
        LOG_D("cannot create communication socket");
        return -1;
    }
    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, PATH);
    //connect server
    int ret = connect(connect_fd, (struct sockaddr *) &address, sizeof(address));
    if (ret == -1) {
        LOG_D("cannot connect to the server");
        close(connect_fd);
        return -1;
    }

    return connect_fd;
}

void *processClient(void *args) {

    JavaVMAttachArgs jvmArgs = {
            JNI_VERSION_1_6,
            /* group */ NULL
    };

    jint attachResult = g_jvm->AttachCurrentThread(&sCallbackEnv, &jvmArgs);
    if (attachResult != 0) {
        LOG_E("Callback thread attachment error: %d", attachResult);
        return NULL;
    }

    //const char *str = sCallbackEnv->GetStringUTFChars(*id, NULL);//注释1
//    LOG_D("id: %s", client_map[0]);

    int client_fd = create_socket();
    if (client_fd <= 0 || client_fd > 100) goto end;
    LOG_D("id: %d", client_fd);

    client_map[client_fd] = client_map[0];
    ssize_t str_len;
    char message[1024];

    while (1) {
        if (EXIT_THREAD != 0) break;
        str_len = read(client_fd, message, sizeof(message) - 1);
        if (str_len == -1) {
            LOG_E("read error, fd: %d, id: %s", client_fd, (char *) client_map[client_fd]);
            break;
        }
        LOG_D("recv message, %s", message);
    }

    end:
    //Detach主线程
    if (g_jvm->DetachCurrentThread() != JNI_OK) {
        LOG_E("%s: DetachCurrentThread() failed", __FUNCTION__);
        return NULL;
    }
    client_map[client_fd] = NULL;
    //退出进程
    pthread_exit(0);
}

pthread_t pt;

void startThread() {
    //创建子线程
    pthread_create(&pt, NULL, runMethod, NULL);
}

pthread_t socket_pt;

int registerSocket(JNIEnv *env, jobject obj, jint id) {
    //创建子线程
    client_map[0] = id;
    if (find_fd_by_id(env, id) != -1) {
        LOG_E("%s: already registered", __FUNCTION__);
        return -1;
    }
    pthread_create(&socket_pt, NULL, processClient, NULL);
    return 0;
}

//参数映射表
static JNINativeMethod methods[] = {
        {"nativeInit",      "()V",                      reinterpret_cast<void *>(init)},
        {"nativeClassInit", "()V",                      reinterpret_cast<void *>(classInit)},
        {"nativeCleanup",   "()V",                      reinterpret_cast<void *>(cleanup)},

        {"registerSocket",  "(I)I",                     reinterpret_cast<void *>(registerSocket)},
        {"socketSend",      "(IIILjava/lang/String;)I", reinterpret_cast<void *>(socketSend)},
        {"startThread",     "()V",                      reinterpret_cast<void *>(startThread)}
};

//为某一个类注册本地方法，调运JNI注册方法
extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (JNI_OK != vm->GetEnv(reinterpret_cast<void **> (&env), JNI_VERSION_1_4)) {
        LOG_W("JNI_OnLoad could not get JNI env");
        return JNI_ERR;
    }

    g_jvm = vm; //用于后面获取JNIEnv
    jclass clazz = env->FindClass("com/jared/jksocket/library/JKSocketNative");  //获取Java

    //注册Native方法
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof((methods)[0])) < 0) {
        LOG_W("RegisterNatives error");
        return JNI_ERR;
    }

    return JNI_VERSION_1_4;
}
