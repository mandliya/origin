
#ifndef ORIGIN_BIN_FUNCTIONS_HPP
#define ORIGIN_BIN_FUNCTIONS_HPP

namespace origin {

// vector of lists 1
template<typename T, typename outer_container, typename Outer_Alloc>
void into_bin(std::vector<outer_container, Outer_Alloc>& in_bins, unsigned int index, T const & in_value)
{
    if(in_bins.size() <= index) {
        in_bins.resize(index+1);
    }
    origin::container_insert(in_bins[index], in_value);
}

template<typename T, typename outer_container, typename Outer_Alloc>
void into_bin(std::vector<outer_container, Outer_Alloc>& in_bins, unsigned int index, T&& in_value)
{
    if(in_bins.size() <= index){
        in_bins.resize(index+1);
    }
    origin::container_insert(in_bins[index], in_value);
}

// array of lists 1
template<typename T, size_t N, typename list_alloc, typename outer_container>
void into_bin(std::array<outer_container,N>& in_bins, unsigned int index, T const& in_value)
{
    origin::container_insert(in_bins[index], in_value);
}

//array of lists 2
template<typename T, size_t N, typename list_alloc, typename outer_container>
void into_bin(std::array<outer_container,N>& in_bins, unsigned int index, T&& in_value)
{
    origin::container_insert(in_bins[index], in_value);
}

/**
 * Starting from the position of bucket_iter look for the next non-empty bucket.
 */
template<typename ContainerOfContainers, typename in_iter>
in_iter
find_next_empty_outer(ContainerOfContainers& container, in_iter iter)
{
    while(iter != container.end() )
    {
        iter++;
        if(!iter->empty())
        {
            break;
        }
    }
    return iter;
}

template<typename ContainerOfContainers, typename in_iter>
in_iter
find_prev_empty_outer(ContainerOfContainers& container, in_iter iter)
{
    while(iter != container.begin() )
    {
        iter--;
        if(!iter->empty()) {
            break;
        }
    }
    return iter;
}


/** @name striding_iterator
 *  Iterator for traversing a container filled with other containers of elements.
 *  The behavior of this class is to provide the notion of all the elements being
 *  all right next to one another.
 */
template<typename bucket_type, typename bin_type>
class strider
{
    public:
    
    /** Iterator types for containers. */
    typedef typename bucket_type::iterator bucket_iterator;
    typedef typename bin_type::iterator    bin_iterator;

    
    strider()
        : bucket_pointer(), current_bin(), current_element()
    {}

    strider(bucket_type* bucket)
        : bucket_pointer(bucket), current_bin( bucket->begin() ), current_element( bucket->begin()->begin() )
    {}
    
    /***/    
    void
    set_bucket(bucket_type* bucket)
    {
        bucket_pointer = bucket;
        current_bin = bucket->begin();
        current_element = bucket->begin()->begin();
    }
    
    /***/
    bin_iterator
    step_forward()
    {
//        current_bin = find_next_empty_outer(*bucket_pointer, current_bin)
        current_element++;        
        if(current_element == current_bin->end()){
            current_bin = find_next_empty_outer(*bucket_pointer, current_bin);
            current_element = current_bin->begin()->begin();
        }  
        return current_element;   
    }
    
    /***/
    bin_iterator
    step_back()
    {
        if(current_element == (*current_bin).begin()){
            current_bin = find_prev_empty_outer(*bucket_pointer, current_bin);
            current_element = (*current_bin).end()--;
            return current_element;
        }
        else{
            return --current_element;
        }
    }
        
    private:

    bucket_type*    bucket_pointer;
    bucket_iterator current_bin;
    bin_iterator    current_element;
};



} // namespace origin



#endif
