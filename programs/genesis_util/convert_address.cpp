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

/**
 * Convert BTC / PTS addresses to a Graphene address.
 */

#include <graphene/chain/pts_address.hpp>
#include <graphene/chain/protocol/address.hpp>
#include <graphene/utilities/key_conversion.hpp>
#include <fc/crypto/elliptic.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <iostream>
#include <string>
#include <leveldb/db.h>

using namespace graphene::chain;

int main(int argc, char** argv)
{
   // grab 0 or more whitespace-delimited PTS addresses from stdin
  
	//auto prk = fc::ecc::private_key::generate();
	//auto exprk = fc::ecc::extended_private_key(prk, fc::sha256()).derive_child(0);

	//auto result = fc::ecc::private_key::regenerate(fc::sha256::hash(exprk.to_base58()));
	leveldb::Options options;
	//options.block_cache = leveldb::NewLRUCache(100 * 1048576);
	options.create_if_missing = true;
	leveldb::DB * db = NULL;
	string path = "E:/coding/Graphene/DFD/bin/programs/test/data/blockchain/undo_db_storage";
	auto open_status = leveldb::DB::Open(options, path, &db);
	FC_ASSERT(open_status.ok());

	leveldb::ReadOptions op;
	string value;
	auto status = db->Get(op, "0c1c46319b79ab5c765b8d3c44fda4c645b5c4a8", &value);
	FC_ASSERT(status.ok());
	std::cout << std::endl;

   return 0;
}
