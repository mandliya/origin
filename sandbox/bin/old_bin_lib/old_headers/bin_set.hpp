// Bin set
// bin_set.hpp

template<typename Bin,
         typename T>
class bin_set
{
    public:
        bin_set();
        bin_set(const bin_table&);
        bin_set(const bin_set&);
        ~bin_set();

        bool empty();
        iterator begin();
        iterator end();
        void insert(T& const);        

    private:
    Bin bin_function;
    bin_table<T, bin_function> container;  

};

// Default Constructor
template<typename Bin, typename T>
bin_set<Bin, T>::bin_set()
    :bin_function(), container()
{  }

template<typename Bin, typename T>
bin_set<Bin, T>::bin_set(const bin_table& rhs)
    :bin_function(), container(rhs)
{  }

template<typename Bin, typename T>
bin_set<Bin, T>::bin_set(const bin_set& rhs)
    :bin_function(), container(rhs.container)
{  }

// Default Destructor
template<typename Bin, typename T>
bin_set<Bin,T>::~bin_set()
{ }

template<typename Bin, typename T>
bool bin_set<Bin, T>::empty()
{return container.empty(); }

template<typename Bin, typename T>
iterator bin_set<Bin, T>::begin()
{return container.begin(); }

template <typename Bin, typename T>
iterator bin_set<Bin, T>::end()
{ return container.end();  }

template <typename Bin, typename T>
void bin_set<Bin, T>::insert(const T& rhs)
{ container.insert(rhs); }
