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
  // FIXME: These aren't going to be constexptr since sqrt isn't constexpr.
  // Constant Expressions
  constexpr double phi() {return (1+sqrt(5))/2;}
    
  // Reciprocal of naturallog(phi)
  constexpr double rec_log_phi() {return 1/log(phi());} 

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
          
          /* Random access container which holds the heap elements */
          std::vector<T> elements_;
          std::vector<fibonacci_heap_node> data_;
           
          /* Internal map for mapping the values stored in the external map
           * to actual index of the element in the heap
           */
          
          Compare compare_;
          Item_Map id_;
          
          /*
           * top_ - index of the top element
           */ 
          
          size_type top_;
         
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
           * cut: Function to 
           * Input: 
           * size_type x : Index of the root element of the heap to be merged
           * Output:
           * Merges the heap pointed to by x in the main heap pointed to by head_
           * Return Value:
           * None       
           */
          void cut (size_type index, size_type y);

          /*
           * cascading cut: Function to 
           * Input: 
           * size_type x : Index of the root element of the heap to be merged
           * Output:
           * Merges the heap pointed to by x in the main heap pointed to by head_
           * Return Value:
           * None       
           */
          void cascading_cut (size_type y);

          /*
           * fibonacci_link: Function to swap two nodes of a tree
           * Input: 
           * None
           * Output:
           * Index of the new root
           * Return Value:
           * size_type: Index of the new root
           */
          void fibonacci_link(size_type y, size_type x);
          
       public:
          /*
           * mutable_fibonacci_heap_impl: Default constructor
           * Input: 
           * None
           * Output:
           * Return Value:
           * None       
           */
          mutable_fibonacci_heap_impl () = default;
          
          /*
           * mutable_fibonacci_heap_impl: 3 argument constructor
           * Input: 
           * Compare &cmp: comparison function predicate
           * Item_Map: Lambda function for map
           * Output:
           * Instantiates a heap with given comparison function
           * and property map
           * Return Value:
           * None
           */
          mutable_fibonacci_heap_impl (const Compare &cmp, const Item_Map& id) :
                                       compare_{cmp}, id_{id}, top_{-1}
          {}
          
          /*
           * mutable_fibonacci_heap_impl: range based constructor
           * Input: 
           * ForwardIterator first: Iterator to the first element of a container 
           * ForwardIterator last: Iterator to the last element of a container
           * Compare &cmp: comparison function predicate
           * Item_Map: Lambda function for map
           * Output:
           * Instantiates a heap of elements with given comparison function
           * and property map
           * Return Value:
           * None       
           */
          template<typename ForwardIterator>
          mutable_fibonacci_heap_impl (ForwardIterator first, ForwardIterator last,
                                       const Compare &cmp, const Item_Map& id) :
                                       compare_{cmp}, id_{id}, top_{-1}
          {
             reserve(std::distance(first, last));
             while(first != last) {
                push(*first);
                ++first;
             }
          }
          
          mutable_fibonacci_heap_impl (std::initializer_list<T> lst, 
                                       const Compare &cmp, const Item_Map& id) :
                                       compare_{cmp}, id_{id}, top_{-1}
          {
             reserve(lst.size());
             for (auto &x : lst) {
                 push (x);
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
          size_type size() const
          {
             return elements_.size();
          }

          void reserve(size_type n)
          {
             elements_.reserve(n);
             data_.reserve(n);
          }
          
          size_type capacity() const
          {
             return elements_.capacity();
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
       size_type D = floor(rec_log_phi()*log(elements_.size() - 1))+1;

       // Create an auxillary array of size D
       std::vector<size_type> aux(D+1, size_type(-1));

       size_type next = top_;
       size_type pseudo_top = data_[top_].left_sibling;
       size_type temp, d, i;
       bool flag = false;

       do{
          temp = next;
          d = data_[temp].degree;
          next = data_[temp].right_sibling;
          if (temp == pseudo_top)
             flag = true;

          while((aux[d] != size_type(-1)) && (d<=D)) {
             if (!compare_(elements_[data_[temp].item_index], elements_[data_[aux[d]].item_index])) {
                fibonacci_link(temp, aux[d]);
                temp = aux[d];
             } else {
                fibonacci_link(aux[d], temp);
             }
             aux[d] = size_type(-1);
             ++d;
          }

          aux[d] = temp;
       } while(!flag);
       
       size_type temp_top = temp;
       top_ = temp_top;
       data_[temp_top].right_sibling = temp_top;
       data_[temp_top].left_sibling = temp_top;

       for(i = 0; i<=D; ++i) {
          if(aux[i] != size_type(-1) && aux[i]!=temp_top) {
             // Add aux[i] to the root list
             // Concatenate w.r.t. right neighbor of top
             data_[data_[top_].right_sibling].left_sibling = aux[i]; 
             data_[aux[i]].right_sibling = data_[top_].right_sibling;
             data_[top_].right_sibling = aux[i];
             data_[aux[i]].left_sibling = top_;
             
             if(!compare_(elements_[data_[top_].item_index], elements_[data_[aux[i]].item_index])) {
                top_ = aux[i];
             }
        }
       }
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

       if (top_ == size_type(-1)) {
          // New heap
          top_ = index;
       } else {
          // Concatenate w.r.t. right neighbor of top
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
       size_type index = id_(d);
       elements_[data_[index].item_index] = d;
       size_type y = data_[index].parent;

       if ((y != (size_type)-1) && !compare_(elements_[data_[y].item_index], d)) {
          cut(index, y);
          cascading_cut(y);
       }

       if(compare_(d, elements_[data_[top_].item_index]))
          top_ = index;
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::cut (size_type x, size_type y)
    {
       data_[y].child = data_[x].right_sibling;
       data_[data_[x].left_sibling].right_sibling = data_[x].right_sibling;
       data_[data_[x].right_sibling].left_sibling = data_[x].left_sibling;
       
       // Decrement degre of y
       data_[y].degree -= 1;
       
       // Remove y as x's parent
       data_[x].parent = -1;

       // Add x to the root list
       data_[x].left_sibling = top_;
       data_[x].right_sibling = data_[top_].right_sibling;
       data_[data_[top_].right_sibling].left_sibling = x;
       data_[top_].right_sibling = x;

       // Mark x as false
       data_[x].mark = false;
    }

    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::cascading_cut (size_type y)
    {
       size_type z = data_[y].parent;
       while( z != size_type(-1)) {
          if (data_[y].mark == false) {
             data_[y].mark = true;
             return;
          } else {
             cut(y, z);
             y = z;
             z = data_[y].parent;
          }
       }
    }
    
    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::fibonacci_link (size_type y, size_type x)
    {
       // Remove y from root list
       data_[data_[y].left_sibling].right_sibling = data_[y].right_sibling; 
       data_[data_[y].right_sibling].left_sibling = data_[y].left_sibling; 

       // Make y a child of x, increment degree of x
       if(data_[x].degree>0) {
          data_[y].right_sibling = data_[x].child;
          data_[y].left_sibling = data_[data_[x].child].left_sibling;
          data_[data_[data_[x].child].left_sibling].right_sibling = y;
          data_[data_[x].child].left_sibling = y;
       } else {
          data_[y].right_sibling = data_[y].left_sibling = y;
       }
       
       data_[y].parent = x;
       data_[x].child = y;
       data_[x].degree += 1;

       // mark y= false
       data_[y].mark = false;
    }
    
    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::pop()
    {
       size_type z = top_;
       size_type x = data_[z].child;
       size_type temp;

       if (data_[z].degree > 0) {
          do{
             // Add x to root list
             // Concatenate w.r.t. right neighbor of top
             temp = data_[x].right_sibling;
             
             // Make x's parent as NULL
             data_[x].parent = -1;
             x = temp;//data_[x].right_sibling;
          } while(x != data_[z].child);
         
          // Attach the childlist list to root list
          data_[data_[top_].right_sibling].left_sibling = data_[x].left_sibling;
          data_[data_[x].left_sibling].right_sibling = data_[top_].right_sibling;
          data_[x].left_sibling = top_;
          data_[top_].right_sibling = x;
       }
       
       // Remove z from the root list
       data_[data_[z].left_sibling].right_sibling = data_[z].right_sibling;
       data_[data_[z].right_sibling].left_sibling = data_[z].left_sibling;
       
       if(z == data_[z].right_sibling) {
          top_ = -1;
       } else {
          top_ = data_[z].right_sibling;
          consolidate();
       }


       // Where in data_ old last element is stored
       size_type index = id_(elements_[elements_.size()-1]);
    
       // Copy the last element to location of old top element
       elements_[data_[z].item_index] = elements_[elements_.size()-1];
    
       // Point the item_index of the old element to correct location
       data_[index].item_index = data_[z].item_index;
    
       // Invalidating the entries of node
       data_[z].parent = -1;
       data_[z].child = -1;
       data_[z].right_sibling = -1;
       data_[z].left_sibling = -1;
       data_[z].degree = 0;
       data_[z].item_index = -1;
    
       elements_.pop_back();
    }
    
    template <class T, 
              class Compare,
              class Item_Map>
       template<typename Char, typename Traits>
    void mutable_fibonacci_heap_impl<T, Compare, Item_Map>::print(std::basic_ostream<Char, Traits>& os)
    {
       if (top_ != size_type(-1)) {
          size_type i = top_;
          do {
             print_recur(i, os);
             os << std::endl;
             i = data_[i].right_sibling;
          } while (i != top_);
       }
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
            mutable_fibonacci_heap() : impl() 
            {}
            
            mutable_fibonacci_heap (const Compare& cmp, const Item_Map& id) : impl(cmp, id)
            {}
            
            template<typename ForwardIterator>
            mutable_fibonacci_heap (ForwardIterator first, ForwardIterator last,
                                    const Compare& cmp, const Item_Map& id) :
                                    impl (first, last, cmp, id)
            {}
 
            mutable_fibonacci_heap (std::initializer_list<T> lst, 
                                    const Compare &cmp, const Item_Map& id) :
                                    impl (lst, cmp, id)
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
          
            size_type size() const
            {
               return impl.size();
            }
          
            void reserve(size_type n)
            {
               impl.reserve(n);
            }
            
            size_type capacity() const
            {
               return impl.capacity();
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
            mutable_fibonacci_heap() : impl() 
            {}
            
            mutable_fibonacci_heap (const Compare& cmp): id_{&map_}, impl (cmp, id_)
            {}
            
            template<typename ForwardIterator>
            mutable_fibonacci_heap (ForwardIterator first, ForwardIterator last,
                                    const Compare& cmp): id_(map_),
                                    impl (first, last, cmp, id_)
            {}
 
            mutable_fibonacci_heap (std::initializer_list<T> lst,
                                    const Compare &cmp):id_{&map_},
                                    impl (lst, cmp, id_)
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
          
            size_type size() const
            {
               return impl.size();
            }
          
            void reserve(size_type n)
            {
               impl.reserve(n);
            }
            
            size_type capacity() const
            {
               return impl.capacity();
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
 
  // FIXME: Document me!
  /**
   * The fibonacci heap...
   *
   * @tparam T      The value type stored in the heap.
   * @tparam Comp   A strict order on T
   */
  template<typename T, typename Comp = std::less<T>>
  class fibonacci_heap
  {
  private:
    typedef T value_type;
    typedef size_t size_type;
    typedef Comp value_compare;

  public:
    /** @name Initialization */
    //@{
    /**
     * @brief Default constructor
     */
    fibonacci_heap(value_compare const& comp = value_compare{}) 
      : compare_{comp}, top_{-1}
    { }

    /**
     * @name Range constructor
     */
    template<typename ForwardIterator>
      fibonacci_heap(ForwardIterator first, 
                     ForwardIterator last,
                     value_compare const &comp) 
        : compare_{comp}, top_{-1}
      {
        reserve(std::distance(first, last));
        while(first != last) {
          push(*first);
          ++first;
        }
      }

    /**
     * @name Initializer list constructor
     */
    fibonacci_heap(std::initializer_list<T> lst,
                   value_compare const& comp) 
      : compare_{comp}, top_{-1}
    {
      reserve(lst.size());
      for (auto &x : lst) {
          push(x);
      }
    }
    //@}
  
    template<typename Char, typename Traits>
    void print(std::basic_ostream<Char, Traits>& os);
    
    /**
     * @name Properties
     */
    //@{
    bool empty() const
    {
      return elements_.empty();
    }
    
    size_type size() const
    {
      return elements_.size();
    }
    
    value_compare value_comp() const
    {
      return compare_;
    }
    //@}

    /** @name Capacity */
    //@{
    size_type capacity() const
    {
        return elements_.capacity();
    }
    
    void reserve(size_type n)
    {
      reversemap_.reserve(n);
      elements_.reserve(n);
      data_.reserve(n);
    }
    //@}
    

    /** @name Priority Queue Operations  */
    //@{
    value_type const& top() const
    {
      return elements_[data_[top_].item_index];
    }

    void push(const value_type& d); 
    
    void pop();
    //@}
    
    
  private:
    std::vector<T> elements_;
    std::vector<fibonacci_heap_node> data_;
    std::vector<size_type> reversemap_;
    Comp compare_;
    size_type top_;

    // Recursively print the tree
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

    // FIXME: What do these do?
    void consolidate ();
    void cut(size_type index, size_type y);
    void cascading_cut(size_type y);

    // fibonacci_link: Function to swap two nodes of a tree
    void fibonacci_link(size_type y, size_type x);
  };
    

  template<typename T, typename Comp>
    void fibonacci_heap<T, Comp>::consolidate()
    {
      // Max Degree of fibonacci heap
      size_type D = floor(rec_log_phi()*log(elements_.size() - 1))+1;

      // Create an auxillary array of size D
      std::vector<size_type> aux(D+1, size_type(-1));

      size_type next = top_;
      size_type pseudo_top = data_[top_].left_sibling;
      size_type temp, d, i;
      bool flag = false;

      do{
        temp = next;
        d = data_[temp].degree;
        next = data_[temp].right_sibling;
        if (temp == pseudo_top)
            flag = true;

        while((aux[d] != size_type(-1)) && (d<=D)) {
            if (!compare_(elements_[data_[temp].item_index], elements_[data_[aux[d]].item_index])) {
              fibonacci_link(temp, aux[d]);
              temp = aux[d];
            } else {
              fibonacci_link(aux[d], temp);
            }
            aux[d] = size_type(-1);
            ++d;
        }

        aux[d] = temp;
      } while(!flag);
      
      size_type temp_top = temp;
      top_ = temp_top;
      data_[temp_top].right_sibling = temp_top;
      data_[temp_top].left_sibling = temp_top;

      for(i = 0; i<=D; ++i) {
        if(aux[i] != size_type(-1) && aux[i]!=temp_top) {
          // Add aux[i] to the root list
          // Concatenate w.r.t. right neighbor of top
          data_[data_[top_].right_sibling].left_sibling = aux[i]; 
          data_[aux[i]].right_sibling = data_[top_].right_sibling;
          data_[top_].right_sibling = aux[i];
          data_[aux[i]].left_sibling = top_;
          
          if(!compare_(elements_[data_[top_].item_index], elements_[data_[aux[i]].item_index])) {
            top_ = aux[i];
          }
        }
      }
    }

  template<typename T, typename Comp>
    void fibonacci_heap<T, Comp>::push(const value_type& d)
    {
      fibonacci_heap_node obj;
      size_type index;

      elements_.push_back(d);

      obj.item_index = elements_.size() - 1;

      data_.push_back(obj);

      index = data_.size() - 1;

      reversemap_.push_back(index);

      data_[index].left_sibling = data_[index].right_sibling = index;

      if (top_ == size_type(-1)) {
        // New heap
        top_ = index;
      } else {
        // Concatenate w.r.t. right neighbor of top
        data_[data_[top_].right_sibling].left_sibling = index;
        data_[index].right_sibling = data_[top_].right_sibling;

        data_[top_].right_sibling = index;
        data_[index].left_sibling = top_;
        
        if(compare_(elements_[data_[index].item_index], elements_[data_[top_].item_index]))
          top_ = index;
      }
    }
  
  
  template<typename T, typename Comp>
    void fibonacci_heap<T, Comp>::cut(size_type x, size_type y)
    {
      if(data_[y].degree == 1) {
        // Set y's child as -1
        data_[y].child = -1;
      } else {
        if (data_[y].child == x) { 
            data_[y].child = data_[x].right_sibling;
        }
        data_[data_[x].left_sibling].right_sibling = data_[x].right_sibling;
        data_[data_[x].right_sibling].left_sibling = data_[x].left_sibling;
      }
      
      // Decrement degre of y
      data_[y].degree -= 1;
      
      // Remove y as x's parent
      data_[x].parent = -1;

      // Add x to the root list
      data_[x].left_sibling = top_;
      data_[x].right_sibling = data_[top_].right_sibling;
      data_[data_[top_].right_sibling].left_sibling = x;
      data_[top_].right_sibling = x;

      // Mark x as false
      data_[x].mark = false;
    }

  template<typename T, typename Comp>
    void fibonacci_heap<T, Comp>::cascading_cut(size_type y)
    {
      size_type z = data_[y].parent;
      while( z != size_type(-1)) {
        if (data_[y].mark == false) {
            data_[y].mark = true;
            return;
        } else {
            cut(y, z);
            y = z;
            z = data_[y].parent;
        }
      }
    }
  
  template<typename T, typename Comp>
  void fibonacci_heap<T, Comp>::fibonacci_link (size_type y, size_type x)
    {
      // Remove y from root list
      data_[data_[y].left_sibling].right_sibling = data_[y].right_sibling; 
      data_[data_[y].right_sibling].left_sibling = data_[y].left_sibling; 

      // Make y a child of x, increment degree of x
      if(data_[x].degree>0) {
        data_[y].right_sibling = data_[x].child;
        data_[y].left_sibling = data_[data_[x].child].left_sibling;
        data_[data_[data_[x].child].left_sibling].right_sibling = y;
        data_[data_[x].child].left_sibling = y;
      } else {
        data_[y].right_sibling = data_[y].left_sibling = y;
      }
      
      data_[y].parent = x;
      data_[x].child = y;
      data_[x].degree += 1;

      // mark y= false
      data_[y].mark = false;
    }
  
  template<typename T, typename Comp>
    void fibonacci_heap<T, Comp>::pop()
    {
      size_type z = top_;
      size_type x = data_[z].child;
      size_type temp;

      if (top_ == size_type(-1)) {
        return;
      }
      
      if (data_[z].degree > 0) {
        do {
          // Add x to root list
          // Concatenate w.r.t. right neighbor of top
          data_[data_[top_].right_sibling].left_sibling = x;
          temp = data_[x].right_sibling;
          data_[x].right_sibling = data_[top_].right_sibling;
          data_[top_].right_sibling = x;
          data_[x].left_sibling = top_;
          
          // Make x's parent as NULL
          data_[x].parent = -1;
          x = temp;
        } while(x != data_[z].child);
      }
      
      // Remove z from the root list
      data_[data_[z].left_sibling].right_sibling = data_[z].right_sibling;
      data_[data_[z].right_sibling].left_sibling = data_[z].left_sibling;
      
      if(z == data_[z].right_sibling) {
        top_ = -1;
      } else {
        top_ = data_[z].right_sibling;
        consolidate();
      }

      // Where in data_ old last element is stored
      size_type index = reversemap_.back();

      // Copy the last element to location of old top element
      elements_[data_[z].item_index] = elements_[elements_.size()-1];

      reversemap_[data_[z].item_index] = index;

      // Point the item_index of the old element to correct location
      data_[index].item_index = data_[z].item_index;

      // Invalidating the entries of node
      data_[z].parent = -1;
      data_[z].child = -1;
      data_[z].right_sibling = -1;
      data_[z].left_sibling = -1;
      data_[z].degree = 0;
      data_[z].item_index = -1;

      reversemap_.pop_back();
      elements_.pop_back();
    }
  
  template<typename T, typename Comp>
    template<typename Char, typename Traits>
      void fibonacci_heap<T, Comp>::print(std::basic_ostream<Char, Traits>& os)
      {
        if (top_ != size_type(-1)) {
          size_type i = top_;
          do {
              print_recur(i, os);
              os << std::endl;
              i = data_[i].right_sibling;
          } while (i != top_);
        }
      }

} // namespace origin

#endif
