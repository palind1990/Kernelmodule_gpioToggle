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
	{ 0xbd97005b, "module_layout" },
	{ 0x9e9fbfdf, "param_array_ops" },
	{ 0x7b66b44c, "param_ops_int" },
	{ 0xfe990052, "gpio_free" },
	{ 0x97934ecf, "del_timer_sync" },
	{ 0x9dfdf722, "gpio_free_array" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0x4293a604, "gpiod_to_irq" },
	{ 0x8574ca6c, "gpio_request_array" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x403f9529, "gpio_request_one" },
	{ 0xc5850110, "printk" },
	{ 0x8f411754, "gpiod_get_raw_value" },
	{ 0x24d273d1, "add_timer" },
	{ 0x526c3a6c, "jiffies" },
	{ 0xba4c293, "gpiod_set_raw_value" },
	{ 0xe30324cf, "gpio_to_desc" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "6FB2B030FA7371A13C2A32B");
