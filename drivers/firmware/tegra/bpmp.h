/*
 * Copyright (c) 2013-2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DRIVERS_BPMP_H
#define _DRIVERS_BPMP_H

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <soc/tegra/tegra_bpmp.h>

#define NR_CHANNELS		12
#define MSG_SZ			32
#define MSG_DATA_SZ		24

#define NR_THREAD_CH		4

#define NR_MRQS			32
#define __MRQ_ATTRS		0xff000000
#define __MRQ_INDEX(id)		((id) & ~__MRQ_ATTRS)

struct fops_entry {
	char *name;
	const struct file_operations *fops;
	mode_t mode;
};

struct bpmp_cpuidle_state {
	int id;
	const char *name;
};

struct mb_data {
	int32_t code;
	int32_t flags;
	u8 data[MSG_DATA_SZ];
} __packed;

struct channel_data {
	struct mb_data *ib;
	struct mb_data *ob;
};

extern struct channel_data channel_area[NR_CHANNELS];

#ifdef CONFIG_DEBUG_FS
extern struct bpmp_cpuidle_state plat_cpuidle_state[];
#endif

extern struct device *device;
extern struct mutex bpmp_lock;
extern int connected;

int bpmp_clk_init(struct platform_device *pdev);
int bpmp_platdbg_init(struct dentry *root, struct platform_device *pdev);
int bpmp_mail_init(struct platform_device *pdev);
int bpmp_get_fwtag(void);
int bpmp_init_modules(struct platform_device *pdev);
void bpmp_cleanup_modules(void);
int bpmp_create_attrs(const struct fops_entry *fent, struct dentry *parent,
		void *data);
int bpmp_attach(void);
void bpmp_detach(void);
int bpmp_mailman_init(void);
void bpmp_handle_mail(int mrq, int ch);

void bpmp_ring_doorbell(void);
uint32_t bpmp_mail_token(void);
void bpmp_mail_token_set(uint32_t val);
void bpmp_mail_token_clr(uint32_t val);
int bpmp_thread_ch_index(int ch);
int bpmp_ob_channel(void);
int bpmp_thread_ch(int idx);
int bpmp_init_irq(struct platform_device *pdev);
int bpmp_connect(void);
void bpmp_handle_irq(int ch);

/* should be called from non-preemptible context */
static inline int bpmp_post(int mrq, void *data, int sz)
{ return tegra_bpmp_send(mrq, data, sz); }

/* should be called from non-preemptible context */
static inline int bpmp_rpc(int mrq, void *ob_data, int ob_sz,
		void *ib_data, int ib_sz)
{ return tegra_bpmp_send_receive_atomic(mrq, ob_data, ob_sz, ib_data, ib_sz); }

/* should be called from sleepable context */
static inline int bpmp_threaded_rpc(int mrq, void *ob_data, int ob_sz,
		void *ib_data, int ib_sz)
{ return tegra_bpmp_send_receive(mrq, ob_data, ob_sz, ib_data, ib_sz); }

int __bpmp_rpc(int mrq, void *ob_data, int ob_sz, void *ib_data, int ib_sz);

#endif