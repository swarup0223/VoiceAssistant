#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "voiceassistant"
#define CLASS_NAME  "voiceassistant"
#define BUFFER_SIZE 256

static dev_t voiceassistant_dev;
static struct cdev voiceassistant_cdev;
static struct class *voiceassistant_class;
static struct device *voiceassistant_device;

static char device_buffer[BUFFER_SIZE];
static size_t buffer_size;
static DEFINE_MUTEX(voiceassistant_mutex);

static int voiceassistant_open(struct inode *inode, struct file *file)
{
    pr_info("voiceassistant: device opened\n");
    return 0;
}

static int voiceassistant_release(struct inode *inode, struct file *file)
{
    pr_info("voiceassistant: device closed\n");
    return 0;
}

static ssize_t voiceassistant_read(
    struct file *file,
    char __user *user_buffer,
    size_t count,
    loff_t *offset)
{
    size_t bytes_to_copy;

    if (*offset >= buffer_size)
        return 0;

    mutex_lock(&voiceassistant_mutex);

    bytes_to_copy = min(count, buffer_size - (size_t)*offset);

    if (copy_to_user(user_buffer,
                     device_buffer + *offset,
                     bytes_to_copy)) {
        mutex_unlock(&voiceassistant_mutex);
        return -EFAULT;
    }

    *offset += bytes_to_copy;

    mutex_unlock(&voiceassistant_mutex);

    pr_info("voiceassistant: read %zu bytes\n", bytes_to_copy);

    return bytes_to_copy;
}

static ssize_t voiceassistant_write(
    struct file *file,
    const char __user *user_buffer,
    size_t count,
    loff_t *offset)
{
    size_t bytes_to_copy = min(count, (size_t)(BUFFER_SIZE - 1));

    mutex_lock(&voiceassistant_mutex);

    memset(device_buffer, 0, BUFFER_SIZE);

    if (copy_from_user(device_buffer,
                       user_buffer,
                       bytes_to_copy)) {
        mutex_unlock(&voiceassistant_mutex);
        return -EFAULT;
    }

    device_buffer[bytes_to_copy] = '\0';
    buffer_size = bytes_to_copy;

    mutex_unlock(&voiceassistant_mutex);

    pr_info("voiceassistant: received command: %s\n", device_buffer);

    return bytes_to_copy;
}

static const struct file_operations voiceassistant_fops = {
    .owner = THIS_MODULE,
    .open = voiceassistant_open,
    .release = voiceassistant_release,
    .read = voiceassistant_read,
    .write = voiceassistant_write,
};

static int __init voiceassistant_init(void)
{
    int ret;

    buffer_size = 0;
    memset(device_buffer, 0, BUFFER_SIZE);

    ret = alloc_chrdev_region(&voiceassistant_dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("voiceassistant: failed to allocate device number\n");
        return ret;
    }

    cdev_init(&voiceassistant_cdev, &voiceassistant_fops);
    voiceassistant_cdev.owner = THIS_MODULE;

    ret = cdev_add(&voiceassistant_cdev, voiceassistant_dev, 1);
    if (ret < 0) {
        pr_err("voiceassistant: failed to add cdev\n");
        unregister_chrdev_region(voiceassistant_dev, 1);
        return ret;
    }

    voiceassistant_class = class_create(CLASS_NAME);
    if (IS_ERR(voiceassistant_class)) {
        ret = PTR_ERR(voiceassistant_class);
        cdev_del(&voiceassistant_cdev);
        unregister_chrdev_region(voiceassistant_dev, 1);
        return ret;
    }

    voiceassistant_device = device_create(
        voiceassistant_class,
        NULL,
        voiceassistant_dev,
        NULL,
        DEVICE_NAME
    );

    if (IS_ERR(voiceassistant_device)) {
        ret = PTR_ERR(voiceassistant_device);
        class_destroy(voiceassistant_class);
        cdev_del(&voiceassistant_cdev);
        unregister_chrdev_region(voiceassistant_dev, 1);
        return ret;
    }

    pr_info(
        "voiceassistant: driver loaded, major=%d minor=%d\n",
        MAJOR(voiceassistant_dev),
        MINOR(voiceassistant_dev)
    );

    return 0;
}

static void __exit voiceassistant_exit(void)
{
    device_destroy(voiceassistant_class, voiceassistant_dev);
    class_destroy(voiceassistant_class);
    cdev_del(&voiceassistant_cdev);
    unregister_chrdev_region(voiceassistant_dev, 1);

    pr_info("voiceassistant: driver unloaded\n");
}

module_init(voiceassistant_init);
module_exit(voiceassistant_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VoiceAssistant Team");
MODULE_DESCRIPTION("VoiceAssistant Linux character device driver");
MODULE_VERSION("1.0");
