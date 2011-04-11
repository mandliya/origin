// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_HEAPS_FIBONACCI_HEAP_HPP
#define ORIGIN_HEAPS_FIBONACCI_HEAP_HPP

#include <cmath>
#include <array>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <origin/utility/meta.hpp>

namespace origin
{
    /* Forward Declaration */
    struct fibonacci_heap_node;
    
    /* Function Prototypes */
    void fibonacci_link (fibonacci_heap_node&, fibonacci_heap_node&, 
          fibonacci_heap_node&, fibonacci_heap_node&, size_t, size_t);
   
    /* Constant Expressions */
    constexpr double phi() {return (1+sqrt(5))/2;}
    constexpr double rec_log_phi() {return 1/log(phi());} // Reciprocal of naturallog(phi)

    /*
     * An adaptation of CLRS's Fibonacci heap implementation
     * in Chapter 19, 3rd Ed, pages .
     */
    
    struct fibonacci_heap_node
    {
       public:
          size_t item_index;
    
          /* Index of parent of the element in the heap */
          size_t parent;
          
          /* Index of the child element in the heap */
          size_t child;
          
          /* Index of the right neighbour of the element in the heap */
          size_t right_sibling;
          
          /* Index of the left neighbour of the element in the heap */
          size_t left_sibling;
          
          /* Degree of the element */
          size_t degree;
          
          /* Mark of the element */
          bool mark;
          
          fibonacci_heap_node ():parent{-1}, child{-1}, right_sibling{-1}, 
                              left_sibling{-1}, degree{0}, mark{false} {};
    };
    
    /* Class: Mutable Fibonacci Heap 
     * Template parameters
     * T : Value type - Type of data to be stored in the heap
     * Compare : Binary function predicate - This gives the ordering between the
     *           element (max/min heap)
     * Item_Map : Property Map - External property map used to find the 
     *                  index of element in the heap
     */
    template <class T, 
              class Compare,
              class Item_Map> 
    class mutable_fibonacci_heap_impl
    {
       private:
          typedef T value_type;
          typedef size_t size_type;
          typedef std::vector<size_type> IndexArray;
          
          /* Random access container which holds the heap elements */
          std::vector<T> elements_;
          std::vector<fibonacci_heap_node> data_;
           
          /* Internal map for mapping the values stored in the external map
           * to actual index of the element in the heap
           */
          
          Compare compare_;
          Item_Map id_;
          
          /*
           * head_ - index of the root element
           * top_ - index of the top element
           */ 
          
          size_type top_, head_;
         
          /*
           * print_recur: Helper function for displaying the fibonacci heap
           * Input:
           * size_type x : Index of the element
           * ostresm &os : Reference to the ouput stream
           * Output:
           * Prints the nodes for a particular fibonacci tree identified by x
           * Return Value: None       
           */
          void print_recur(size_type x, std::ostream& os)
          {
             if (x != size_type (-1)) {
                os << elements_[data_[x].item_index];
                if (data_[x].degree > 0) {
                   os << "(";
                   size_type i = data_[x].child;
                   do {
                      print_recur (i, os);
                      os << " ";
                      i = data_[i].right_sibling;
                   } while (i != data_[x].child);
                   os << ")";
                }
             }
          }
          
          /*
           * merge: Function to merge two fibonacci heaps
           * Input: 
           * size_type x : Index of the root element of the heap to be merged
           * Output:
           * Merges the heap pointed to by x in the main heap pointed to by head_
           * Return Value:
           * None       
           */
          void merge (size_type index);
          
          /*
           * consolidate: Function to consolidate
           * Input: 
           * size_type x : Index of the root element of the heap to be merged
           * Output:
           * Merges the heap pointed to by x in the main heap pointed to by head_
           * Return Value:
           * None       
           */
          void consolidate ();


          /*
           * mutable_fibonacci_heap_union: Unites the heap pointed to by head_ with 
           * the heap specified
           * Input: 
           * size_type index: Index of the root element of the heap to be united
           * Output:
           * United fibonacci heap
           * Return Value:
           * None       
           */
          void mutable_fibonacci_heap_union (size_type index);
           
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
           * mutable_fibonacci_heap_impl: Default constructor
           * Input: 
           * None
           * Output:
           * Return Value:
           * None       
           */
          mutable_fibonacci_heap_impl () {};
          
          /*
           * mutable_fibonacci_heap_impl: 3 argument constructor
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
          mutable_fibonacci_heap_impl (size_type n,
                const Compare &cmp, const Item_Map& id) :
                compare_{cmp}, id_{id}, top_{-1}, head_{-1} 
          { 
          }
          
          /*
           * mutable_fibonacci_heap_impl: range based constructor
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
             mutable_fibonacci_heap_impl (ForwardIterator first, ForwardIterator last,
                   const Compare &cmp, const Item_Map& id) :
                   compare_{cmp}, 
                   id_{id}, top_{-1}, head_{-1}
          {
             while(first != last) {
                push(*first);
                ++first;
             }
          }
          
          /*
           * print: Function for displaying the fibonacci heap
           * Input:
           * ostresm &os : Reference to the ouput stream
           * Output:
           * Outputs the fibonacci heap to the specified output stream
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
           * fibonacci heap with a new top element
           * Return Value:
           * None
           */
          void pop();
    };
     
    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::consolidate()
    {
       // Max Degree of fibonacci heap
       size_type D = floor(rec_log_phi()*elements_.size())+1;

       // Create an auxillary array of size D
       std::vector<size_type> aux(D, -1);

       size_type temp = head_;
       size_type x, d, y;

       do{
          x = temp;
          d = data_[d].degree;

          while(aux[d] != -1) {
             y = aux[d];
             if(!compare_(elements_[data_[x].item_index], elements_[data_[y].item_index])) {
                // Swap x, y
                std::swap(data_[x].item_index, data_[y].item_index);
             }

             fibonacci_link(data_[data_[y].left_sibling], 
                   data_[data_[y].right_sibling], data_[y], data_[x], y, x);
             aux[d] = -1;
             ++d;
          }

          aux[d] = x;

          temp = data_[temp].right_sibling;
       } while(temp != head_);

       top_ = -1;

       /* FIXME: Remaining to be completed tomorrow ;) */

    }

