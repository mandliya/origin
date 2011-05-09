// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_BINNING_BIN_TABLE_HPP
#define ORIGIN_BINNING_BIN_TABLE_HPP
#include <memory>
#include <vector>
#include <type_traits>
#include <initializer_list>
#include <utility>

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
 *  @note this class is actually missing the allocator typedef which is present 
 *  inside of most containers. This is because this container takes containers
 *  as parameters.
 *
 *  @todo add something which changes the default parameters into maps if 
 *  BinFunc returns a string.
 *
 *  @todo Work with the value_type to ensure that values are passed by value  
 *  when pod and small enough, and const& other wise.
 *  Functions/Constructors to keep in mind :
 *      self& operator=(std::initializer_list<value_type const&> init_list);
 *      explicit bin_table(std::initializer_list<value_type const&> init_list);
 *
 *
 *
 *
 *
 *
 *
 *
 */

template <
    typename T,
    typename BinFunc,
    typename BucketType = std::vector<T>,
    typename BinType = std::vector<BucketType>
>
class bin_table {
    typedef bin_table<T,BinFunc,BucketType,BinType> self;

public:
    

    typedef BucketType                              bucket_type;
    typedef BinType                                 bin_type;
    typedef T                                       value_type;
    typedef typename bucket_type::reference         reference;
    typedef typename bucket_type::const_reference   const_refenece;
    typedef typename bucket_type::pointer           pointer;
    typedef typename bucket_type::const_pointer     const_pointer;
    typedef typename bucket_type::size_type         size_type;


    /** @name bin and bucket iterators
     *  @brief bin and bucket type iterators. These are used used mainly during
     *  the implementation of this container.
     */
    /// {
    typedef typename bin_type::iterator             bin_iterator;
    typedef typename bin_type::const_iterator       bin_const_iterator;
    typedef typename bucket_type::iterator          bucket_iterator;
    typedef typename bucket_type::const_iterator    bucket_const_iterator;
    /// }

    /** @name forward constructor
     *  @brief forwards all arguments the the base/impl type of this class.
     */
    // template<typename... Args>
    // bin_table(Args... args);

    /** @name forward assignment operator.
     *  @brief forwards the the assignment call to the base class.
     */
    //template<typename Arg>
    // self& operator=(Arg arg);

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
    explicit bin_table(bin_table const& other);

    /** @name Move Constructor.
     *  @brief Move the tables from one table into another.
     *  @throws this should be no throw.
     */
    explicit bin_table(bin_table&& other);

    /** @name initializer_list constructors
     *  @brief Constructors which accept different initializer lists.
     */
    /// {

    /** @name value_type Copy initializer_list Constructor.
     *  @brief Copies all of the elements from the initialiser_list into the
     *  bin_table.
     *  @throws std::bad_alloc
     */
    explicit bin_table(std::initializer_list<value_type const&> init_list);

    /** @name value_type Move initializer_list Constructor.
     *  @brief Moves all of the elements from the initialiser_list into the
     *  bin_table.
     *  @throws std::bad_alloc
     */
    explicit bin_table(std::initializer_list<value_type&&> init_list);

    /** @name bucket_type Copy initializer_list Constructor.
     *  @brief Copies all of the different bucket_types into the bin_table.
     *  @throws std::bad_alloc
     */
    explicit bin_table(std::initializer_list<typename BinType::value_type const&> init_list);
    /// }


    /** @name operators 
     *  @brief Basic operators used with this container.
     */
    /// {

    /** @name Copy Assignment Operator
     *  @brief Create a copy of other and assign it's value to this container.
     *  @param other the container to make a copy of.
     *  @returns A reference to this object.
     *  @throws std::bad_alloc or any exception thrown by a copy constructor of
     *  either type T, BinType or BucketType.
     */
    self& operator=(self const& other);

    /** @name Move Assignment Operator
     *  @brief Move assignment operator.
     *  @param other the object for which this bin_table will gain owner ship of.
     *  @returns A reference to this object.
     *  @throws Shouldn't ever throw.
     */
    self& operator=(self&& other);

    /** @name initializer_list Assignement Operator
     *  @brief allows for the assignment of an initilizer_list to the current
     *  bin_table. This does remove all previous element.
     *  @returns A reference to this.
     *  @throws std::bad_alloc or any exception thrown by a copy constructor of
     *  either type T, BinType or BucketType.
     */
    self& operator=(std::initializer_list<value_type const&> init_list);

    /** @name value_type R-Value Reference initilizer_list Assignment Operator
     *  @biref Move initializer_list assignment operator.
     *  @returns A refenrence to self.
     *  @throws should be no throw.
     */
    self& operator=(std::initializer_list<value_type&&> init_list);

    /** @name bucket initializer_list assignment.
     *  @biref takes an initializer list of r-value reference buckets to use
     *  use with the container.
     */
    self& operator=(std::initializer_list<typename bin_type::value_type&&> bucket_list);
    /// }
    
private:
};


}} // end origin::binning

#endif
