// tort.c

//Archivos de Encabezado
#include <linux/module.h>   // Para módulos del kernel
#include <linux/kernel.h>   // Para printk y funciones del kernel
#include <linux/fs.h>       // Para operaciones de archivos (file_operations)
#include <linux/string.h>   // Para funciones de strings como strlen
#include <linux/cdev.h>     // Estructura cdev (Character Device)
#include <linux/uaccess.h>  // Acceso de usuario (copy to user)
#include <linux/device.h>   // Crear dispositivo
#include <linux/kdev_t.h>   // Device numbers: Macros para major y minor numbers

// Información del módulo (se muestra con modinfo)
MODULE_LICENSE("GPL");                          // Licencia GPL requerida
MODULE_AUTHOR("Equipo Tort");                // Autor del módulo
MODULE_DESCRIPTION("Modulo Tort - Sistemas Operativos I - Tercer Parcial");       // Descripción

//Titulos para mensaje de la bitacora
#define MSG_ERROR "Error Tort:"
#define MSG_TORT "Tort dice:"
#define MSG_USER "Tort recibio un mensaje del usuario:"

// Definir el nombre del dispositivo
#define DEVICE_NAME "tort"
#define CLASS_NAME  "tort-class"
// Variables globales del módulo
static dev_t deviceNumbers; //
static struct class *myClass;
static struct cdev myCdev;
static int flujoActivo = 0;   // Control del modo: 0=normal, 1=flujo continuo

//Prototipo de las File Operations
static int tort_open(struct inode *inode, struct file *file);
static int tort_release(struct inode *inode, struct file *file);
static ssize_t tort_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset);
static ssize_t tort_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset);

// Estructura que define las operaciones del dispositivo
// El kernel usa esta estructura para saber qué funciones llamar
static struct file_operations fops = {
    .open = tort_open,      // Función para abrir dispositivo
    .release = tort_release, // Función para cerrar dispositivo  
    .read = tort_read,      // Función para leer del dispositivo
    .write = tort_write,    // Función para escribir al dispositivo
};

// Función que se llama cuando se abre el dispositivo (ej: cat /dev/meny)
static int tort_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s Abriendo Dispositivo\n", MSG_TORT);
    return 0;  // Retorna 0 si se abrió correctamente
}

// Función que se llama cuando se cierra el dispositivo
static int tort_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s Cerrando Dispositivo\n", MSG_TORT);
    return 0;  // Retorna 0 si se cerró correctamente
}

// Función que se llama cuando se lee del dispositivo (ej: cat /dev/tort)
static ssize_t tort_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    // El mensaje que vamos a enviar al usuario
    // Paso 2: Mostrar mensaje en espacio de usuario
    const char *msg = "Tort dice: YA VALISTE TORTAS CARNAL, SACA LAS CARTERAS, AQUI NO MANDA BUKELE\n";
    int msg_len = strlen(msg);  // Longitud del mensaje
    
    if (flujoActivo) {
        // MODO 1 (FLUJO): Siempre envía el mensaje completo
        // copy_to_user copia datos del kernel al espacio del usuario
        if (copy_to_user(buffer, msg, min(len, (size_t)msg_len)) == 0) {
            return min(len, (size_t)msg_len);  // Retorna cuántos bytes se enviaron
        }
    } else {
        // MODO 0 (NORMAL): Solo envía el mensaje una vez
        // *offset lleva la cuenta de hasta dónde se ha leído
        if (*offset == 0) {
            // Primera lectura - enviar mensaje
            if (copy_to_user(buffer, msg, min(len, (size_t)msg_len)) == 0) {
                *offset += min(len, (size_t)msg_len);  // Actualizar posición de lectura
                return min(len, (size_t)msg_len);      // Retornar bytes enviados
            }
        }
    }
    return 0;  // Retorna 0 cuando no hay mas datos (fin de archivo)
}

// Funcion que se llama cuando se escribe al dispositivo (ej: echo "1" > /dev/tort)
static ssize_t tort_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    char kbuf[128];
    char cmd;

    // Leer solo el primer byte
    if (copy_from_user(&cmd, buffer, 1) != 0)
        return -EFAULT;

    // Detectar comandos: '1' o '0' incluso si vienen con '\n'
    if ((cmd == '1' || cmd == '0') && len <= 2) {
        if (cmd == '1') {
            flujoActivo = 1;
            pr_info("%s Modo Flujo Activado\n", MSG_TORT);
        } 
        else {
            flujoActivo = 0;
            pr_info("%s Modo Flujo Desactivado\n", MSG_TORT);
        }
        return len;
    }

    // Si NO es 1 o 0 → se trata como mensaje normal
    size_t n = len;
    if (n >= sizeof(kbuf))
        n = sizeof(kbuf) - 1;

    if (copy_from_user(kbuf, buffer, n) != 0)
        return -EFAULT;

    kbuf[n] = '\0';

    pr_info("%s %s", MSG_USER, kbuf);

    return len;
}



// Función que se ejecuta al cargar el módulo (insmod)
static int __init tort_init(void) {
    printk(KERN_INFO "%s Insertando mi modulo\n", MSG_TORT);

    //Asignar el major y minor number
    if (alloc_chrdev_region(&deviceNumbers, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ERR "%s No se pudo obtener los numeros del dispositivo\n", MSG_ERROR);
        return -1;
    }
    //Crear clase del dispositivo
    if ((myClass = class_create(CLASS_NAME)) == NULL) {
        printk(KERN_ERR "%s No se pudo crear la clase del dispositivo\n", MSG_ERROR);
        goto ClassError;
    }

    // Crear archivo de dispositivo
    if (device_create(myClass, NULL, deviceNumbers, NULL, DEVICE_NAME) == NULL) {
        printk(KERN_ERR "%s No se pudo crear el dispositivo\n", MSG_ERROR);
        goto FileError;
    }

    //Inicializar archivo de dispositivo
    cdev_init(&myCdev, &fops);

    //Registrar dispositivo en el kernel
    if (cdev_add(&myCdev, deviceNumbers, 1) == -1) {
        
        printk(KERN_ERR "%s No se pudo registrar el dispositivo en el kernel\n", MSG_ERROR);
        goto AddError;
    }


    printk(
        KERN_INFO 
        "%s Modulo insertado con exito - Major Number: %d - Minor Number: %d\n",
        MSG_TORT, MAJOR(deviceNumbers), MINOR(deviceNumbers)
    );
    return 0;

    /* Error Handling */
    AddError:
        device_destroy(myClass, deviceNumbers);
    FileError:
        class_destroy(myClass);
    ClassError:
        unregister_chrdev_region(deviceNumbers, 1);
        return -1;
}

// Función que se ejecuta al descargar el módulo (rmmod)
static void __exit tort_exit(void) {
    cdev_del(&myCdev);
    device_destroy(myClass, deviceNumbers);
    class_destroy(myClass);
    unregister_chrdev_region(deviceNumbers, 1);
    printk(KERN_INFO "%s Fuga la oruga...\n", MSG_TORT);
}

// Especificar qué función es de inicialización y cuál de limpieza
module_init(tort_init);  //  se ejecuta al cargar
module_exit(tort_exit);  // tort_exit se ejecuta al descargar
