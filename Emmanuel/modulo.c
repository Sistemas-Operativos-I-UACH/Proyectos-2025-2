#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Emmanuel Olivas");
MODULE_DESCRIPTION("Rickroll");

#define DEVICE_NAME "rick"
#define CLASS_NAME  "rickclass"
#define BUFFER_SIZE 1024

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class = NULL;
static struct device *my_device = NULL;

static char kernel_msg[] ="Hola\n";

static char write_buffer[BUFFER_SIZE];

static int stream_mode = 0;
static struct task_struct *stream_thread = NULL;

static int rick_open(struct inode *inode, struct file *file);
static int rick_release(struct inode *inode, struct file *file);
static ssize_t rick_read(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t rick_write(struct file *file, const char __user *buf, size_t len, loff_t *off);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = rick_open,
    .release = rick_release,
    .read = rick_read,
    .write = rick_write,
};

static int stream_fn(void *data)
{
    while (!kthread_should_stop()) {

        if (stream_mode) {
            printk(KERN_INFO
                   "Never gonna give you up, never gonna let you down\n"
                   "Never gonna run around and desert you\n"
                   "Never gonna make you cry, never gonna say goodbye\n"
                   "Never gonna tell a lie and hurt you\n");
        }

        msleep(1000);
    }
    return 0;
}

static int rick_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Rick: dispositivo abierto.\n");
    return 0;
}

static int rick_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Rick: dispositivo cerrado.\n");
    return 0;
}

static ssize_t rick_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    ssize_t msg_len = strlen(kernel_msg);
    return simple_read_from_buffer(buf, len, off, kernel_msg, msg_len);
}

static ssize_t rick_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    size_t copy_len = min(len, (size_t)BUFFER_SIZE - 1);

    if (copy_from_user(write_buffer, buf, copy_len) != 0)
        return -EFAULT;

    write_buffer[copy_len] = '\0';

    if (write_buffer[0] == '1') {
        stream_mode = 1;
        printk(KERN_INFO "Rick: modo flujo ACTIVADO.\n");
    }
    else if (write_buffer[0] == '0') {
        stream_mode = 0;
        printk(KERN_INFO "Rick: modo flujo DESACTIVADO.\n");
    }
    else {
        printk(KERN_INFO "Rick: recibido  → %s\n", write_buffer);
    }

    return copy_len;
}

static int __init rick_init(void)
{
    int ret;

    printk(KERN_INFO "Rick: cargando módulo...\n");

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "Rick: alloc_chrdev_region falló (%d)\n", ret);
        return ret;
    }

    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret) {
        printk(KERN_ERR "Rick: cdev_add falló (%d)\n", ret);
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        printk(KERN_ERR "Rick: class_create falló\n");
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(my_class);
    }

    my_device = device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        printk(KERN_ERR "Rick: device_create falló\n");
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(my_device);
    }

    stream_thread = kthread_run(stream_fn, NULL, "Rick_stream_thread");

    printk(KERN_INFO "Rick: módulo listo. Dispositivo creado en /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit rick_exit(void)
{
    printk(KERN_INFO "Rick: descargando módulo...\n");

    if (stream_thread)
        kthread_stop(stream_thread);

    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "Rick: módulo removido.\n");
}

module_init(rick_init);
module_exit(rick_exit);
