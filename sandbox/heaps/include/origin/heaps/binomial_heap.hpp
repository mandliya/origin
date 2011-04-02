// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_HEAPS_BINOMIAL_HEAP_HPP
#define ORIGIN_HEAPS_BINOMIAL_HEAP_HPP

#include <vector>
#include <iostream>
#include <unordered_map>

#include <origin/utility/meta.hpp>

namespace origin
{
    /* Forward Declaration */
    struct binomial_heap_node;
    
    /* Function Prototypes */
    void binomial_link (binomial_heap_node& node1, binomial_heap_node& node2, size_t index1, size_t index2);
    
    /*
     * An adaptation of CLRS's Binomial heap implementation
     * in Chapter 19, 3rd Ed, pages .
     */
    
    struct binomial_heap_node
    {
       public:
          size_t item_index;
    
          /* Index of parent of the element in the heap */
          size_t parent;
          
          /* Index of the child element in the heap */
          size_t child;
          
          /* Index of the right neighbour of the element in the heap */
          size_t right_sibling;
          
          /* Degree of the element */
          size_t degree;
          binomial_heap_node ():parent{size_t(-1)}, child{size_t(-1)}, right_sibling{size_t(-1)}, degree{size_t(0)} {};
    };
    
    /* Class: Binomial Heap 
     * Template parameters
     * T : Value type - Type of data to be stored in the heap
     * Compare : Binary function predicate - This gives the ordering between the
     *           element (max/min heap)
     * Item_Map : Property Map - External property map used to find the 
     *                  index of element in the heap
     */
    template <class T, 
              class Compare,
              class Item_Map> // This label is not a final design
    class binomial_heap_impl
    {
       private:
          // FIXME: Value type should be "T const" for mutable priority queues.
          typedef T value_type; //TODO: Will address then when we write Immutable heap 
          typedef size_t size_type;
          typedef std::vector<size_type> IndexArray;
          
          /* Random access container which holds the heap elements */
          std::vector<T> elements_;
          std::vector<binomial_heap_node> data_;
           
          /* Internal map for mapping the values stored in the external map
           * to actual index of the element in the heap
           */
          
          Compare compare_;
          Item_Map id_;
          IndexArray index_array;
          
          /*
           * head_ - index of the root element
           * top_ - index of the top element
           */ 
          
          size_type top_, head_;
         
          /*
           * print_recur: Helper function for displaying the binomial heap
           * Input:
           * size_type x : Index of the element
           * ostresm &os : Reference to the ouput stream
           * Output:
           * Prints the nodes for a particular binomial tree identified by x
           * Return Value: None       
           */
          void print_recur(size_type x, std::ostream& os)
          {
             if (x != -1) {
                os << elements_[data_[x].item_index];
                if (data_[x].degree > 0) {
                   os << "(";
                   size_type i = data_[x].child;
                   do {
                      print_recur (i, os);
                      os << " ";
                      i = data_[i].right_sibling;
                   } while (i != -1);
                   os << ")";
                }
             }
          }
          
          /*
           * merge: Function to merge two binomial heaps
           * Input: 
           * size_type x : Index of the root element of the heap to be merged
           * Output:
           * Merges the heap pointed to by x in the main heap pointed to by head_
           * Return Value:
           * None       
           */
          void merge (size_type index);

          /*
           * binomial_heap_union: Unites the heap pointed to by head_ with 
           * the heap specified
           * Input: 
           * size_type index: Index of the root element of the heap to be united
           * Output:
           * United binomial heap
           * Return Value:
           * None       
           */
          void binomial_heap_union (size_type index);
           
          /*
           * get_new_top: Function to find the index of new root element
           * Input: 
           * None
           * Output:
           * Index of the new root
           * Return Value:
           * size_type: Index of the new root
           */
          size_type get_new_top(); 
          
       public:
          /*
           * binomial_heap_impl: Default constructor
           * Input: 
           * None
           * Output:
           * Return Value:
           * None       
           */
          binomial_heap_impl () {};
          
          /*
           * binomial_heap_impl: 3 argument constructor
           * Input: 
           * size_type n: number of elements in the heap initially
           * Compare &cmp: comparison function predicate
           * Item_Map: Lambda function for map
           * Output:
           * Instantiates a heap of n elements with given comparison function
           * and property map
           * Return Value:
           * None
           */
          binomial_heap_impl (size_type n,
                const Compare &cmp, const Item_Map& id) :
                compare_{cmp}, id_{id}, index_array(n), top_{-1}, head_{-1} 
          { 
          }
          
