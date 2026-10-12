#include <errno.h>
#include <fcntl.h>
#include <jni.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* S0226114 touch_fops: set takes a scalar, get takes an int pointer. */
#define SET_TOUCH_MODE 6001
#define GET_TOUCH_MODE 6004

JNIEXPORT jint JNICALL
Java_com_pantech_settings_device_TouchMode_read(JNIEnv *env, jclass clazz)
{
    (void)env;
    (void)clazz;
    int fd = open("/dev/touch_fops", O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return -errno;
    int mode;
    int result = ioctl(fd, GET_TOUCH_MODE, &mode);
    int error = errno;
    close(fd);
    if (result < 0)
        return -error;
    mode %= 10;
    /* Internal pen mode 3 uses the same configuration as menu pen mode 2. */
    return mode == 3 ? 2 : mode;
}

JNIEXPORT jint JNICALL
Java_com_pantech_settings_device_TouchMode_write(JNIEnv *env, jclass clazz, jint mode)
{
    (void)env;
    (void)clazz;
    if (mode < 0 || mode > 2)
        return -EINVAL;
    int fd = open("/dev/touch_fops", O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return -errno;
    int result = ioctl(fd, SET_TOUCH_MODE, (unsigned long)mode);
    int error = errno;
    close(fd);
    return result < 0 ? -error : 0;
}
