// Actividad: Modulo de Kernel Linux
// Integrantes del equipo:
// - 360249 Adrian Ricardo Galicia Gutierrez
// - 367910 Juan David Rocha Montelongo
// Fecha de la elaboracion: 26/11/2025

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Proyecto Rocha");
MODULE_DESCRIPTION("Modulo sencillo con flujo, bitacora y dispositivo");
MODULE_VERSION("1.0");

static int modo_flujo = 0;  // 0 por defecto (apagado)
static char mensaje[128] = "Hola desde el modulo Rocha\n";

/* write: recibe cadenas desde espacio de usuario */
static ssize_t rocha_write(struct file *file, const char __user *buf,
                           size_t len, loff_t *off)
{
    char kbuf[128];
    size_t to_copy = min(len, sizeof(kbuf) - 1);

    if (copy_from_user(kbuf, buf, to_copy))
        return -EFAULT;

    kbuf[to_copy] = '\0';

    pr_info("rocha_module: recibido desde usuario: '%s'\n", kbuf);

    if (strcmp(kbuf, "1") == 0) {
        modo_flujo = 1;
        pr_info("rocha_module: modo flujo ACTIVADO\n");

        /* modo flujo sin hilo — ciclo bloqueante hasta CTRL+C */
        while (modo_flujo) {
            pr_info("rocha_module: %s", mensaje);
            msleep(1000);
            if (signal_pending(current))  // CTRL-C desde user
                break;
        }
    }
    else if (strcmp(kbuf, "0") == 0) {
        modo_flujo = 0;
        pr_info("rocha_module: modo flujo DESACTIVADO\n");
    }
    else {
        /* actualizar el mensaje */
        strncpy(mensaje, kbuf, sizeof(mensaje));
        mensaje[sizeof(mensaje)-1] = '\0';
        pr_info("rocha_module: mensaje cambiado a: %s\n", mensaje);
    }

    return to_copy;
}

/* read: muestra el estado */
static ssize_t rocha_read(struct file *file, char __user *buf,
                          size_t len, loff_t *off)
{
    char estado[64];
    int slen;

    slen = snprintf(estado, sizeof(estado), "modo_flujo=%d\n", modo_flujo);

    if (*off >= slen)
        return 0;

    if (copy_to_user(buf, estado, slen))
        return -EFAULT;

    *off = slen;
    return slen;
}

static const struct file_operations rocha_fops = {
    .owner  = THIS_MODULE,
    .write  = rocha_write,
    .read   = rocha_read,
};

/* misc device — crea /dev/rocha_device automáticamente */
static struct miscdevice rocha_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "rocha_device",
    .fops  = &rocha_fops,
};

static int __init rocha_init(void)
{
    int ret;

    pr_info("rocha_module: iniciando modulo...\n");

    ret = misc_register(&rocha_device);
    if (ret) {
        pr_err("rocha_module: error al registrar dispositivo\n");
        return ret;
    }

    pr_info("rocha_module: dispositivo creado en /dev/rocha_device\n");
    pr_info("rocha_module: listo (modo flujo = 0)\n");
    return 0;
}

static void __exit rocha_exit(void)
{
    pr_info("rocha_module: deteniendo modulo...\n");
    misc_deregister(&rocha_device);
}

module_init(rocha_init);
module_exit(rocha_exit);
