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
	{ 0xf342b875, "of_property_read_string" },
	{ 0x72dc3d7a, "of_find_property" },
	{ 0x9f49dcc4, "__stack_chk_fail" },
	{ 0xc5850110, "printk" },
	{ 0x1bf78489, "of_find_node_opts_by_path" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "429B95D8AF0606D671D822B");
