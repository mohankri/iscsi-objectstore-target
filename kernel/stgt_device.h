/*
 * STGT device
 *
 * (C) 2005 FUJITA Tomonori <tomof@acm.org>
 * (C) 2005 Mike Christie <michaelc@cs.wisc.edu>
 * This code is licenced under the GPL.
 */
#ifndef __SCSI_STGT_DEVICE_H
#define __SCSI_STGT_DEVICE_H

#include <linux/device.h>
#include <linux/list.h>

struct stgt_device;
struct stgt_cmnd;

struct stgt_device_template {
	const char *name;
	struct module *module;
	unsigned priv_data_size;

	int (* create)(struct stgt_device *);
	void (* destroy)(struct stgt_device *);
	int (* queuecommand)(struct stgt_device *device, struct stgt_cmnd *cmd);
	int (* prepcommand)(struct stgt_device *device, struct stgt_cmnd *cmn);

	/*
	 * Pointer to the sysfs class properties for this host, NULL terminated.
	 */
	struct class_device_attribute **device_attrs;
};

struct stgt_device {
	struct stgt_device_template *sdt;
	void *sdt_data;

	struct class_device cdev;

        char *path;
        uint32_t lun;
        uint32_t blk_shift;
        uint64_t blk_count;

        struct stgt_target *target;
        struct list_head dlist;
};

#define cdev_to_stgt_device(cdev) \
        container_of(cdev, struct stgt_device, cdev)

extern struct stgt_device* stgt_device_create(struct stgt_target *target,
					      char *device_type, char *path,
					      uint32_t lun,
					      unsigned long dflags);
extern int stgt_device_destroy(struct stgt_device *device);
extern int stgt_sysfs_register_device(struct stgt_device *device);
extern void stgt_sysfs_unregister_device(struct stgt_device *device);
extern int stgt_device_template_register(struct stgt_device_template *sdt);
extern void stgt_device_template_unregister(struct stgt_device_template *sdt);

#endif