#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define HELLO_MAJOR_NUM 290
#define HELLO_NAME  "hello"

static char array[2000] = {0,}; //Global Variable

static long hello_ioctl (struct file *flip, unsigned int cmd, unsigned long arg){
    int cnt = 0; int k = 0;
    if(_IOC_TYPE (cmd) != 0x55){printk("Wrong Magic#!\n"); return -1;}
    switch (_IOC_NR(cmd)){
        case 99: printk("IOCTL Size-> %d\n", _IOC_SIZE(cmd));
        copy_from_user(array,arg,_IOC_SIZE(cmd));
        break;

        case 98: printk("IOCTL Size-> %d\n", _IOC_SIZE(cmd));
        copy_from_user(&cnt,arg,_IOC_SIZE(cmd));
        for(k=0; k<cnt; k++) printk("%c", array[k]); printk("\n"); break;

        default: break;
    }
}


static int hello_open(struct inode *node, struct file *pfile){
    printk("hello_open enter\n");
    return 0;
}

static ssize_t hello_read(struct file *pfile, char __user *pBuff, size_t size, loff_t *filepos){
    char arrData[4] = {'1','2','3','4'};
    printk("hello_read enter\n");
    if(size >= 4){
        copy_to_user(pBuff, arrData, 4);
        return 4;
    }
    return 0;
}

static ssize_t hello_write(struct file pfile, const char __user *pBuff, size_t size, loff_t *filepos){
    char arrData[4];
    printk("hello_write enter\n");
    if(size >= 4){
        copy_from_user(arrData, pBuff, 4);
        return 4;
    }
    return 0;
}

static int hello_release(struct inode *node, struct file *pfile){
    printk("hello_release enter\n");
    return 0;
}

static const struct file_operations hello_fops = {
        .owner = THIS_MODULE,
        .open = hello_open,
        .read = hello_read,
        .write = hello_write,
        .release = hello_release,
        .unlocked_ioctl = hello_ioctl,
};

int flagRegisterSuccess = 0;

int init_hello(void){
    int ret;
    printk("Hello world.\n");
    ret = register_chrdev(HELLO_MAJOR_NUM, HELLO_NAME, &hello_fops);
    if(ret < 0){
        printk("Hello registr_chrdev fail\n");
        return ret;
    }
    flagRegisterSuccess = 1;
    return 0;
}

void exit_hello(void){
    if(flagRegisterSuccess){
        unregister_chrdev(HELLO_MAJOR_NUM, HELLO_NAME);
    }
    printk("Good-bye~ \n");
}

module_init(init_hello);
module_exit(exit_hello);

MODULE_LICENSE("GPL");