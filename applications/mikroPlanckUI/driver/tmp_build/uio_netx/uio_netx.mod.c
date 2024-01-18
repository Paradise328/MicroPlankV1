#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
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
__used __section(__versions) = {
	{ 0xb3753869, "module_layout" },
	{ 0x5ab5b891, "param_ops_int" },
	{ 0xa3bf6975, "param_array_ops" },
	{ 0xe23f4d3, "param_ops_ulong" },
	{ 0x478f00e1, "pci_unregister_driver" },
	{ 0x24694125, "__pci_register_driver" },
	{ 0x977f511b, "__mutex_init" },
	{ 0x409bcb62, "mutex_unlock" },
	{ 0x2ab7989d, "mutex_lock" },
	{ 0xe582fa0, "misc_register" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x38e946e0, "misc_deregister" },
	{ 0xc5850110, "printk" },
	{ 0xc87520bc, "__uio_register_device" },
	{ 0x93a219c, "ioremap_nocache" },
	{ 0xedf4f48c, "pci_request_regions" },
	{ 0x42980214, "pci_enable_device" },
	{ 0xf5cb25c8, "kmem_cache_alloc_trace" },
	{ 0x35216b26, "kmalloc_caches" },
	{ 0x9409627e, "_dev_info" },
	{ 0xae630ce8, "pci_disable_device" },
	{ 0x7154900d, "pci_release_regions" },
	{ 0xb249b3bc, "uio_unregister_device" },
	{ 0x37a0cba, "kfree" },
	{ 0xedc03953, "iounmap" },
	{ 0x2b54d1d5, "dma_free_attrs" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0xbcbfbe44, "_dev_warn" },
	{ 0xfb578fc5, "memset" },
	{ 0xfda0bdf3, "dma_alloc_attrs" },
	{ 0x4a453f53, "iowrite32" },
	{ 0xe484e35f, "ioread32" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "uio");


MODULE_INFO(srcversion, "DC344D4934CD86FAD032D6B");
