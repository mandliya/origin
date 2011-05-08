// Bin map
// bin_map.hpp

// T should be an std::pair<key, value> form
template<typename Bin, template T>
class bin_map
{
    public:
        bin_map();
        bin_map(const bin_table&);
        bin_map(const bin_map&);
        ~bin_map();

// Remember iterator isnt defined so fix that
        bool empty();
        iterator begin();
        iterator end();
        void insert(T& const);

      private:
      Bin bin_function;
      bin_table<T, bin_function> container;
};

/** Default Constructor. */  
template<typename Bin, typename T>
bin_map<Bin, T>::bin_map()
    :bin_function(), container()
{  }

/** Copy constructor. */
template<typename Bin, typename T>
bin_map<Bin, T>::bin_map(const bin_table& rhs)
    :bin_function(), container(rhs)
{  }

/** Copy Constructor. */
template<typename Bin, typename T>
bin_map<Bin, T>::bin_map(const bin_map& rhs)
    :bin_function(), container(rhs.container)
{  }

/** Default Destructor. */
template<typename Bin, typename T>
bin_map<Bin, T>::~bin_map()
{  }

/** Overloads the .empty() function. */
template<typename Bin, typename T>
bool bin_map<Bin, T>::empty()
{  return container.empty();  }

/** Overloads the begin and end functions. */
template<typename Bin, typename T>
iterator bin_map<Bin, T>::begin()
{return container.begin(); }

template<typename Bin, typename T>
iterator bin_map<Bin, T>::end()
{return container.end(); }

template<typename Bin, typename T>
void bin_map<Bin, T>::insert(const T& rhs)
{ container.insert(rhs); }

      
