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
#include <graphene/chain/witness_evaluator.hpp>
#include <graphene/chain/witness_object.hpp>
#include <graphene/chain/committee_member_object.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/protocol/vote.hpp>

namespace graphene { namespace chain {

void_result miner_create_evaluator::do_evaluate( const miner_create_operation& op )
{ try {
	auto miner_obj = db().get(op.miner_account);
	FC_ASSERT(miner_obj.addr == op.miner_address, "the address is not correct");
	//account cannot be a guard
    auto & iter = db().get_index_type<guard_member_index>().indices().get<by_account>();
    FC_ASSERT(iter.find(op.miner_account) == iter.end(),"account cannot be a guard.");
   
    auto & iter_miner = db().get_index_type<miner_index>().indices().get<by_account>();
    FC_ASSERT(iter_miner.find(op.miner_account) == iter_miner.end(),"the account has beeen a miner.");

    return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

object_id_type miner_create_evaluator::do_apply( const miner_create_operation& op )
{ try {
   vote_id_type vote_id;
   db().modify(db().get_global_properties(), [&vote_id](global_property_object& p) {
      vote_id = get_next_vote_id(p, vote_id_type::witness);
   });

   const auto& new_miner_object = db().create<miner_object>( [&]( miner_object& obj ){
         obj.miner_account  = op.miner_account;
         obj.signing_key      = op.block_signing_key;
         obj.vote_id          = vote_id;
         obj.url              = op.url;
   });
   return new_miner_object.id;
} FC_CAPTURE_AND_RETHROW( (op) ) }

void miner_create_evaluator::pay_fee()
{
	FC_ASSERT(core_fees_paid.asset_id == asset_id_type());
	db().modify(db().get(asset_id_type()).dynamic_asset_data_id(db()), [this](asset_dynamic_data_object& d) {
		d.current_supply -= this->core_fees_paid.amount;
	});
}


void_result witness_update_evaluator::do_evaluate( const witness_update_operation& op )
{ try {
   FC_ASSERT(db().get(op.witness).miner_account == op.witness_account);
   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result witness_update_evaluator::do_apply( const witness_update_operation& op )
{ try {
   database& _db = db();
   _db.modify(
      _db.get(op.witness),
      [&]( miner_object& wit )
      {
         if( op.new_url.valid() )
            wit.url = *op.new_url;
		 if (op.new_signing_key.valid())
		 {
			 wit.signing_key = *op.new_signing_key;
			 wit.last_change_signing_key_block_num = _db.head_block_num()+1;
		 }
      });
   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }


} } // graphene::chain
