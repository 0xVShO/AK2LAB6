// SPDX-License-Identifier: GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/bug.h>
#include "hello1.h"

MODULE_AUTHOR("Vlad");
MODULE_DESCRIPTION("Hello base module with BUG_ON and kmalloc fail");
MODULE_LICENSE("GPL");

struct hello_time {
        struct list_head list;
        ktime_t start_time;
        ktime_t end_time;
};

static LIST_HEAD(hello_list);

int print_hello(unsigned int count)
{
        uint i;
        struct hello_time *ptr;
        struct hello_time *md, *tmp;

        BUG_ON(count > 10);

        if (count == 0 || (count >= 5 && count <= 10)) {
                pr_warn("Warning: count is %u (expected 1-4)\n", count);
        }

        for (i = 0; i < count; i++) {
                ptr = kmalloc(sizeof(*ptr), GFP_KERNEL);
                
                if (i == 4) {
                        ptr = NULL; 
                }

                if (!ptr) {
                        pr_err("Memory allocation failed on iteration %u\n", i);
                        list_for_each_entry_safe(md, tmp, &hello_list, list) {
                                list_del(&md->list);
                                kfree(md);
                        }
                        return -ENOMEM;
                }

                ptr->start_time = ktime_get();
                pr_info("Hello, world!\n");
                ptr->end_time = ktime_get();

                list_add_tail(&ptr->list, &hello_list);
        }

        return 0;
}
EXPORT_SYMBOL(print_hello);

static int __init hello1_init(void)
{
        pr_info("Module hello1 loaded\n");
        return 0;
}

static void __exit hello1_exit(void)
{
        struct hello_time *md, *tmp;

        list_for_each_entry_safe(md, tmp, &hello_list, list) {
                pr_info("Time spent: %lld ns\n", ktime_to_ns(ktime_sub(md->end_time, md->start_time)));
                list_del(&md->list);
                kfree(md);
        }
        pr_info("Module hello1 unloaded\n");
}

module_init(hello1_init);
module_exit(hello1_exit);
