#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

static long start = 0;

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};

/* This function is called when the module is loaded. */
int proc_init(void) {
        // creates the /proc/hello entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &proc_ops);
        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
        start = (long)jiffies;
	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {
    // removes the /proc/hello entry
    remove_proc_entry(PROC_NAME, NULL);
    printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/hello is read,
 * and is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 * params:
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;
        rv = sprintf(buffer, "In proc_read(), the Seconds is: %lu\n", (jiffies-start)/HZ);

        printk( KERN_INFO "In proc_read(), the Seconds is: %lu\n",(jiffies-start)/HZ);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("SGG");


// OUTPUT ===============================================================
//
//In proc_read(), the Seconds is: 3
//In proc_read(), the Seconds is: 6
//[ 2091.525499] /proc/seconds created
//[ 2094.531437] In proc_read(), the Seconds is: 3
//[ 2097.538229] In proc_read(), the Seconds is: 6
//[ 2097.559080] /proc/seconds removed
//
//...end of script

// END OF OUTPUT ==========================================================


// INSREM.SH ==============================================================
//
//#!/bin/bash
//
//# clear the message queue, then
//# insert simple.ko into the kernel and remove it again, then
//# display the messages as a result of doing so
//
//sudo -p "osc"
//sudo dmesg -c
//clear
//
//printf "beginning script...\n\n"
//sudo insmod seconds.ko
//sleep 3
//cat /proc/seconds
//sleep 3
//cat /proc/seconds
//sudo rmmod seconds
//sudo dmesg
//printf "\n...end of script\n\n"
//
// END OF INSREM.SH ==========================================================


// MAKEFILE ==================================================================
//
//obj-m += seconds.o
//all:
//	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
//clean:
//	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
//
// END OF MAKEFILE ============================================================


