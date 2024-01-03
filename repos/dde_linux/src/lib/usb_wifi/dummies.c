/*
 * \brief  Dummy definitions of Linux Kernel functions - handled manually
 * \author Josef Soentgen
 * \date   2022-02-09
 */

/*
 * Copyright (C) 2022 Genode Labs GmbH
 *
 * This file is distributed under the terms of the GNU General Public License
 * version 2.
 */

#include <lx_emul.h>


#include <linux/kobject.h>

int kobject_uevent(struct kobject * kobj,enum kobject_action action)
{
	lx_emul_trace(__func__);
	return 0;
}


#include <linux/irq.h>

void note_interrupt(struct irq_desc * desc,irqreturn_t action_ret)
{
	lx_emul_trace_and_stop(__func__);
}


#include <linux/syscore_ops.h>

void register_syscore_ops(struct syscore_ops * ops)
{
	lx_emul_trace(__func__);
}


#include <linux/preempt.h>

void migrate_enable(void)
{
	lx_emul_trace_and_stop(__func__);
}


void migrate_disable(void)
{
	lx_emul_trace_and_stop(__func__);
}


void synchronize_rcu_expedited(void)
{
	lx_emul_trace(__func__);
}


/*
 * linux/seq_file.h depends on user_namespace being defined, add
 * all dummies pulling in this header below here
 */


#include <linux/seq_file.h>


#include <linux/vt_kern.h>

void unblank_screen(void)
{
	lx_emul_trace_and_stop(__func__);
}


const unsigned long module_cert_size = 0;
const u8 system_certificate_list[] = { };
const unsigned long system_certificate_list_size = sizeof (system_certificate_list);

const u8 shipped_regdb_certs[] = { };
unsigned int shipped_regdb_certs_len = sizeof (shipped_regdb_certs);


#include <linux/filter.h>
#include <linux/jump_label.h> /* for DEFINE_STATIC_KEY_FALSE */

void bpf_prog_change_xdp(struct bpf_prog *prev_prog, struct bpf_prog *prog)
{
	lx_emul_trace(__func__);
}

DEFINE_STATIC_KEY_FALSE(bpf_stats_enabled_key);


asmlinkage __wsum csum_partial(const void * buff,int len,__wsum sum)
{
	lx_emul_trace_and_stop(__func__);
}


struct static_key_false init_on_alloc;


#include <linux/fs.h>

unsigned int get_next_ino(void)
{
	static unsigned int count = 0;
	return ++count;
}


#include <linux/netdevice.h>

int __init dev_proc_init(void)
{
	lx_emul_trace(__func__);
	return 0;
}


#include <linux/stringhash.h>

unsigned int full_name_hash(const void * salt,const char * name,unsigned int len)
{
	lx_emul_trace(__func__);
	return 0;
}


#include <linux/key.h>

static struct key _key;

struct key * keyring_alloc(const char * description,kuid_t uid,kgid_t gid,const struct cred * cred,key_perm_t perm,unsigned long flags,struct key_restriction * restrict_link,struct key * dest)
{
	lx_emul_trace(__func__);
	return &_key;
}


#include <linux/kobject.h>

int kobject_uevent_env(struct kobject * kobj,enum kobject_action action,char * envp_ext[])
{
	lx_emul_trace(__func__);
	return 0;
}


unsigned long lpj_fine = 0;


#include <linux/pid.h>

void put_pid(struct pid * pid)
{
	lx_emul_trace(__func__);
}


#include <linux/filter.h>

int sk_filter_trim_cap(struct sock * sk,struct sk_buff * skb,unsigned int cap)
{
	lx_emul_trace(__func__);
	return 0;
}


#include <linux/capability.h>

bool file_ns_capable(const struct file * file,struct user_namespace * ns,int cap)
{
	lx_emul_trace(__func__);
	return true;
}


#include <linux/rcupdate.h>

void synchronize_rcu(void)
{
	lx_emul_trace(__func__);
}


#include <linux/skbuff.h>

void __skb_get_hash(struct sk_buff * skb)
{
	lx_emul_trace(__func__);
}


#include <linux/skbuff.h>

bool __skb_flow_dissect(const struct net * net,const struct sk_buff * skb,struct flow_dissector * flow_dissector,void * target_container,const void * data,__be16 proto,int nhoff,int hlen,unsigned int flags)
{
	lx_emul_trace(__func__);
	return false;
}


#include <linux/pid.h>

pid_t pid_vnr(struct pid * pid)
{
	lx_emul_trace(__func__);
	return 0;
}


#include <linux/verification.h>

