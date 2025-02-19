#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define DRIVER_NAME "DriverFran"
#define DRIVER_CLASS "DriverFranClass"
#define NUM_DEVICES 3

static dev_t major_minor = -1;
static struct cdev FranCdev[NUM_DEVICES];
static struct class *FranClass = NULL;

// FUNCIONES DE OPERACIONES MATEMÁTICAS (DE CADA DRIVER)
static void pitagoras(const char __user *user_buffer);
static void euclidesPrimo(const char __user *user_buffer);
static void euclidesDivisible(const char __user *user_buffer);

// FUNCIÓN AUXILIAR
static int esPrimo(int num);

// FILE_OPERATIONS
static int FranOpen(struct inode *inode, struct file *file);
static ssize_t FranWrite(struct file *file, const char __user *user_buffer, size_t count, loff_t *f_pos);
static int FranRelease(struct inode *inode, struct file *file);

static int FranOpen(struct inode *inode, struct file *file) {
	pr_info("DriverFran: Dispositivo abierto\n");
	return 0;
}

static ssize_t FranWrite(struct file *file, const char __user *user_buffer, size_t count, loff_t *f_pos) {
	int minor = MINOR(file_inode(file)->i_rdev);

	if (minor < 0 || minor >= NUM_DEVICES) {
		return -EINVAL;
	}

	switch (minor) {
		case 0:
			pitagoras(user_buffer);
			break;
		case 1:
			euclidesPrimo(user_buffer);
			break;
		case 2:
			euclidesDivisible(user_buffer);
			break;
		default:
			pr_err("Dispositivo inválido\n");
			return -EINVAL;
	}

	return count;
}

static int FranRelease(struct inode *inode, struct file *file) {
	pr_info("DriverFran: Dispositivo cerrado\n");
	return 0;
}

static const struct file_operations FranFops = {
	.owner = THIS_MODULE,
	.open = FranOpen,
	.write = FranWrite,
	.release = FranRelease,
};

static int __init init_driver(void) {
	int n_device;
	dev_t id_device;

	if (alloc_chrdev_region(&major_minor, 0, NUM_DEVICES, DRIVER_NAME) < 0) {
		pr_err("Falló la asignación del número mayor\n");
		return -1;
	}

	if ((FranClass = class_create(DRIVER_CLASS)) == NULL) {
		unregister_chrdev_region(major_minor, NUM_DEVICES);
		pr_err("Falló la creación de la clase\n");
		return -1;
	}

	for (n_device = 0; n_device < NUM_DEVICES; n_device++) {
		cdev_init(&FranCdev[n_device], &FranFops);
		id_device = MKDEV(MAJOR(major_minor), MINOR(major_minor) + n_device);

		if (cdev_add(&FranCdev[n_device], id_device, 1) == -1) {
			pr_err("Falló la creación del nodo de dispositivo\n");
			goto error;
		}

		if (device_create(FranClass, NULL, id_device, NULL, DRIVER_NAME "%d", n_device) == NULL) {
			pr_err("Falló la creación del dispositivo\n");
			goto error;
		}
	}

	pr_info("DriverFran: Inicializado y cargado\n");
	return 0;

error:
	for (int j = 0; j < n_device; j++) {
		device_destroy(FranClass, MKDEV(MAJOR(major_minor), MINOR(major_minor) + j));
		cdev_del(&FranCdev[j]);
	}

	class_destroy(FranClass);
	unregister_chrdev_region(major_minor, NUM_DEVICES);
	return -1;
}

static void __exit exit_driver(void) {
	int n_device;

	for (n_device = 0; n_device < NUM_DEVICES; n_device++) {
		device_destroy(FranClass, MKDEV(MAJOR(major_minor), MINOR(major_minor) + n_device));
		cdev_del(&FranCdev[n_device]);
	}

	class_destroy(FranClass);
	unregister_chrdev_region(major_minor, NUM_DEVICES);
	pr_info("DriverFran: Descargado\n");
}

static int esPrimo(int num) {
	if (num < 2) {
		return 0;
	}

	for (int i = 2; i * i <= num; i++) {
		if (num % i == 0) {
			return 0;
		}
	}

	return 1;
}

// FUNCIONES MATEMÁTICAS
static void pitagoras(const char __user *user_buffer) {
    int a, b, c;
    char clean_buffer[256];
    char extra;
    int i = 0;

    while (user_buffer[i] != '\0' && i < sizeof(clean_buffer) - 1) {
        if (user_buffer[i] != '\n' && user_buffer[i] != '\r') {
            clean_buffer[i] = user_buffer[i];
        } else {
            break;
        }
        i++;
    }
    
    clean_buffer[i] = '\0';

    int result = sscanf(clean_buffer, "%d %d %d %c", &a, &b, &c, &extra);

    if (result != 3) {
        pr_info("ERROR EN DriverFran0: SE NECESITAN EXACTAMENTE 3 NÚMEROS\n");
        return;
    }

    if (c * c == a * a + b * b) {
        pr_info("%d, %d y %d CUMPLEN el Teorema de Pitágoras\n", a, b, c);
    } else {
        pr_info("%d, %d y %d NO CUMPLEN el Teorema de Pitágoras\n", a, b, c);
    }
}

static void euclidesPrimo(const char __user *user_buffer) {
    int num;
    char clean_buffer[256];
    char extra;
    int i = 0;

    while (user_buffer[i] != '\0' && i < sizeof(clean_buffer) - 1) {
        if (user_buffer[i] != '\n' && user_buffer[i] != '\r') {
            clean_buffer[i] = user_buffer[i];
        } else {
            break;
        }
        i++;
    }
    
    clean_buffer[i] = '\0';

    int result = sscanf(clean_buffer, "%d %c", &num, &extra);

    if (result != 1) {
        pr_info("ERROR EN DriverFran1: SE NECESITA EXACTAMENTE 1 NÚMERO\n");
        return;
    }

    if (esPrimo(num)) {
        pr_info("%d ES PRIMO según el Teorema de Euclides\n", num);
    } else {
        pr_info("%d NO ES PRIMO según el Teorema de Euclides\n", num);
    }
}

static void euclidesDivisible(const char __user *user_buffer) {
    int a, b;
    char clean_buffer[256];
    char extra;
    int i = 0;

    while (user_buffer[i] != '\0' && i < sizeof(clean_buffer) - 1) {
        if (user_buffer[i] != '\n' && user_buffer[i] != '\r') {
            clean_buffer[i] = user_buffer[i];
        } else {
            break;
        }
        i++;
    }
    
    clean_buffer[i] = '\0';

    int result = sscanf(clean_buffer, "%d %d %c", &a, &b, &extra);

    if (result != 2) {
        pr_info("ERROR EN DriverFran2: SE NECESITAN EXACTAMENTE 2 NÚMEROS\n");
        return;
    }

    if (b != 0 && a % b == 0) {
        pr_info("%d ES DIVISIBLE por %d según el Teorema de Euclides\n", a, b);
    } else {
        pr_info("%d NO ES DIVISIBLE por %d según el Teorema de Euclides\n", a, b);
    }
}

MODULE_LICENSE("GPL");
module_init(init_driver);
module_exit(exit_driver);
