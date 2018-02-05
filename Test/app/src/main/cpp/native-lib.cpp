#include <jni.h>
#include <string>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "log.h"

#define PATH "/data/data/com.jared.jnidaemon/app_socket/localsocket"

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
    int ret = connect(connect_fd, (struct sockaddr*) &address, sizeof(address));
    if (ret == -1) {
        LOG_D("cannot connect to the server");
        close(connect_fd);
        return 1;
    }
    char snd_buf[1024];
    memset(snd_buf, 0, 1024);
    strcpy(snd_buf, "message from client 2222");
    //send info server
    for (int i = 0; i < 4; i++)
        write(connect_fd, snd_buf, sizeof(snd_buf));
    close(connect_fd);
    return 0;
}

extern "C"
JNIEXPORT jstring stringFromJNI(JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    socket();

    return env->NewStringUTF(hello.c_str());
}


//参数映射表
static JNINativeMethod methods[] = {
        {"stringFromJNI",   "()Ljava/lang/String;", reinterpret_cast<void *>(stringFromJNI)},
};

//为某一个类注册本地方法，调运JNI注册方法
extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (JNI_OK != vm->GetEnv(reinterpret_cast<void **> (&env), JNI_VERSION_1_4)) {
        LOG_W("JNI_OnLoad could not get JNI env");
        return JNI_ERR;
    }

    jclass clazz = env->FindClass("com/jkrobot/test/MainActivity");  //获取Java

    //注册Native方法
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof((methods)[0])) < 0) {
        LOG_W("RegisterNatives error");
        return JNI_ERR;
    }

    return JNI_VERSION_1_4;
}