    template <class T, 
              class Compare,
              class Item_Map>
    typename mutable_fibonacci_heap_impl<T, Compare, Item_Map>::size_type 
    mutable_fibonacci_heap_impl<T, Compare, Item_Map>::get_new_top()
    {
       return 0;
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::push(const value_type& d)
    {
       fibonacci_heap_node obj;
       size_type index;

       elements_.push_back(d);

       obj.item_index = elements_.size() - 1;

       data_.push_back(obj);

       index = data_.size() - 1;

       data_[index].left_sibling = data_[index].right_sibling = index;

       id_(d) = index;

       if (head_ == size_type(-1)) {
          /* New heap */
          head_ = index;
          top_ = head_;
       } else {
          /* Concatenate w.r.t. right neighbor */
          data_[data_[top_].right_sibling].left_sibling = index;
          data_[index].right_sibling = data_[top_].right_sibling;

          data_[top_].right_sibling = index;
          data_[index].left_sibling = top_;
          
          if (compare_(elements_[data_[index].item_index], elements_[data_[top_].item_index]))
             top_ = index;
       }
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::update(const value_type& d)
    {
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::merge (size_type index)
    {
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::mutable_fibonacci_heap_union (size_type index)
    {
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::pop()
    {
    }
    
    template <class T, 
              class Compare,
              class Item_Map>
       template<typename Char, typename Traits>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::print(std::basic_ostream<Char, Traits>& os)
    {
       if (head_ != size_type(-1)) {
          size_type i = head_;
          do {
             print_recur(i, os);
             os << std::endl;
             i = data_[i].right_sibling;
          } while (i != head_);
       }
    }
      
    /*
     * fibonacci_link: Links the specified nodes in the heap
     * Input: 
     * size_type y: element in the heap
     * size_type z: element in the heap
     * Output:
     * links element y and z in the heap
     * Return Value:
     * None
    */
    void fibonacci_link (fibonacci_heap_node& y_left, fibonacci_heap_node& y_right, 
          fibonacci_heap_node& y, fibonacci_heap_node& x, size_t y_index, size_t x_index)
    {
       // Remove node1 from the root list
       y_left.right_sibling = y.right_sibling;
       y_right.left_sibling = y.left_sibling;

       // Make y a child of x
       y.parent = x_index;
       x.child = y_index;
       ++x.degree;

       // Unmark y
       y.mark = false;
    }

   
    
    /*
     * Fibonacci Heap implementation used when an external map is provided
     * by the user
     */
    template <class T, 
              class Compare,
              class Item_Map = default_t> 
    class mutable_fibonacci_heap
    {
        public:
            typedef T value_type;
            typedef size_t size_type;
        
        protected:
            typedef mutable_fibonacci_heap_impl<value_type, Compare, Item_Map> search_impl;
            
        public:
            mutable_fibonacci_heap(): impl() 
            {}
            
            mutable_fibonacci_heap (size_type n,
                const Compare& cmp, const Item_Map& id) : impl(n, cmp, id)
            {}
            
            template<typename ForwardIterator>
            mutable_fibonacci_heap (ForwardIterator first, ForwardIterator last,
                           const Compare& cmp, const Item_Map& id):
                           impl (first, last, cmp, id)
            {}
 
            void update(const value_type& d) 
            { 
                impl.update(d); 
            }
          
            void push(const value_type& d)
            { 
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
                impl.pop();
            }
            
            template<typename Char, typename Traits>
            void print(std::basic_ostream<Char, Traits>& os)
            {
               impl.print(os);
            }

        protected:
            search_impl impl;
    };

    /*
     * Fibonacci Heap Implementation in case of no external map
     */
    template <class T, 
              class Compare>
    class mutable_fibonacci_heap<T, Compare, default_t>
    {
 
        public:
            typedef T value_type;
            typedef size_t size_type;
        
        protected:
            typedef typename std::unordered_map<value_type, size_type> internal_map;
            struct item_map {
                internal_map *imap;

                public:
                item_map (internal_map *map): imap(map) 
                {}
                
                template<typename value_type>
                size_type & operator() (const value_type& key)
                {
                    return (*imap)[key];
                }
            };


            typedef mutable_fibonacci_heap_impl<value_type, Compare, item_map> search_impl;

        public:
            mutable_fibonacci_heap() 
            {};
            
            mutable_fibonacci_heap (size_type n,
                const Compare& cmp): map_{n}, id_{&map_}, impl (n, cmp, id_)
              
            { 
            }
            
            template<typename ForwardIterator>
            mutable_fibonacci_heap (ForwardIterator first, ForwardIterator last,
                           const Compare& cmp): id_(map_),
                           impl (first, last, cmp, id_)
            {
            }
 
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
                const value_type top_element = impl.top();
                impl.pop();
                map_.erase(top_element);
            }
            
            template<typename Char, typename Traits>
            void print(std::basic_ostream<Char, Traits>& os) 
            {
                impl.print(os);
            }
                
        protected:
            internal_map map_;
            item_map id_;
            search_impl impl;

    };
}
#endif // ORIGIN_HEAPS_FIBONACCI_HEAP_HPP
