/*
 * Copyright (c) 2012, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/acpi.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/clocksource.h>

extern struct of_device_id __clksrc_of_table[];
extern struct of_device_id __clksrc_ret_of_table[];

static const struct of_device_id __clksrc_of_table_sentinel
	__used __section(__clksrc_of_table_end);

static const struct of_device_id __clksrc_ret_of_table_sentinel
	__used __section(__clksrc_ret_of_table_end);

void __init clocksource_probe(void)
{
	struct device_node *np;
	const struct of_device_id *match;
	of_init_fn_1 init_func;
	of_init_fn_1_ret init_func_ret;
	unsigned clocksources = 0;
	int ret;

	for_each_matching_node_and_match(np, __clksrc_of_table, &match) {
		if (!of_device_is_available(np))
			continue;

		init_func = match->data;
		init_func(np);
		clocksources++;
	}

	for_each_matching_node_and_match(np, __clksrc_ret_of_table, &match) {
		if (!of_device_is_available(np))
			continue;

		init_func_ret = match->data;

		ret = init_func_ret(np);
		if (ret) {
			pr_err("Failed to initialize '%s': %d",
			       of_node_full_name(np), ret);
			continue;
		}

		clocksources++;
	}

	clocksources += acpi_probe_device_table(clksrc);

	if (!clocksources)
		pr_crit("%s: no matching clocksources found\n", __func__);
}
