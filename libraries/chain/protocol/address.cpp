/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <graphene/chain/protocol/types.hpp>
#include <graphene/chain/protocol/address.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/crypto/base58.hpp>
#include <algorithm>

namespace graphene {
	namespace chain {

		bool address::testnet_mode = false;
		address::address() {}

		address::address(const std::string& base58str, const char *prefix_str)
		{
			if (base58str == ADDRESS_NOT_INITED)
				return;
			std::string prefix(prefix_str);
			if (testnet_mode)
				prefix = GRAPHENE_ADDRESS_TESTNET_PREFIX;
			if (base58str.size() <= prefix.size())
			{
				return;
			}
			FC_ASSERT(is_valid(base58str, prefix), "${str}", ("str", base58str + " -- " + prefix));

			std::vector<char> v = fc::from_base58(base58str);
            version = (unsigned char)v.data();
			memcpy(addr.data, v.data()+1, std::min<size_t>(v.size() - 4, sizeof(addr)));
		}

		bool address::is_valid(const std::string& base58str, const std::string& prefix)
		{
			if (prefix.empty())
			{
				if (testnet_mode)
					return is_valid(base58str, GRAPHENE_ADDRESS_TESTNET_PREFIX);
				else
					return is_valid(base58str, GRAPHENE_ADDRESS_PREFIX);
			}
			if (base58str.size() <= prefix.size())
				return false;
			const size_t prefix_len = prefix.size();
			std::vector<char> v;
			try
			{
				v = fc::from_base58(base58str);
				if (v.size() == 0)
					return false;
				if (v[0] != addressVersion::CONTRACT && v[0] != addressVersion::MULTISIG && v[0] != addressVersion::NORMAL)
					return false;
			}
			catch (const fc::parse_error_exception& e)
			{
				return false;
			}
		
			if (v.size() != sizeof(fc::ripemd160) + 5)
				return false;
			auto check = fc::sha256::hash(v.data(), v.size() - 4);
			check = fc::sha256::hash(check); // double
			if (memcmp(v.data() + 21, (char*)&check, 4) != 0)
				return false;

			return true;
		}

		address::address(const fc::ecc::public_key& pub, unsigned char version)
		{
			auto dat = pub.serialize();
			auto sha2 = ::fc::sha256::hash(dat.data, sizeof(dat));
			auto rep = fc::ripemd160::hash((char*)&sha2, sizeof(sha2));
            this->version = version;
			memcpy(addr.data, (char*)&rep, sizeof(rep));
		}

		std::string address::address_to_string() const
		{
			string prefix = GRAPHENE_ADDRESS_PREFIX;
			if (testnet_mode)
				prefix = GRAPHENE_ADDRESS_TESTNET_PREFIX;
			if (*this == address())
				return ADDRESS_NOT_INITED;
			fc::array<char, 25> bin_addr;
            memcpy((char*)& bin_addr, (char*)& version, 1);
			memcpy((char*)&bin_addr + 1, (char*)&addr, sizeof(addr));

			auto check = fc::sha256::hash(bin_addr.data, bin_addr.size() - 4);
			check = fc::sha256::hash(check); // double
			memcpy(((char*)&bin_addr) + 21, (char*)&check, 4);
			return  fc::to_base58(bin_addr.data, sizeof(bin_addr));
		}
		address::address(const pts_address& ptsaddr)
		{
			memcpy(addr.data, ptsaddr.addr.data, sizeof(addr));
		}
		address::address(const fc::ripemd160& rip, unsigned char version)
		{
			this->version = version;
			memcpy(addr.data, (char*)&rip, sizeof(rip));
		}
		address::address(const fc::ecc::public_key_data& pub, unsigned char version)
		{
			auto dat = fc::ecc::public_key(pub).serialize();
			auto sha2 = ::fc::sha256::hash(dat.data, sizeof(dat));
			auto rep = fc::ripemd160::hash((char*)&sha2, sizeof(sha2));
			this->version = version;
			memcpy(addr.data, (char*)&rep, sizeof(rep));
		}
		address::address(const graphene::chain::public_key_type& pub, unsigned char version)
		{
			if (pub == graphene::chain::public_key_type())
				return ;
			auto dat = pub.operator fc::ecc::public_key().serialize();
			auto sha2 = ::fc::sha256::hash(dat.data, sizeof(dat));
			auto rep = fc::ripemd160::hash((char*)&sha2, sizeof(sha2));
			this->version = version;
			memcpy(addr.data, (char*)&rep, sizeof(rep));
		}

		address::operator std::string()const
		{
			return address_to_string();
		}
	}
}
namespace fc
{
	void to_variant(const graphene::chain::address& var, variant& vo)
	{
		vo = std::string(var);
	}
	void from_variant(const variant& var, graphene::chain::address& vo)
	{
		vo = graphene::chain::address(var.as_string());
	}
}
