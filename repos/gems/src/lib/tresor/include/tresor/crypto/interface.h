/*
 * \brief  Integration of the Tresor block encryption
 * \author Martin Stein
 * \author Josef Soentgen
 * \date   2020-11-10
 */

/*
 * Copyright (C) 2020 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _TRESOR__CRYPTO__INTERFACE_H_
#define _TRESOR__CRYPTO__INTERFACE_H_

/* base includes */
#include <base/exception.h>
#include <base/stdint.h>


namespace Tresor_crypto {

	struct Interface;

	Interface &get_interface();

	enum { BLOCK_SIZE = 4096u };

} /* namespace Tresor_crypto */


struct Tresor_crypto::Interface
{
	struct Buffer_too_small        : Genode::Exception { };
	struct Key_value_size_mismatch : Genode::Exception { };

	struct Complete_request
	{
		bool     const valid;
		Genode::uint64_t const block_number;
	};

	struct Slots
	{
		enum { NUM_SLOTS = 2, };
		Genode::uint32_t _store[NUM_SLOTS] { };

		bool store(Genode::uint32_t const id)
		{
			for (Genode::uint32_t &slot : _store) {
				if (slot == 0) {
					slot = id;
					return true;
				}
			}
			return false;
		}

		void remove(Genode::uint32_t const id)
		{
			for (Genode::uint32_t &slot : _store) {
				if (slot == id) {
					slot = 0;
					return;
				}
			}
		}

		template <typename FN>
		void for_each_key(FN const &func)
		{
			for (Genode::uint32_t const slot : _store) {
				if (slot != 0) {
					func(slot);
				}
			}
		}
	};

	Slots _slots { };

	virtual ~Interface() { }

	template <typename FN>
	void for_each_key(FN const &func)
	{
		_slots.for_each_key(func);
	}

	virtual bool execute() = 0;

	virtual bool add_key(Genode::uint32_t const  id,
	                     char const     *value,
	                     Genode::size_t          value_len) = 0;

	virtual bool remove_key(Genode::uint32_t const id) = 0;

	virtual bool submit_encryption_request(Genode::uint64_t const  block_number,
	                                       Genode::uint32_t const  key_id,
	                                       Genode::Const_byte_range_ptr const &src) = 0;

	virtual Complete_request encryption_request_complete(Genode::Byte_range_ptr const &dst) = 0;

	virtual bool submit_decryption_request(Genode::uint64_t const  block_number,
	                                       Genode::uint32_t const  key_id,
	                                       Genode::Const_byte_range_ptr const &src) = 0;

	virtual Complete_request decryption_request_complete(Genode::Byte_range_ptr const &dst) = 0;
};

#endif /* _TRESOR__CRYPTO__INTERFACE_H_ */