int verify_pkcs7_signature(const void *data, size_t len,
               const void *raw_pkcs7, size_t pkcs7_len,
               struct key *trusted_keys,
               enum key_being_used_for usage,
               int (*view_content)(void *ctx,
                           const void *data, size_t len,
                           size_t asn1hdrlen),
               void *ctx)
{
	return true;
}


#include <linux/acpi.h>
#include <acpi/acpi.h>
#include <acpi/acpi_bus.h>
#include <acpi/acpixf.h>

struct irq_domain *pci_host_bridge_acpi_msi_domain(struct pci_bus *bus)
{
	return NULL;
}


bool pciehp_is_native(struct pci_dev *bridge)
{
	return true;
}


#include <linux/net.h>

int net_ratelimit(void)
{
	lx_emul_trace(__func__);
	/* suppress */
	return 0;
}

#include <net/gen_stats.h>

void gen_kill_estimator(struct net_rate_estimator __rcu ** rate_est)
{
	lx_emul_trace(__func__);
	/* suppress */
	return;
}

#include <crypto/sha2.h>

void sha224_final(struct sha256_state * sctx,u8 * out)
{
	lx_emul_trace_and_stop(__func__);
}


#include <crypto/sha2.h>

void sha256_final(struct sha256_state * sctx,u8 * out)
{
	lx_emul_trace_and_stop(__func__);
}


#include <crypto/sha2.h>

void sha256_update(struct sha256_state * sctx,const u8 * data,unsigned int len)
{
	lx_emul_trace_and_stop(__func__);
}


#include <linux/usb.h>

int usb_reset_device(struct usb_device *dev)
{
	lx_emul_trace(__func__);
	return 0;
}


#include <linux/ratelimit.h>

int ___ratelimit(struct ratelimit_state * rs,const char * func)
{
	lx_emul_trace(__func__);
	return 1;
}

#include <linux/pgtable.h>

pteval_t __default_kernel_pte_mask __read_mostly = ~0;


#include <linux/random.h>

u32 __get_random_u32_below(u32 ceil)
{
	lx_emul_trace_and_stop(__func__);
}


u16 get_random_u16(void)
{
	lx_emul_trace_and_stop(__func__);
}


u8 get_random_u8(void)
{
	lx_emul_trace_and_stop(__func__);
}

DEFINE_PER_CPU_READ_MOSTLY(cpumask_var_t, cpu_sibling_map);
EXPORT_PER_CPU_SYMBOL(cpu_sibling_map);


#include <linux/filter.h>

DEFINE_STATIC_KEY_FALSE(bpf_master_redirect_enabled_key);
EXPORT_SYMBOL_GPL(bpf_master_redirect_enabled_key);


extern const struct attribute_group dev_attr_physical_location_group;
const struct attribute_group dev_attr_physical_location_group = {};


#include <linux/acpi.h>

extern void software_node_notify_remove(struct device * dev);
void software_node_notify_remove(struct device * dev)
{
	lx_emul_trace(__func__);
}

extern bool dev_add_physical_location(struct device * dev);
bool dev_add_physical_location(struct device * dev)
{
	lx_emul_trace(__func__);
	return false;
}


#include <net/gen_stats.h>

void gnet_stats_basic_sync_init(struct gnet_stats_basic_sync * b)
{
	lx_emul_trace(__func__);
}


#include <linux/context_tracking_irq.h>

noinstr void ct_irq_enter(void)
{
	lx_emul_trace(__func__);
}


#include <linux/context_tracking_irq.h>

noinstr void ct_irq_exit(void)
{
	lx_emul_trace(__func__);
}


unsigned int pci_rescan_bus(struct pci_bus *bus)
{
	lx_emul_trace(__func__);
	return 0;
}


void pcim_pin_device(struct pci_dev *pdev)
{
	lx_emul_trace(__func__);
}


void pcim_iounmap(struct pci_dev *pdev, void __iomem *addr)
{
	lx_emul_trace(__func__);
}


#include <linux/sysctl.h>

void __init __register_sysctl_init(const char * path,struct ctl_table * table,const char * table_name)
{
	lx_emul_trace(__func__);
}


void * high_memory;


int pcim_iomap_regions(struct pci_dev *pdev, int mask, const char *name)
{
	lx_emul_trace(__func__);
	return 0;
}


int pcie_capability_clear_and_set_word(struct pci_dev *dev, int pos,
                                       u16 clear, u16 set)
{
	lx_emul_trace_and_stop(__func__);
	return 0;
}

#include <linux/utsname.h>
#include <linux/user_namespace.h>

struct user_namespace init_user_ns;
struct uts_namespace init_uts_ns;