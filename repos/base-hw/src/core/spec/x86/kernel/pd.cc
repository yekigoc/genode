/*
 * \brief   Kernel backend for protection domains
 * \author  Stefan Kalkowski
 * \date    2015-03-20
 */

/*
 * Copyright (C) 2015 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* core includes */
#include <platform_pd.h>
#include <kernel/pd.h>

Kernel::Pd::Pd(Kernel::Pd::Table   * const table,
               Genode::Platform_pd * const platform_pd)
: _table(table), _platform_pd(platform_pd) { }


Kernel::Pd::~Pd() { }


void Kernel::Pd::admit(Kernel::Cpu::Context * const c) {
	c->init((addr_t)translation_table(), this == Kernel::core_pd()); }
