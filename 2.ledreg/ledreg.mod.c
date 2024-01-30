#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x667bfb48, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x383419fd, "class_destroy" },
	{ 0x17e6ec06, "device_destroy" },
	{ 0x6ddd9da3, "device_create" },
	{ 0x3ecfccd6, "__class_create" },
	{ 0x23875fc, "__register_chrdev" },
	{ 0x9f49dcc4, "__stack_chk_fail" },
	{ 0x56470118, "__warn_printk" },
	{ 0xdcb764ad, "memset" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0x6b4b2933, "__ioremap" },
	{ 0xaf56600a, "arm64_use_ng_mappings" },
	{ 0xc5850110, "printk" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "A75EF2392616714492AE285");
