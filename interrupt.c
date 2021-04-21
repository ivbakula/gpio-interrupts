#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/gpio/driver.h>
#include <linux/gpio/consumer.h>

#define MODULE_NAME	"interrupt_driven_led"
#define MODULE_COMPAT	"irq-control"

#define LEVEL_HIGH	1	
#define LEVEL_LOW	0	
#define GPIO_LABEL	"ctrl"

struct irq_ledctrl {
	bool 			level;		/* logical level 0 || 1 */
	int			irq;		/* interrupt number */
	struct gpio_desc 	*irq_line;	/* interrupt gpio line */
	struct gpio_desc	*output;	/* output gpio line (connect +LED)*/
};

static struct irq_ledctrl ledctrl;

static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
	ledctrl.level = !ledctrl.level;	
	gpiod_set_value(ledctrl.output, ledctrl.level ? LEVEL_HIGH : LEVEL_LOW);
	return IRQ_HANDLED;
}

static int irq_led_probe(struct platform_device *pdev)
{
	struct device *dev;
	int ret;
	dev = &pdev->dev;

	ledctrl.level    = true;
	ledctrl.output	 = gpiod_get_index(dev, GPIO_LABEL, 0, GPIOD_OUT_HIGH);
	if (IS_ERR(ledctrl.output)) {
		ret = -ENODEV;
		printk(KERN_ALERT "Unable to get output line\n");
		goto err_output;
	}

	ledctrl.irq_line = gpiod_get_index(dev, GPIO_LABEL, 1, GPIOD_IN);
	if (IS_ERR(ledctrl.irq_line)) {
		ret = -ENODEV;
		printk(KERN_ALERT "Unable to get irq line\n");
		goto err_irqline;
	}
	
	ledctrl.irq = gpiod_to_irq(ledctrl.irq_line);
	if (ledctrl.irq < 0) {
		ret = ledctrl.irq;
		printk(KERN_ALERT "Unable to obtain IRQ. Error code: %d\n", ret);
		goto err_irq;
	}

	ret = request_irq(ledctrl.irq, gpio_irq_handler, IRQF_TRIGGER_RISING,
			MODULE_NAME, NULL);
	if (ret < 0) {
		printk(KERN_ALERT "Unable to get irq_handler\n"
				  "Error code: %d\n", ret);
		goto err_irq;
	}

	gpiod_set_value(ledctrl.output, ledctrl.level ? LEVEL_HIGH : LEVEL_LOW);
	return 0;

err_irq:
	gpiod_put(ledctrl.irq_line);
err_irqline:
	gpiod_put(ledctrl.output);
err_output:
	return ret; 
}

static int irq_led_remove(struct platform_device *pdev)
{
	free_irq(ledctrl.irq, NULL);
	gpiod_put(ledctrl.irq_line);
	gpiod_put(ledctrl.output);
	return 0;
}

static const struct of_device_id irq_led_gpio_ids[] = {
	{ .compatible = "irq-control"},
	{ },
};
MODULE_DEVICE_TABLE(of, irq_led_gpio_ids);

static struct platform_driver irq_led_driver = {
	.driver = {
		.name = MODULE_NAME,
		.of_match_table = of_match_ptr(irq_led_gpio_ids),
	},
	.probe  = irq_led_probe,
	.remove = irq_led_remove,
};
module_platform_driver(irq_led_driver);
MODULE_LICENSE("GPL");
