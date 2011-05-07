#ifndef FREE_HISTOGRAM_FUNC_HPP
#define FREE_HISTOGRAM_FUNC_HPP

#include"sieve.hpp"

/**
 *  This takes two iterators, and iterates from the begenning to the end, inserting each element it iterates over into the 
 * underlying container.
 * @param s is an iterator pointing to the begenning of the elements that you want to be filled into the bin.
 * @param e is an iterator pointing to the one past the end of the elements you want to be filled into the bin.
 * @param hist is the underlying container.
 */
template<typename Iter, typename To_Be_Binned >
void fill_bins(Iter s, Iter e, To_Be_Binned& hist)
{
    //Loop based on an iterator
    // where s is the starting position and e is the ending position
    for(;s!=e ;s++){
        hist.insert(*s);
    }
}

/**
 *  Specialization that allows for sieve to work for both histogram 
 * and binning class.
 */
template<typename T>
void push_bucket(int& i, T const& x)
{i++;}

/** Const verson of verctor push back. */
template<typename Bucket, typename Tt>
void push_bucket(Bucket& bucket, Tt const& x)
{push_dispatch(bucket, x, container_category(bucket) );}

/** Non-const version of vector push_back.*/
template<typename Bucket, typename Tt>
void push_bucket(Bucket& bucket, Tt&& x)
{push_dispatch(bucket, x, container_category(bucket) );}

#endif


