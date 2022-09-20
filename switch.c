#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("cronyx");

static unsigned int led;
module_param(led,int,0660);

static unsigned int button;
module_param(button,int,0660);

static unsigned int irqNum;
static bool stateOn = 0;
 
static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

static void call_external(void) {
  char *argv[] = { "/bin/sh", "/usr/bin/external.sh", NULL };
  static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
  call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
}

static int __init gpio_init(void) {
  int result = 0;
  
  if (!gpio_is_valid(led) || !gpio_is_valid(button)) {
	printk(KERN_INFO "SW: Led or button gpio is invalid!\n");
	return -ENODEV;
  }

  printk(KERN_INFO "SW: Initializing... led=%d button=%d\n", led, button);

  stateOn = false;
  gpio_request(led, "sysfs");
  gpio_direction_output(led, stateOn);
  // gpio_set_value(led, stateOn);
  gpio_export(led, false);

  gpio_request(button, "sysfs");
  gpio_direction_input(button);
  gpio_set_debounce(button, 400);
  gpio_export(button, false);

  printk(KERN_INFO "SW: The button state is: %d\n", gpio_get_value(button));
 
  irqNum = gpio_to_irq(button);
  printk(KERN_INFO "SW: The button is mapped to IRQ: %d\n", irqNum);

  result = request_irq(irqNum, (irq_handler_t) gpio_irq_handler, IRQF_TRIGGER_RISING, "gpio_handler", NULL);
  printk(KERN_INFO "SW: The interrupt request result is: %d\n", result);

  return result;
}

static void __exit gpio_exit(void) {
  printk(KERN_INFO "SW: The button state is: %d\n", gpio_get_value(button));
  gpio_set_value(led, 0);
  gpio_unexport(led);
  free_irq(irqNum, NULL);
  gpio_unexport(button);
  gpio_free(led);
  gpio_free(button);
  printk(KERN_INFO "SW: Uninitializing...");
}

static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
  stateOn = !stateOn;
  gpio_set_value(led, stateOn);
  printk(KERN_INFO "SW: Interrupt! (button state is %d)\n", gpio_get_value(button));
  call_external();
  return (irq_handler_t) IRQ_HANDLED;
}

module_init(gpio_init);
module_exit(gpio_exit);
