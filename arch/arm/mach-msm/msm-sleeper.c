/* Copyright (c) 2010-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/powersuspend.h>
#include <linux/workqueue.h>
#include <linux/cpu.h>
#include <linux/module.h>
#include <linux/cpufreq.h>
#include <mach/cpufreq.h>

#define MSM_SLEEPER_MAJOR_VERSION	1
#define MSM_SLEEPER_MINOR_VERSION	1

extern uint32_t maxscroff;
extern uint32_t maxscroff_freq;

#ifdef CONFIG_HAS_POWERSUSPEND
static void msm_sleeper_power_suspend(struct power_suspend *h)
{
	int cpu;

	if (maxscroff) {
		for_each_possible_cpu(cpu) {
			msm_cpufreq_set_freq_limits(cpu, MSM_CPUFREQ_NO_LIMIT, maxscroff_freq);
			pr_info("msm-sleeper: limit max frequency to: %d\n", maxscroff_freq);
		}
		limit_set = 1;
	}
	return; 
}

static void msm_sleeper_power_resume(struct power_suspend *h)
{
	int cpu;

	if (!limit_set)
		return;

	for_each_possible_cpu(cpu) {
		msm_cpufreq_set_freq_limits(cpu, MSM_CPUFREQ_NO_LIMIT, MSM_CPUFREQ_NO_LIMIT);
		pr_info("msm-sleeper: restore max frequency.\n");
	}
	limit_set = 0;
	return; 
}

static struct power_suspend msm_sleeper_power_suspend_driver = {
	.suspend = msm_sleeper_power_suspend,
	.resume = msm_sleeper_power_resume,
};
#endif

static int __init msm_sleeper_init(void)
{
	pr_info("msm-sleeper version %d.%d\n",
		 MSM_SLEEPER_MAJOR_VERSION,
		 MSM_SLEEPER_MINOR_VERSION);

#ifdef CONFIG_HAS_POWERSUSPEND
	register_power_suspend(&msm_sleeper_power_suspend_driver);
#endif
	return 0;
}


MODULE_DESCRIPTION("'msm-sleeper' - Limit max frequency while screen is off");
MODULE_LICENSE("GPL v2");

late_initcall(msm_sleeper_init);
