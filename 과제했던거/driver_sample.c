//13장 p.12
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

int __init init_hello(void) //__init: optional 쓰면 좋고 안써도 됨
{
    printk("Hello world.\n");
    return 0;
}

void __exit exit_hello(void) //__exit: optional
{
    printk("Good-bye~ \n");
}

module_init(init_hello);
module_exit(exit_hello);

MODULE_LICENSE("GPL");