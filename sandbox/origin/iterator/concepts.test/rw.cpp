// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <vector>
#include <memory>

#include <origin/iterator/concepts.hpp>

using namespace std;
using namespace origin;

int main()
{
	using V = vector<int>;

	static_assert(Readable<int*>(), "");
	static_assert(Readable<V::iterator>(), "");
	static_assert(Readable<V::const_iterator>(), "");

	static_assert(Move_writable<int*, int>(), "");
	static_assert(Move_writable<V::iterator, int>(), "");
	static_assert(!Move_writable<V::const_iterator, int>(), "");
	static_assert(Permutable<V::iterator>(), "");
	static_assert(!Permutable<V::const_iterator>(), "");
	
	static_assert(Copy_writable<int*, int>(), "");
	static_assert(Copy_writable<V::iterator, int>(), "");
	static_assert(!Copy_writable<V::const_iterator, int>(), "");
	static_assert(Mutable<V::iterator>(), "");
	static_assert(!Mutable<V::const_iterator>(), "");

	using V2 = vector<unique_ptr<int>>;
	static_assert(Move_writable<V2::iterator, unique_ptr<int>&&>(), "");
	static_assert(!Copy_writable<V2::iterator, const unique_ptr<int>&>(), "'");
	static_assert(Permutable<V2::iterator>(), "");

	static_assert(!Readable<int>(), "");
}
