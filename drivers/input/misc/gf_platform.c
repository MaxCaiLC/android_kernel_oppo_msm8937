// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2014-2018, OPPO Mobile Comm Corp., Ltd
 */

#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/timer.h>
#include <linux/err.h>

#include "gf_spi.h"

#if defined(USE_SPI_BUS)
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#elif defined(USE_PLATFORM_BUS)
#include <linux/platform_device.h>
#endif

/*GPIO pins reference.*/
int gf_parse_dts(struct gf_dev* gf_dev)
{
    int rc = 0;
    
    gf_dev->reset_gpio = of_get_named_gpio(gf_dev->spi->dev.of_node, "goodix,gpio_reset", 0);
    if (!gpio_is_valid(gf_dev->reset_gpio)) {
        pr_info("RESET GPIO is invalid.\n");
        return -1;
    }
    rc = gpio_request(gf_dev->reset_gpio, "goodix_reset");
    if (rc) {
        dev_err(&gf_dev->spi->dev, "Failed to request RESET GPIO. rc = %d\n", rc);
        return -1;
    }
    gpio_direction_output(gf_dev->reset_gpio, 1);

    /*get irq resourece*/
    gf_dev->irq_gpio = of_get_named_gpio(gf_dev->spi->dev.of_node, "goodix,gpio_irq", 0);
    pr_info("gf:irq_gpio:%d\n", gf_dev->irq_gpio);
    if (!gpio_is_valid(gf_dev->irq_gpio)) {
        pr_info("IRQ GPIO is invalid.\n");
        return -1;
    }

    rc = gpio_request(gf_dev->irq_gpio, "goodix_irq");
    if(rc) {
        dev_err(&gf_dev->spi->dev, "Failed to request IRQ GPIO. rc = %d\n", rc);
        return -1;
    }
    gpio_direction_input(gf_dev->irq_gpio);

    return 0;
}

void gf_cleanup(struct gf_dev	* gf_dev)
{
    pr_info("[info] %s\n",__func__);

    if (gpio_is_valid(gf_dev->irq_gpio)) {
        gpio_free(gf_dev->irq_gpio);
        pr_info("remove irq_gpio success\n");
    }

    if (gpio_is_valid(gf_dev->reset_gpio)) {
        gpio_free(gf_dev->reset_gpio);
        pr_info("remove reset_gpio success\n");
    }
}

/*power management*/
int gf_power_on(struct gf_dev* gf_dev)
{
    int rc = 0;

    msleep(10);
    pr_info("---- power on ok ----\n");

    return rc;
}

int gf_power_off(struct gf_dev* gf_dev)
{
    int rc = 0;			

    pr_info("---- power off ----\n");
    return rc;
}

/********************************************************************
 *CPU output low level in RST pin to reset GF. This is the MUST action for GF.
 *Take care of this function. IO Pin driver strength / glitch and so on.
 ********************************************************************/
int gf_hw_reset(struct gf_dev *gf_dev, unsigned int delay_ms)
{	
    if (gf_dev == NULL) {
        pr_info("Input buff is NULL.\n");
        return -1;
    }
    pr_err("%s enter\n", __func__);
    gpio_direction_output(gf_dev->reset_gpio, 1);
    gpio_set_value(gf_dev->reset_gpio, 0);
    mdelay(5);
    gpio_set_value(gf_dev->reset_gpio, 1);
    mdelay(delay_ms);
    return 0;
}

int gf_irq_num(struct gf_dev *gf_dev)
{
    if(gf_dev == NULL) {
        pr_info("Input buff is NULL.\n");
        return -1;
    } else {
        return gpio_to_irq(gf_dev->irq_gpio);
    }
}

