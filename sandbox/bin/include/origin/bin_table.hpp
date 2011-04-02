
#ifndef ORIGIN_BIN_TABLE_HPP
#define ORIGIN_BIN_TABLE_HPP

#include <vector>
#include <list>
#include <stdexcept>

#include <origin/containers.hpp>

#include "bin_free_functions.hpp"

namespace origin {

/**
 * The bin_table class implements a binning facility. In a sense, binning is
 * similar to hashing in that integer values are associated with objects.
 * However, bin numbers are not probabilistically unique identifiers for the
 * objects they describe. As such, binning is more applicable for the
 * categorization of objects or discretization of values.
 *
 * This class allows parameterization over the bucket type - the container
 * into which objects are binned.
 *
 * @internal
 * @note We have to track the size independently of the buckets so that the
 * size() can be computed in constant time.
 *
 * @todo Consider optimizations on the bucket type. For example, this could
 * default to counted_list - even forcing std::list/forward_list to be wrapped
 * if explicitly specified. Basically, the bucket type should have a constant
 * time size.
 *
 * @todo Support resize policies for the bucket vector as a template parameter.
 *
 * @todo Is it really worth parameterizing the bucket vector? For consideration,
 * even std::hashtable is built over Node** and not vector<Node*>. This
 * reduces the dependence on vector, but adds a little extra overhead. Note that
 * the Node** is actually an intrusive list that implements chaining.
 */
template <
    typename T,
    typename Binner,
    template <typename...> class Bucket = std::list,
    typename Alloc = std::allocator<T>>
class bin_table {
    typedef Bucket<T, Alloc> BucketType;
    typedef std::vector<BucketType> BucketsType;
public:
    typedef typename Alloc::template rebind<T>::other allocator_type;
    typedef typename Alloc::reference reference;
    typedef typename Alloc::const_reference const_reference;
    typedef typename Alloc::pointer pointer;
    typedef typename Alloc::const_pointer const_pointer;
    typedef typename Alloc::size_type size_type;
    typedef Binner binner;

    /** @name Constructors and Destructor */
    //@{
    bin_table()
        : _binner(), _buckets()
    { }

    bin_table(bin_table const& x)
        : _binner(x._binner), _buckets(x._buckets)
    { }

    bin_table(bin_table&& x)
        : _binner(std::forward<Binner>(x._binner))
        , _buckets(std::forward<BucketsType>(x._buckets))
    { }

    /** Create a bin table with n initial buckets. */
    bin_table(size_type n)
        : _binner(), _buckets(n)
    { }
    //@}

    /** @name Insert
     * The insert function uses the into bin to insert an object using
     * the push dispatch from core. This is the move version of the insert.
     *
     * @todo What if the bucket type is a unique associative container? That
     * makes it possible that the container insertion will fail (i.e. as with
     * singleton_set). We can't use container_insert for all cases.
     */
    //@{
    void insert(T&& x)
    { container_insert(_buckets[bucket(x)], x); }

    void insert(T const& x)
    { container_insert(_buckets[bucket(x)], x); }

    //@}

    /** @name Iterators
     * Return begin and end iterators over the elements in the bin table.
     */
    //@{
    /*
    iterator begin() { return _buckets.begin(); }
    iterator end() { return _buckets.end(); }

    const_iterator begin() const { return _buckets.begin(); }
    const_iterator end() const { return _buckets.end(); }
    */
    //@}

    /** @name Size and Capacity */
    //@{
    bool empty() const { return _buckets.empty();  }
    size_type size() const { return 0; }
    //@}

    /** @name Buckets */
    //@{
    /** Return the number of buckets. */
    size_type bucket_count() const { return _buckets.size(); }

    /** Return the maximum number of allowable buckets. */
    size_type max_bucket_count() const { return _buckets.max_size(); }

    /**
     * Return the size of the given bucket number.
     * @note When buckets are implemented by std::list or std::forward_list,
     * then this function is O(n).
     */
    size_type bucket_size(size_type n) const { return _buckets[n].size(); }

    /** Return the bucket number for the given object. */
    size_type bucket(T const& x) const {
        std::size_t n = _binner(x);
        if(n >= _buckets.size()) {
            throw std::out_of_range("Invalid bucket number");
        }
        return n;
    }
    //@}

    /** @name Observers */
    //@{
    allocator_type get_allocator() const { return allocator_type(); }
    binner bin_function() const { return _binner; }
    //@}

private:
    Binner _binner;
    BucketsType _buckets;
    size_type _size;
};

} // namespace origin

#endif
