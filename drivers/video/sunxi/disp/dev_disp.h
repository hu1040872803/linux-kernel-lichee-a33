#ifndef __DEV_DISP_H__
#define __DEV_DISP_H__

#include "drv_disp_i.h"

#ifdef CONFIG_FB_SUNXI_UMP
#include <ump/ump_kernel_interface_ref_drv.h>
#endif

#define FB_RESERVED_MEM

#define DISPLAY_NORMAL 0
#define DISPLAY_LIGHT_SLEEP 1
#define DISPLAY_DEEP_SLEEP 2

#define SUNXI_MAX_FB 2

struct info_mm
{
	void *info_base;	/* Virtual address */
	unsigned long mem_start;	/* Start of frame buffer mem */
				/* (physical address) */
	u32 mem_len;			/* Length of frame buffer mem */
};

struct proc_list
{
	void (*proc)(u32 screen_id);
	struct list_head list;
};

struct ioctl_list
{
	unsigned int cmd;
	int (*func)(unsigned int cmd, unsigned long arg);
	struct list_head list;
};

struct standby_cb_list
{
	int (*suspend)(void);
	int (*resume)(void);
	struct list_head list;
};

typedef struct
{
	struct device           *dev;
	u32                     reg_base[DISP_MOD_NUM];
	u32                     reg_size[DISP_MOD_NUM];
	u32                     irq_no[DISP_MOD_NUM];

	disp_init_para          disp_init;

	bool                    fb_enable[FB_MAX];
	disp_fb_mode            fb_mode[FB_MAX];
	u32                     layer_hdl[FB_MAX][3];//[fb_id][0]:screen0 layer handle;[fb_id][1]:screen1 layer handle
	struct fb_info *        fbinfo[FB_MAX];
	disp_fb_create_info fb_para[FB_MAX];
	u32                     pseudo_palette [FB_MAX][16];
	wait_queue_head_t       wait[3];
	unsigned long           wait_count[3];
	struct work_struct      vsync_work[3];
	struct work_struct      resume_work[3];
	struct work_struct      start_work;
	ktime_t                 vsync_timestamp[3];

	int                     blank[3];

	struct proc_list        sync_proc_list;
	struct proc_list        sync_finish_proc_list;
	struct ioctl_list       ioctl_extend_list;
	struct standby_cb_list  stb_cb_list;
	struct mutex            mlock;

#ifdef CONFIG_FB_SUNXI_UMP
	ump_dd_handle ump_wrapped_buffer[SUNXI_MAX_FB][3];
#endif


}fb_info_t;

typedef struct
{
	u32    		        exit_mode;//0:clean all  1:disable interrupt
	bool			        b_lcd_enabled[3];
}disp_drv_info;

struct sunxi_disp_mod {
	disp_mod_id id;
	char name[32];
};

#define DISP_RESOURCE(res_name, res_start, res_end, res_flags) \
{\
	.start = (int __force)res_start, \
	.end = (int __force)res_end, \
	.flags = res_flags, \
	.name = #res_name \
},

int disp_open(struct inode *inode, struct file *file);
int disp_release(struct inode *inode, struct file *file);
ssize_t disp_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
ssize_t disp_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
int disp_mmap(struct file *file, struct vm_area_struct * vma);
long disp_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

s32 disp_create_heap(u32 pHeapHead, u32 pHeapHeadPhy, u32 nHeapSize);
void *disp_malloc(u32 num_bytes, u32 *phy_addr);
void  disp_free(void *virt_addr, void* phy_addr, u32 num_bytes);

extern s32 Display_Fb_Request(u32 fb_id, disp_fb_create_info *fb_para);
extern s32 Display_Fb_Release(u32 fb_id);
extern s32 Display_Fb_get_para(u32 fb_id, disp_fb_create_info *fb_para);
extern s32 Display_get_disp_init_para(disp_init_para * init_para);

extern s32 DRV_disp_vsync_event(u32 sel);
extern s32 capture_event(u32 sel);
extern s32 DRV_disp_take_effect_event(u32 sel);
extern s32 disp_register_sync_proc(void (*proc)(u32));
extern s32 disp_unregister_sync_proc(void (*proc)(u32));
extern s32 disp_register_sync_finish_proc(void (*proc)(u32));
extern s32 disp_unregister_sync_finish_proc(void (*proc)(u32));
extern s32 disp_register_ioctl_func(unsigned int cmd, int (*proc)(unsigned int cmd, unsigned long arg));
extern s32 disp_unregister_ioctl_func(unsigned int cmd);
extern s32 disp_register_standby_func(int (*suspend)(void), int (*resume)(void));
extern s32 disp_unregister_standby_func(int (*suspend)(void), int (*resume)(void));

extern s32 DRV_DISP_Init(void);
extern s32 DRV_DISP_Exit(void);

extern fb_info_t g_fbi;

extern disp_drv_info    g_disp_drv;

extern s32 DRV_lcd_open(u32 sel);
extern s32 DRV_lcd_close(u32 sel);
extern s32 Fb_Init(void);
extern s32 Fb_Exit(void);

extern int dispc_blank(int disp, int blank);

/* composer */
#if defined(CONFIG_ARCH_SUN6I) || defined(CONFIG_ARCH_SUN8IW3P1) ||defined(CONFIG_ARCH_SUN8IW5P1) || defined(CONFIG_ARCH_SUN9IW1P1)
extern s32 composer_init(void);
#endif

#ifdef CONFIG_FB_SUNXI_UMP
extern int (*disp_get_ump_secure_id) (struct fb_info *info, fb_info_t *g_fbi,
				      unsigned long arg, int buf);
#endif

#endif
