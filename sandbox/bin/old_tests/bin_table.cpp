
#include <iostream>

#include <origin/bin_table.hpp>

using namespace origin;
using namespace std;

// Map character grades onto earned points.
struct gpa
{
    size_t operator()(char c) const {
        char d = tolower(c);
        switch(d) {
            case 'a': return 4;
            case 'b': return 3;
            case 'c': return 2;
            case 'd': return 1;
            case 'f': return 0;
            default: throw std::domain_error("Unknown grade");
        }
    }
};



int main()
{
    bin_table<char, gpa> x(5);
    x.insert('a');
    x.insert('b');
    x.insert('a');

    cout << x.bucket_size(4) << "\n";
}
