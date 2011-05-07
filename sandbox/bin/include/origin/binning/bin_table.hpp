// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_BINNING_BIN_TABLE_HPP
#define ORIGIN_BINNING_BIN_TABLE_HPP
#include <vector>

namespace origin { namespace binning {

/** @name bin_table<T,BinFunc,Bucket,Alloc>
 *  @brief A historgram/raidax based data structure. This is the semi dynamic
 *  verison of a bin_table. A more static version would use a std::array for
 *  the bin type.
 *
 *  @param T - Item in bucket.
 *  @param BinFunc - The functions which produces the index to the bins of
 *      buckets.
 *  @param BucketType - Bucket which sits inside of each bin and collects items.
 *  @param BinType - bin type is the index which holds the element
 *  
 *  
 *  
 *  
 *
 *  @todo add something which changes the default parameters into maps if 
 *  BinFunc returns a string.
 */
template <
    typename T,
    typename BinFunc,
    typename BucketType = std::vector<T>,
    typename BinType = std::vector<BucketType>
>
class bin_table {
public:
    

    typedef BucketType                              bucket_type;
    typedef BinType                                 bin_type;
    typedef T                                       value_type;
    typedef typename bucket_type::reference         reference;
    typedef typename bucket_type::const_reference   const_refenece;
    typedef typename bucket_type::pointer           pointer;
    typedef typename bucket_type::const_pointer     const_pointer;
    typedef typename bucket_type::size_type         size_type;

    /** @name default constructor.
     *  @brief Default construct a bin_table.
     *  @throws std::bad_alloc 
     */
    bin_table();

    /** @name Copy Constructor. 
     *  @brief Creates a copy of the bin_table.
     *  @param other - The bin table to make a copy of,
     *  @throws std::bad_alloc
     */
    bin_table(bin_table const& other);

    /** @name Move Constructor.
     *  @brief Move the tables from one table into another.
     *  @throws std::bad_alloc
     */
    bin_table(bin_table&& other);


    
private:
};


}} // end origin::binning

#endif
