/*
 * Copyright (c) 2003-2007, John Wiegley.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of New Artisans LLC nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _BALPAIR_H
#define _BARPAIR_H

#include "balance.h"

namespace ledger {

class balance_pair_t
  : public equality_comparable<balance_pair_t,
           equality_comparable<balance_pair_t, balance_t,
           equality_comparable<balance_pair_t, amount_t,
           additive<balance_pair_t,
           additive<balance_pair_t, balance_t,
           additive<balance_pair_t, amount_t,
           multiplicative<balance_pair_t, unsigned long,
           multiplicative<balance_pair_t, long,
	   multiplicative<balance_pair_t, amount_t> > > > > > > > >
{
  balance_t quantity;
  optional<balance_t> cost;

  friend class value_t;
  friend class entry_base_t;

public:
  // constructors
  balance_pair_t() {
    TRACE_CTOR(balance_pair_t, "");
  }
  balance_pair_t(const balance_pair_t& bal_pair)
    : quantity(bal_pair.quantity), cost(bal_pair.cost) {
    TRACE_CTOR(balance_pair_t, "copy");
  }
  balance_pair_t(const balance_t& _quantity)
    : quantity(_quantity) {
    TRACE_CTOR(balance_pair_t, "const balance_t&");
  }
  balance_pair_t(const amount_t& _quantity)
    : quantity(_quantity) {
    TRACE_CTOR(balance_pair_t, "const amount_t&");
  }
  ~balance_pair_t() {
    TRACE_DTOR(balance_pair_t);
  }

  // assignment operator
  balance_pair_t& operator=(const balance_pair_t& bal_pair) {
    if (this != &bal_pair) {
      quantity = bal_pair.quantity;
      cost     = bal_pair.cost;
    }
    return *this;
  }

  // in-place arithmetic
  balance_pair_t& operator+=(const balance_pair_t& bal_pair) {
    if (bal_pair.cost && ! cost)
      cost = quantity;
    quantity += bal_pair.quantity;
    if (cost)
      *cost += bal_pair.cost ? *bal_pair.cost : bal_pair.quantity;
    return *this;
  }
  balance_pair_t& operator-=(const balance_pair_t& bal_pair) {
    if (bal_pair.cost && ! cost)
      cost = quantity;
    quantity -= bal_pair.quantity;
    if (cost)
      *cost -= bal_pair.cost ? *bal_pair.cost : bal_pair.quantity;
    return *this;
  }

  // comparison
  bool operator==(const balance_pair_t& bal_pair) const {
    return quantity == bal_pair.quantity;
  }
  bool operator==(const balance_t& bal) const {
    return quantity == bal;
  }
  bool operator==(const amount_t& amt) const {
    return quantity == amt;
  }

  balance_pair_t& operator*=(const amount_t& amt) {
    quantity *= amt;
    if (cost)
      *cost *= amt;
    return *this;
  }
  balance_pair_t& operator/=(const amount_t& amt) {
    quantity /= amt;
    if (cost)
      *cost /= amt;
    return *this;
  }

  // unary negation
  void in_place_negate() {
    quantity.in_place_negate();
    if (cost)
      cost->in_place_negate();
  }
  balance_pair_t negate() const {
    balance_pair_t temp = *this;
    temp.in_place_negate();
    return temp;
  }
  balance_pair_t operator-() const {
    return negate();
  }

  // test for non-zero (use ! for zero)
  operator bool() const {
    return quantity;
  }

  bool is_realzero() const {
    return ((! cost || cost->is_realzero()) && quantity.is_realzero());
  }

  balance_pair_t abs() const {
    balance_pair_t temp = *this;
    temp.quantity = temp.quantity.abs();
    if (temp.cost)
      temp.cost = temp.cost->abs();
    return temp;
  }

  optional<amount_t>  amount(const optional<const commodity_t&>& commodity =
			     optional<const commodity_t&>()) const {
    return quantity.amount(commodity);
  }
  optional<balance_t> value(const optional<moment_t>& moment =
			    optional<moment_t>()) const {
    return quantity.value(moment);
  }

  balance_t
  strip_annotations(const bool keep_price = amount_t::keep_price,
		    const bool keep_date  = amount_t::keep_date,
		    const bool keep_tag   = amount_t::keep_tag) const {
    return quantity.strip_annotations(keep_price, keep_date, keep_tag);
  }

  void print(std::ostream& out, const int first_width,
	     const int latter_width = -1) const {
    quantity.print(out, first_width, latter_width);
  }

  balance_pair_t& add(const amount_t&  amt,
		      const optional<amount_t>& a_cost = optional<amount_t>()) {
    if (a_cost && ! cost)
      cost = quantity;
    quantity += amt;
    if (cost)
      *cost += a_cost ? *a_cost : amt;
    return *this;
  }

  bool valid() {
    return quantity.valid() && (! cost || cost->valid());
  }

  void in_place_reduce() {
    quantity.in_place_reduce();
    if (cost) cost->in_place_reduce();
  }
  balance_pair_t reduce() const {
    balance_pair_t temp(*this);
    temp.in_place_reduce();
    return temp;
  }

  void in_place_round() {
    quantity = quantity.round();
    if (cost)
      cost = cost->round();
  }
  balance_pair_t round() const {
    balance_pair_t temp(*this);
    temp.in_place_round();
    return temp;
  }

  balance_pair_t unround() const {
    balance_pair_t temp(quantity.unround());
    if (cost)
      temp.cost = cost->unround();
    return temp;
  }

  friend std::ostream& operator<<(std::ostream& out,
				  const balance_pair_t& bal_pair);
};

inline std::ostream& operator<<(std::ostream& out,
				const balance_pair_t& bal_pair) {
  bal_pair.quantity.print(out, 12);
  return out;
}

} // namespace ledger

#endif // _BALPAIR_H
