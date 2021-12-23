#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

static int toggleSpeed = 1;
static int cnt = 0;
static int edgePin = 21;
static int ioPins[2] = {20, 21};
static int arr_argc = 0;

module_param(toggleSpeed, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(toggleSpeed, "An integer");

module_param_array(ioPins, int, &arr_argc, 0000);
MODULE_PARM_DESC(ioPins, "An array of integers");

module_param(edgePin, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(edgePin, "An integer");

static struct timer_list blink_timer;
static long led1 = 0;
static long led2 = 0;

/* ----------- From gpiomod_inpirq.c -----------  */
/* Define GPIOs for LEDs */
static struct gpio leds[] = {
	{4, GPIOF_OUT_INIT_LOW, "LED 1"},
};

/* Define GPIOs for BUTTONS */
static struct gpio buttons[] = {
	{17, GPIOF_IN, "BUTTON 1"}, // turns LED on
	{18, GPIOF_IN, "BUTTON 2"}, // turns LED off
};

static int button_irqs[] = {-1, -1};

/*The interrupt service routine called on button presses*/
static irqreturn_t button_isr(int irq, void *data)
{
	if (irq == button_irqs[0] && !gpio_get_value(leds[0].gpio))
	{
		gpio_set_value(leds[0].gpio, 1);
		cnt++;
		printk(KERN_INFO "Count: %d (off)", cnt);
	}
	else if (irq == button_irqs[1] && gpio_get_value(leds[0].gpio))
	{
		gpio_set_value(leds[0].gpio, 0);
		cnt++;
		printk(KERN_INFO "Count: %d (on)", cnt);
	}

	return IRQ_HANDLED;
}

static void blink_timer_func(struct timer_list *t)
{
	gpio_set_value(ioPins[0], led1);
	gpio_set_value(ioPins[1], led2);

	led1 = !led1;
	led2 = !led2;

	blink_timer.expires = jiffies + (toggleSpeed * HZ); // 1 sec.
	add_timer(&blink_timer);
}

static int __init clargmod_init(void)
{
	int i;
	int ret = 0;

	printk(KERN_INFO "Togglespeed: %d\n", toggleSpeed);
	printk(KERN_INFO "Edge detection pin: %d\n", edgePin);

	for (i = 0; i < (sizeof ioPins / sizeof(int)); i++)
	{
		printk(KERN_INFO "myintArray[%d] = %d\n", i, ioPins[i]);
	}

	printk(KERN_INFO "got %d arguments for myintArray.\n", arr_argc);
	printk(KERN_INFO "%s\n", __func__);

	// register, turn off
	ret = gpio_request_one(ioPins[0], GPIOF_OUT_INIT_LOW, "ioPins[0]");
	ret = gpio_request_one(ioPins[1], GPIOF_OUT_INIT_LOW, "ioPins[1]");

	if (ret)
	{
		printk(KERN_ERR "Unable to request GPioPins: %d\n", ret);
		return ret;
	}
	timer_setup(&blink_timer, blink_timer_func, 0);

	blink_timer.function = blink_timer_func;
	blink_timer.expires = jiffies + (toggleSpeed * HZ); // 1 sec.
	add_timer(&blink_timer);

	/* -------- From gpiomod_inpirq.c -------- */
	// register LED gpios
	ret = gpio_request_array(leds, ARRAY_SIZE(leds));

	if (ret)
	{
		printk(KERN_ERR "Unable to request GPIOs for LEDs: %d\n", ret);
		return ret;
	}

	// register BUTTON gpios
	ret = gpio_request_array(buttons, ARRAY_SIZE(buttons));

	if (ret)
	{
		printk(KERN_ERR "Unable to request GPIOs for BUTTONs: %d\n", ret);
		goto fail1;
	}

	printk(KERN_INFO "Current button1 value: %d\n", gpio_get_value(buttons[0].gpio));

	ret = gpio_to_irq(buttons[0].gpio);

	if (ret < 0)
	{
		printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
		goto fail2;
	}

	button_irqs[0] = ret;

	printk(KERN_INFO "Successfully requested BUTTON1 IRQ # %d\n", button_irqs[0]);

	ret = request_irq(button_irqs[0], button_isr, IRQF_TRIGGER_RISING /*| IRQF_DISABLED*/, "gpiomod#button1", NULL);

	if (ret)
	{
		printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
		goto fail2;
	}

	ret = gpio_to_irq(buttons[1].gpio);

	if (ret < 0)
	{
		printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
		goto fail2;
	}

	button_irqs[1] = ret;

	printk(KERN_INFO "Successfully requested BUTTON2 IRQ # %d\n", button_irqs[1]);

	ret = request_irq(button_irqs[1], button_isr, IRQF_TRIGGER_RISING /*| IRQF_DISABLED*/, "gpiomod#button2", NULL);

	if (ret)
	{
		printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
		goto fail3;
	}

	return 0;

fail3:
	free_irq(button_irqs[0], NULL);

fail2:
	gpio_free_array(buttons, ARRAY_SIZE(leds));

fail1:
	gpio_free_array(leds, ARRAY_SIZE(leds));

	return ret;
}

/*
 * Exit function
 */
static void __exit clargmod_exit(void)
{
	int i;

	printk(KERN_INFO "%s\n", __func__);

	// deactivate timer if running
	del_timer_sync(&blink_timer);

	// turn LED off
	gpio_set_value(ioPins[0], 0);
	gpio_set_value(ioPins[1], 0);

	// unregister GPIO
	gpio_free(ioPins[0]);
	gpio_free(ioPins[1]);

	/* -------- From gpiomod_inpirq.c -------- */
	// free irqs
	free_irq(button_irqs[0], NULL);
	free_irq(button_irqs[1], NULL);

	// turn all LEDs off
	for (i = 0; i < ARRAY_SIZE(leds); i++)
	{
		gpio_set_value(leds[i].gpio, 0);
	}

	// unregister
	gpio_free_array(leds, ARRAY_SIZE(leds));
	gpio_free_array(buttons, ARRAY_SIZE(buttons));
	/* -------- Till here -------- */
}

MODULE_AUTHOR("justin ooghe");
MODULE_DESCRIPTION("kernelmodule to set some gpios");

module_init(clargmod_init);
module_exit(clargmod_exit);