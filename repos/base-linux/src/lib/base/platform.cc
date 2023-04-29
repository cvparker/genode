/*
 * \brief  Support for the Linux-specific environment
 * \author Norman Feske
 * \author Stefan Thoeni
 * \date   2008-12-12
 */

/*
 * Copyright (C) 2008-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

/* Genode includes */
#include <util/arg_string.h>
#include <base/thread.h>
#include <linux_dataspace/client.h>
#include <linux_syscalls.h>

/* base-internal includes */
#include <base/internal/platform.h>
#include <base/internal/native_thread.h>
#include <base/internal/globals.h>
#include <base/internal/parent_socket_handle.h>
#include <base/internal/capability_space_tpl.h>

#include <deprecated/env.h>

using namespace Genode;


/*********************************
 ** Support for Rm_session_mmap **
 *********************************/

size_t Region_map_mmap::_dataspace_size(Dataspace_capability ds)
{
	if (local(ds))
		return Local_capability<Dataspace>::deref(ds)->size();

	return Dataspace_client(ds).size();

}


int Region_map_mmap::_dataspace_fd(Dataspace_capability ds)
{
	Untyped_capability fd_cap = Linux_dataspace_client(ds).fd();
	return lx_dup(Capability_space::ipc_cap_data(fd_cap).dst.socket.value);
}


bool Region_map_mmap::_dataspace_writeable(Dataspace_capability ds)
{
	return Dataspace_client(ds).writeable();
}


/******************
 ** Local_parent **
 ******************/

Session_capability Local_parent::session(Parent::Client::Id  id,
                                         Service_name const &service_name,
                                         Session_args const &args,
                                         Affinity     const &affinity)
{
	if (strcmp(service_name.string(), Rm_session::service_name()) == 0) {

		Local_rm_session *local_rm_session = new (_alloc)
			Local_rm_session(_local_rm, _alloc, _local_sessions_id_space, id);

		return local_rm_session->local_session_cap();
	}

	return Expanding_parent_client::session(id, service_name, args, affinity);
}


Parent::Close_result Local_parent::close(Client::Id id)
{
	auto close_local_fn = [&] (Local_session &local_session)
	{
		Capability<Rm_session> rm =
			static_cap_cast<Rm_session>(local_session.local_session_cap());
		destroy(_alloc, Local_capability<Rm_session>::deref(rm));
	};

	/*
	 * Local RM sessions are present in the '_local_sessions_id_space'. If the
	 * apply succeeds, 'id' referred to the local session. Otherwise, we
	 * forward the request to the parent.
	 */
	try {
		_local_sessions_id_space.apply<Local_session>(id, close_local_fn);
		return CLOSE_DONE;
	}
	catch (Id_space<Client>::Unknown_id) { }

	return Parent_client::close(id);
}


Local_parent::Local_parent(Parent_capability parent_cap,
                           Region_map &local_rm, Allocator &alloc)
:
	Expanding_parent_client(parent_cap), _local_rm(local_rm), _alloc(alloc)
{ }


/**************
 ** Platform **
 **************/

/**
 * List of Unix environment variables, initialized by the startup code
 */
extern char **lx_environ;


/**
 * Read environment variable as long value
 */
static unsigned long get_env_ulong(const char *key)
{
	for (char **curr = lx_environ; curr && *curr; curr++) {

		Arg arg = Arg_string::find_arg(*curr, key);
		if (arg.valid())
			return arg.ulong_value(0);
	}

	return 0;
}


static Platform *_platform_ptr;


Env_deprecated *Genode::env_deprecated()
{
	if (!_platform_ptr) {
		error("missing call of init_platform");
		for (;;);
	}

	struct Impl : Env_deprecated, Noncopyable
	{
		Platform &_pf;

		Impl(Platform &pf) : _pf(pf) { }

		Parent                *parent()          override { return &_pf.parent; }
		Cpu_session           *cpu_session()     override { return &_pf.cpu; }
		Cpu_session_capability cpu_session_cap() override { return  _pf.cpu_cap; }
		Region_map            *rm_session()      override { return &_pf.rm; }
		Pd_session            *pd_session()      override { return &_pf.pd; }
		Pd_session_capability  pd_session_cap()  override { return  _pf.pd_cap; }
	};

	static Impl impl { *_platform_ptr };

	return &impl;
}


static Parent_capability obtain_parent_cap()
{
	long const local_name = get_env_ulong("parent_local_name");

	Untyped_capability parent_cap =
		Capability_space::import(Rpc_destination(Lx_sd{PARENT_SOCKET_HANDLE}),
		                         Rpc_obj_key(local_name));

	return reinterpret_cap_cast<Parent>(parent_cap);
}


void Genode::init_parent_resource_requests(Genode::Env & env)
{
	/**
	 * Catch up asynchronous resource request and notification
	 * mechanism construction of the expanding parent environment
	 */
	using Parent = Expanding_parent_client;
	static_cast<Parent*>(&env.parent())->init_fallback_signal_handling();
}


void Genode::init_platform()
{
	static Platform platform { obtain_parent_cap() };

	/* allow use of deprecated_env */
	_platform_ptr = &platform;

	init_log(platform.parent);
	init_rpc_cap_alloc(platform.parent);

	env_stack_area_region_map    = &platform.pd._stack_area;
	env_stack_area_ram_allocator = &platform.pd;

	/* register TID and PID of the main thread at core */
	Linux_native_cpu_client native_cpu(platform.cpu.native_cpu());

	native_cpu.thread_id(platform.parent.main_thread_cap(),
	                     lx_getpid(), lx_gettid());

	init_rpc_cap_alloc(platform.parent);
}


/*************************
 ** Support for seccomp **
 *************************/

/* Linux includes */
#include <errno.h>
#include <sys/prctl.h>     /* prctl */
#include <linux/seccomp.h> /* seccomp's constants */


extern char _binary_seccomp_bpf_policy_bin_start[];
extern char _binary_seccomp_bpf_policy_bin_end[];


void Genode::binary_ready_hook_for_platform()
{
	if (lx_prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) != 0) {
		error("PR_SET_NO_NEW_PRIVS failed");
		throw Exception();
	}

	struct Bpf_program
	{
		uint16_t  blk_cnt;
		uint64_t *blks;
	};

	for (char* i = _binary_seccomp_bpf_policy_bin_start;
	     i < _binary_seccomp_bpf_policy_bin_end - sizeof(uint32_t); i++) {

		uint32_t *v = reinterpret_cast<uint32_t *>(i);
		if (*v == 0xCAFEAFFE) {
			*v = lx_getpid();
		}
	}

	Bpf_program program {
		.blk_cnt = (uint16_t)((_binary_seccomp_bpf_policy_bin_end -
		                       _binary_seccomp_bpf_policy_bin_start) /
		                       sizeof(uint64_t)),
		.blks = (uint64_t *)_binary_seccomp_bpf_policy_bin_start
	};

	uint64_t flags = SECCOMP_FILTER_FLAG_TSYNC;
	auto ret = lx_seccomp(SECCOMP_SET_MODE_FILTER, (int)flags, &program);
	if (ret != 0) {
		error("SECCOMP_SET_MODE_FILTER failed ", ret);
		throw Exception();
	}
}