          /*
           * binomial_heap_impl: range based constructor
           * Input: 
           * ForwardIterator first: Iterator to the first element of a container 
           * ForwardIterator last: Iterator to the last element of a container
           * Compare &cmp: comparison function predicate
           * Item_Map: Lambda function for map
           * Output:
           * Instantiates a heap of n elements with given comparison function
           * and property map
           * Return Value:
           * None       
           */
          template<typename ForwardIterator>
             binomial_heap_impl (ForwardIterator first, ForwardIterator last,
                   const Compare &cmp, const Item_Map& id) :
                   index_array(std::distance(first, last)), compare_{cmp}, 
                   id_{id}, top_{-1}, head_{-1}
          {
             while(first != last) {
                push(*first);
                ++first;
             }
          }
          
          /*
           * print: Function for displaying the binomial heap
           * Input:
           * ostresm &os : Reference to the ouput stream
           * Output:
           * Outputs the binomial heap to the specified output stream
           * Return Value:
           * None       
           * Note: This a helper function developed for unit testing
           */
          template<typename Char, typename Traits>
             void print(std::basic_ostream<Char, Traits>& os);
          
          
          /*
           * Update: Updates the given element in the heap
           * Input: 
           * value_type &d: Reference to element which has to be updated
           * Output:
           * Updated heap
           * Return Value:
           * None       
           * Precondition: Element d must already be updated in the map
           */
          void update(const value_type& d);
          
          /*
           * push: Insets the given element in the heap
           * Input: 
           * value_type &d: Reference to element which has to be inserted
           * Output:
           * Heap with the new element inserted
           * Return Value:
           * None       
           * Precondition: Element d must already be present in the map
           */
          void push(const value_type& d); 
          
         
          /*
           * top: Function to return the top element of the heap
           * Input: 
           * None
           * Output:
           * top element
           * Return Value:
           * value_type &: Reference to the top element is retured
           */
          value_type& top()
          {
             return elements_[data_[top_].item_index];
          }
          
          /*
           * top: Constant Function to return the const top element of the heap
           * Input: 
           * None
           * Output:
           * top element
           * Return Value:
           * value_type &: Reference to the top element is retured
           */
          const value_type& top() const
          {
             return elements_[data_[top_].item_index];
          }
          
          /*
           * empty: Function to check for empty heap
           * Input: 
           * None
           * Output:
           * State of the heap (empty/notempty)
           * Return Value:
           * bool : True if heap is empty, False otherwise
           */
          bool empty() const
          {
             return elements_.size()==0;
          }
          
          /*
           * size: Function to find the size of the heap
           * Input: 
           * None
           * Output:
           * Number of elements in the heap
           * Return Value:
           * size_type : Number of elements
           */
          inline size_type size() const
          {
             return elements_.size();
          }
          
          /*
           * pop: Removes the top element from the heap
           * Input: 
           * None
           * Output:
           * binomial heap with a new top element
           * Return Value:
           * None
           */
          void pop();
    };
    
