/*
 * \brief  Wireless network driver Linux port
 * \author Josef Soentgen
 * \date   2022-02-10
 */

/*
 * Copyright (C) 2022 Genode Labs GmbH
 *
 * This file is distributed under the terms of the GNU General Public License
 * version 2.
 */

/* Genode includes */
#include <base/attached_rom_dataspace.h>
#include <base/component.h>
#include <base/env.h>
#include <os/reporter.h>
#include <net/mac_address.h>
#include <genode_c_api/uplink.h>

/* DDE Linux includes */
#include <lx_emul/init.h>
#include <lx_emul/page_virt.h>
#include <lx_emul/task.h>
#include <lx_kit/env.h>
#include <lx_kit/init.h>
#include <lx_user/io.h>

/* local includes */
#include "lx_user.h"
#include "dtb_helper.h"

using namespace Genode;


extern "C" int  lx_emul_rfkill_get_any(void);
extern "C" void lx_emul_rfkill_switch_all(int blocked);

static Signal_context_capability _rfkill_sigh_cap;


bool _wifi_get_rfkill(void)
{
	/*
	 * It is safe to call this from non EP threads as we
	 * only query a variable.
	 */
	return lx_emul_rfkill_get_any();
}


void _wifi_set_rfkill(bool blocked)
{
	if (!rfkill_task_struct_ptr)
		return;

	lx_emul_rfkill_switch_all(blocked);

	lx_emul_task_unblock(rfkill_task_struct_ptr);
	Lx_kit::env().scheduler.schedule();

	/*
	 * We have to open the device again after unblocking
	 * as otherwise we will get ENETDOWN. So unblock the uplink
	 * task _afterwards_ because there we call * 'dev_open()'
	 * unconditionally and that will bring the netdevice UP again.
	 */
	lx_emul_task_unblock(uplink_task_struct_ptr);
	Lx_kit::env().scheduler.schedule();

	Signal_transmitter(_rfkill_sigh_cap).submit();
}


bool wifi_get_rfkill(void)
{
	return _wifi_get_rfkill();
}


extern "C" unsigned int wifi_ifindex(void)
{
	/* TODO replace with actual qyery */
	return 2;
}


extern "C" char const *wifi_ifname(void)
{
	/* TODO replace with actual qyery */
	return "wlan0";
}


struct Mac_address_reporter
{
	bool _enabled = false;
 
	Net::Mac_address _mac_address { };

	Constructible<Reporter> _reporter { };

	Env &_env;

	Signal_context_capability _sigh;

	Mac_address_reporter(Env &env, Signal_context_capability sigh)
	: _env(env), _sigh(sigh)
	{
		Attached_rom_dataspace config { _env, "config" };

		config.xml().with_optional_sub_node("report", [&] (Xml_node const &xml) {
			_enabled = xml.attribute_value("mac_address", false); });
	}

	void mac_address(Net::Mac_address const &mac_address)
	{
		_mac_address = mac_address;

		Signal_transmitter(_sigh).submit();
	}

	void report()
	{
		if (!_enabled)
			return;

		_reporter.construct(_env, "devices");
		_reporter->enabled(true);

		Reporter::Xml_generator report(*_reporter, [&] () {
			report.node("nic", [&] () {
				report.attribute("mac_address", String<32>(_mac_address));
			});
		});

		/* report only once */
		_enabled = false;
	}
};

Constructible<Mac_address_reporter> mac_address_reporter;


/* used from socket_call.cc */
void _wifi_report_mac_address(Net::Mac_address const &mac_address)
{
	mac_address_reporter->mac_address(mac_address);
}


struct Wlan
{
	Env                    &_env;
	Io_signal_handler<Wlan> _signal_handler { _env.ep(), *this,
	                                          &Wlan::_handle_signal };

	Dtb_helper _dtb_helper { _env };

	void _handle_signal()
	{
		if (uplink_task_struct_ptr) {
			lx_emul_task_unblock(uplink_task_struct_ptr);
			Lx_kit::env().scheduler.schedule();
		}

		genode_uplink_notify_peers();

		mac_address_reporter->report();
	}

	Wlan(Env &env) : _env { env }
	{
		mac_address_reporter.construct(_env, _signal_handler);

		genode_uplink_init(genode_env_ptr(_env),
		                   genode_allocator_ptr(Lx_kit::env().heap),
		                   genode_signal_handler_ptr(_signal_handler));

		lx_emul_start_kernel(_dtb_helper.dtb_ptr());
	}
};


static Blockade *wpa_blockade;


extern "C" void wakeup_wpa()
{
	static bool called_once = false;
	if (called_once)
		return;

	wpa_blockade->wakeup();
	called_once = true;
}


void wifi_init(Env &env, Blockade &blockade)
{
	wpa_blockade = &blockade;

	static Wlan wlan(env);
}


void wifi_set_rfkill_sigh(Signal_context_capability cap)
{
	_rfkill_sigh_cap = cap;
}
