#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x2b22e0c3, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x1a47188f, __VMLINUX_SYMBOL_STR(rpl_lisp_xmit) },
	{ 0x66b550e2, __VMLINUX_SYMBOL_STR(ovs_netdev_tunnel_destroy) },
	{ 0xb2c8d605, __VMLINUX_SYMBOL_STR(ovs_vport_ops_unregister) },
	{ 0xbb560634, __VMLINUX_SYMBOL_STR(__ovs_vport_ops_register) },
	{ 0x179ba693, __VMLINUX_SYMBOL_STR(__skb_get_rxhash) },
	{ 0xa814bbe3, __VMLINUX_SYMBOL_STR(ovs_tunnel_get_egress_info) },
	{ 0x606885ba, __VMLINUX_SYMBOL_STR(ovs_vport_free) },
	{ 0xdef52746, __VMLINUX_SYMBOL_STR(ovs_netdev_link) },
	{ 0x6e720ff2, __VMLINUX_SYMBOL_STR(rtnl_unlock) },
	{ 0x57c8b416, __VMLINUX_SYMBOL_STR(dev_change_flags) },
	{ 0xef9e8b5a, __VMLINUX_SYMBOL_STR(rpl_lisp_dev_create_fb) },
	{ 0xc7a4fbed, __VMLINUX_SYMBOL_STR(rtnl_lock) },
	{ 0xc2b538ac, __VMLINUX_SYMBOL_STR(ovs_vport_alloc) },
	{ 0xcd279169, __VMLINUX_SYMBOL_STR(nla_find) },
	{ 0x2438e0a6, __VMLINUX_SYMBOL_STR(nla_put) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=openvswitch";


MODULE_INFO(srcversion, "4ACF251697BBC0AEED176A5");