    template <class T, 
              class Compare,
              class Item_Map>
    typename binomial_heap_impl<T, Compare, Item_Map>::size_type 
    binomial_heap_impl<T, Compare, Item_Map>::get_new_top()
    {
       size_type top_index;
       size_type tmp;
       if (head_ == size_type(-1)) {
          return -1;
       }
       
       top_index = head_;
       tmp = data_[top_index].right_sibling;
       
       while (tmp != size_type(-1)) {
          if (compare_(elements_[data_[tmp].item_index], elements_[data_[top_index].item_index])) {
             top_index = tmp;
          }
          
          tmp = data_[tmp].right_sibling;
       }
       
       return top_index;
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void binomial_heap_impl<T, Compare, Item_Map>::push(const value_type& d)
    {
       binomial_heap_node obj;
       size_type index;
      
       elements_.push_back(d);
    
       obj.item_index = elements_.size() - 1;
       obj.parent = size_type(-1);
       obj.right_sibling = size_type(-1);
       obj.child = size_type(-1);
       obj.degree = 0;
       
       data_.push_back(obj);
       
       index = data_.size() - 1;
       
       index_array[id_(d)] = index;
       if (head_ == size_type(-1)) {
          /* New heap */
          head_ = index;
          top_ = head_;
       } else {
          /* Unite the 1 element heap with the existing heap */
          binomial_heap_union(index);
          if (compare_(elements_[data_[index].item_index], elements_[data_[top_].item_index]))
             top_ = index;
       }
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void binomial_heap_impl<T, Compare, Item_Map>::update(const value_type& d)
    {
       size_type index = index_array[id_(d)];
       size_type parent = data_[index].parent;
       size_type temp;
       elements_[data_[index].item_index] = d;
       
       while (parent != size_type(-1) && 
             compare_(d, elements_[data_[parent].item_index])) {
          elements_[data_[index].item_index] = elements_[data_[parent].item_index];
          elements_[data_[parent].item_index] = d;
          temp = id_(elements_[data_[index].item_index]);
          index_array[temp] = index;
          
          index = parent;
          parent = data_[parent].parent;
       }
       
       temp = id_(d);
       
       index_array[temp] = index;
       
       if (compare_(d, elements_[data_[top_].item_index])) {
          top_ = index;
       }
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void binomial_heap_impl<T, Compare, Item_Map>::merge (size_type index)
    {
       size_type p = head_;
       size_type q = index;
       
       size_type current = data_.size();
       size_type initial_size = -1;
       data_.push_back(binomial_heap_node());
       
       while (p != initial_size || q != initial_size) {
          if (q == initial_size || 
                (p != initial_size && data_[p].degree < data_[q].degree)) {
             data_[current].right_sibling = p;
             current = p;
             p = data_[p].right_sibling;
          } else {
             data_[current].right_sibling = q ;
             current = q;
             q = data_[q].right_sibling;
          }
       }
       
       head_ = data_.back().right_sibling;
       data_.pop_back();
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void binomial_heap_impl<T, Compare, Item_Map>::binomial_heap_union (size_type index)
    {
       /* Merge the root lists */
       merge(index);
       if (data_[head_].right_sibling == size_type(-1)) {
          return;
       }
       
       size_type x = head_;
       size_type next_x = data_[head_].right_sibling;
       size_type prev_x = size_type(-1);
       
       while (next_x != size_type(-1)) {
          if ((data_[x].degree != data_[next_x].degree) 
                || (data_[next_x].right_sibling != size_type(-1) 
                   && data_[data_[next_x].right_sibling].degree 
                   == data_[x].degree)) {
             prev_x = x;
             x = next_x;
          } else {
             if (compare_ (elements_[data_[x].item_index], elements_[data_[next_x].item_index])) {
                data_[x].right_sibling = data_[next_x].right_sibling;
                binomial_link(data_[next_x], data_[x], next_x, x);
             } else {
                if (prev_x == size_type (-1)) {
                   head_ = next_x;
                } else {
                   data_[prev_x].right_sibling = next_x;
                }
                
                binomial_link(data_[x], data_[next_x], x, next_x);
                x = next_x;
             }
          }
          
          next_x = data_[x].right_sibling;
       }
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void binomial_heap_impl<T, Compare, Item_Map>::pop()
    {
       size_type new_head = size_type(-1);
       
       if (head_ == size_type(-1)) {
          return;
       }
      
       size_type temp = top_;
    
       /* Reverse the list */
       if (data_[top_].child != size_type(-1)) {
          size_type tmp_head = data_[top_].child;
          size_type tmp_sibling;
          
          while (tmp_head != size_type(-1)) {
             tmp_sibling = data_[tmp_head].right_sibling;
             data_[tmp_head].parent = size_type(-1);
             data_[tmp_head].right_sibling = new_head;
             new_head = tmp_head;
             tmp_head = tmp_sibling;
          }
       }
       
       if (data_[head_].right_sibling == size_type(-1)) {
          head_ = new_head;
       } else {
          if (head_ == top_) {
             head_ = data_[head_].right_sibling;
          } else {
             size_type tmp = head_;
             while (data_[tmp].right_sibling != top_) {
                tmp = data_[tmp].right_sibling;
             }
             
             data_[tmp].right_sibling = data_[top_].right_sibling;
          }
          
          binomial_heap_union(new_head);
       }
       
       top_ = get_new_top();
    
       // where in data_ old top element was stored
       size_type index = index_array[id_(elements_[elements_.size()-1])];
    
       // copy the last element to location of old root
       elements_[temp] = elements_[elements_.size()-1];
    
       //point the item_index of the old element to correct location
       data_[index].item_index = temp;
    
       // Invalidating the entries of node
       data_[temp].parent = -1;
       data_[temp].child = -1;
       data_[temp].right_sibling = -1;
       data_[temp].degree = 0;
    
       elements_.pop_back();
    
    }
    
    template <class T, 
              class Compare,
              class Item_Map>
       template<typename Char, typename Traits>
    void binomial_heap_impl<T, Compare, Item_Map>::print(std::basic_ostream<Char, Traits>& os)
    {
       if (head_ != -1) {
          size_type i = head_;
          do {
             print_recur(i, os);
             os << std::endl;
             i = data_[i].right_sibling;
          } while (i != -1);
       }
    }
    
    
    /*
     * binomial_link: Links the specified nodes in the heap
     * Input: 
     * size_type y: element in the heap
     * size_type z: element in the heap
     * Output:
     * links element y and z in the heap
     * Return Value:
     * None
    */
    void binomial_link (binomial_heap_node& node1, binomial_heap_node& node2, size_t index1, size_t index2)
    {
       node1.parent = index2;
       node1.right_sibling = node2.child;
       node2.child = index1;
       node2.degree = node2.degree + 1;
    }

    template <class T, 
              class Compare,
              class Item_Map = default_t> // This label is not a final design
    class binomial_heap
    {
        public:
            typedef T value_type;
            typedef size_t size_type;
        
        protected:
            typedef binomial_heap_impl<value_type, Compare, Item_Map> search_impl;
            
        public:
            binomial_heap(): impl() 
            {}
            
            binomial_heap (size_type n,
                const Compare& cmp, const Item_Map& id) : impl(n, cmp, id)
            {}
            
            template<typename ForwardIterator>
            binomial_heap (ForwardIterator first, ForwardIterator last,
                           const Compare& cmp, const Item_Map& id):
                           impl (first, last, cmp, id)
            {}
 
            void update(const value_type& d) 
            { impl.update(d); }
          
            void push(const value_type& d)
            { impl.push(d); }
          
            value_type& top()
            {
               return impl.top(); 
            }
          
            const value_type& top() const
            {
                return impl.top();
            }
          
            bool empty() const
            {
               return impl.empty(); 
            }
          
            inline size_type size() const
            {
               return impl.size();
            }
          
            void pop()
            { return impl.pop();}
          
        protected:
            search_impl impl;

    };


    //FIXME: This implementation is not correct

    template <class T, 
              class Compare>
    class binomial_heap<T, Compare, default_t>
    {
 
        public:
            typedef T value_type;
            typedef size_t size_type;
        
        protected:
            typedef typename std::unordered_map<value_type, size_type> internal_map;
            struct item_map {
                internal_map imap;

                public:
                item_map (internal_map &map): imap(map) 
                {}
                
                template<typename value_type>
                size_type operator() (const value_type& key)
                {
                    return imap[key];
                }
            };


            typedef binomial_heap_impl<value_type, Compare, item_map> search_impl;

        public:
            binomial_heap() 
            {};
            
            binomial_heap (size_type n,
                const Compare& cmp): id_(map_), impl (n, cmp, id_)
              
            {}
            
            template<typename ForwardIterator>
            binomial_heap (ForwardIterator first, ForwardIterator last,
                           const Compare& cmp): id_(map_),
                           impl (first, last, cmp, id_)
            {}
 
            /*
             * The assumption is that heap never stores the actual value. It 
             * contains reference to the value, and the reference remains 
             * unchanged during update
             */
            void update(const value_type& d)
            {
                impl.update(d); 
            }
          
            void push(const value_type& d)
            {   
                map_[d] = impl.size();
                impl.push(d); 
            }
          
            value_type& top()
            {
               return impl.top(); 
            }
          
            const value_type& top() const
            {
                return impl.top();
            }
          
            bool empty() const
            {
               return impl.empty(); 
            }
          
            inline size_type size() const
            {
               return impl.size();
            }
          
            void pop()
            {   
                //map_.erase(impl.top);
                return impl.pop();
            }
          
        protected:
            internal_map map_;
            item_map id_;
            search_impl impl;

    };
}
#endif // ORIGIN_HEAPS_BINOMIAL_HEAP_HPP
