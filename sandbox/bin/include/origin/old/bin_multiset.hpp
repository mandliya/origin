// Chris Tuttle
// bin_multiset.hpp

template<typename Bin,
         typename T>
class bin_multiset
{
    public:
    bin_multiset();
    bin_multiset(const bin_table&);
    bin_multiset(const bin_multiset&);
    ~bin_multiset();

    bool empty();
    iterator begin();
    iterator end();    
    void insert(T& const);

    private:
    

};

template<typename Bin, typename T>
bin_multiset<Bin,T>::bin_multiset()
    : bin_function(), container()
{   }

template<typename Bin, typename T>
bin_multiset<Bin,T>::bin_multiset(bin_table& const rhs)
    : bin_function(), container(rhs)
{   }

template<typename Bin, typename T>
bin_multiset<Bin,T>::bin_multiset(bin_multiset& const rhs)
    : bin_function(), container(rhs.container)
{   }

template<typename Bin, typename T>
bin_multiset<Bin,T>::~bin_multiset()
{  }

template<typename Bin, typename T>
bool bin_multiset<Bin,T>::empty()
{ return container.empty();  }

template<typename Bin, typename T>
iterator bin_multiset<Bin,T>::begin()
{  return container.begin(); }

template<typename Bin, typename T>
iterator bin_multiset<Bin,T>::end()
{  return container.end();  }

template<typename Bin, typename T>
void bin_multiset<Bin, T>::insert(T& const rhs)
{ container.insert(rhs);  }


