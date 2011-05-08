// Chris Tuttle & Brian Bartman
// test_histogram.cpp

#include "origin/histogram.hpp"

#include "origin/free_functions.hpp"

#include <boost/assert.hpp>

#include<iostream>

#include<string>
#include<vector>
#include<array>
#include<unordered_map>

using namespace std;

// Declaration of the binning function
struct bin
{

        int operator()(string s)
        {
            if(s=="A")
                return 0;
            
            if(s=="B")
                return 1;
            
            if(s=="C")
                return 2;
            
            if(s=="D")
                return 3;
              
             if(s=="F")
                return 4;

        }

};

void test_constructors()
{
    // Tests the default construction
    histogram<string, bin, vector<int>> hist;
    BOOST_ASSERT(hist.empty() == true);

    // Tests the copy constructor
    hist.insert("A");
    histogram<string, bin, vector<int>> hist2(hist);
    BOOST_ASSERT(hist.tally(0) == hist2.tally(0));

    // Tests the move constructor
    histogram<string, bin, vector<int>> hist3(move(hist2));
    BOOST_ASSERT(hist.tally(0) == hist3.tally(0));
    BOOST_ASSERT(hist2.empty() == true);

}

void test_assignment()
{
    // Tests the equality assignment
    vector<string> data = {"A", "B", "C", "D"};
    histogram<string, bin, vector<int>> hist;
    fill_bins(data.begin(), data.end(), hist);
    BOOST_ASSERT(hist.empty() == false);
    BOOST_ASSERT(hist.size() == 4);
    histogram<string, bin, vector<int>> hist2;
    hist2 = hist;
    BOOST_ASSERT(hist.size() == 4);

    // Tests the move assignment
    histogram<string, bin, vector<int>> hist3;
    hist3 = move(hist2);
    BOOST_ASSERT(hist3.size() == 4);
    BOOST_ASSERT(hist2.empty() == true);

}

void test_member_funcs()
{
    // Tests the empty function
    vector<string> data = {"A","B","C","D"};
    histogram<string, bin, vector<int>> hist;
    BOOST_ASSERT(hist.empty() == true);
    hist.insert("A");
    BOOST_ASSERT(hist.empty() == false);

    // Tests the size function, wth the A from before should be 5
    fill_bins(data.begin(), data.end(), hist);
    BOOST_ASSERT(hist.size() == 5);

    // Tests the bin function
    BOOST_ASSERT(hist.bin("A") == 0);
 
    // Tests the tally function, there are 2 A's in the first index
    BOOST_ASSERT(hist.tally(0) == 2);

    // Tests the count function, there are 2 A's in the A bin
    BOOST_ASSERT(hist.count("A") == 2);

}

void test_insertion()
{
    vector<string> data = {"A","B","C","D"};
    histogram<string, bin, vector<int>> hist;
    
    // Test the const insertion
    hist.insert("A");
    BOOST_ASSERT(hist.tally(0) == 1);
    BOOST_ASSERT(hist.count("A") == 1);

    // Test the move reference
    hist.insert(data[2]);
    BOOST_ASSERT(hist.tally(2) == 1);
    BOOST_ASSERT(hist.count("C") == 1);

}

void test_vector()
{
    // Tests the vector for construction
    vector<string> data = {"A","B","C","D","F"};
    histogram<string, bin, vector<int>> hist;
    BOOST_ASSERT(hist.empty() == true);

    // Tests insertion in the vector
    hist.insert("A");
    BOOST_ASSERT(hist.empty() == false);

    // Tests fill bins with the vector, remember this has 2 A's now
    fill_bins(data.begin(), data.end(), hist);

    // Tests to see if 2 A's are added, 1 B, 1 C, and 1 D, and 1 F
    BOOST_ASSERT(hist.tally(0) == 2);
    BOOST_ASSERT(hist.tally(1) == 1);
    BOOST_ASSERT(hist.tally(2) == 1);
    BOOST_ASSERT(hist.tally(3) == 1);
    BOOST_ASSERT(hist.tally(4) == 1);


   // Tests a more complicated version of the letters
   histogram<string, bin, vector<int>> hist2;
   BOOST_ASSERT(hist2.empty() == true);

   // Fills the hist out of order with 3 a's, 2 b's, 1 c, 2 d's and 0 f's
   vector<string> data2 = {"B", "A", "B", "D", "C", "A", "A", "D"};
   fill_bins(data2.begin(), data2.end(), hist2);

   // Tests to see if this was filled correctly
   BOOST_ASSERT(hist2.tally(0) == 3);
   BOOST_ASSERT(hist2.tally(1) == 2);
   BOOST_ASSERT(hist2.tally(2) == 1);
   BOOST_ASSERT(hist2.tally(3) == 2);

}


void test_array()
{
    // Tests the array for construction
    array<string,5> data = {"A","B","C","D","F"};
    histogram<string, bin, array<int, 5>> hist;

    // Tests insertion in the array
    hist.insert("A");
    BOOST_ASSERT(hist.empty() == false);

    // Tests fill bins with the vector, remember this has 2 A's now
    fill_bins(data.begin(), data.end(), hist);

    // Tests to see if 2 A's are added, 1 B, 1 C, and 1 D, and 1 F
    BOOST_ASSERT(hist.tally(0) == 2);
    BOOST_ASSERT(hist.tally(1) == 1);
    BOOST_ASSERT(hist.tally(2) == 1);
    BOOST_ASSERT(hist.tally(3) == 1);
    BOOST_ASSERT(hist.tally(4) == 1);


   // Tests a more complicated version of the letters
   histogram<string, bin, array<int, 8>> hist2;

   // Fills the hist out of order with 3 a's, 2 b's, 1 c, 2 d's and 0 f's
   array<string,8> data2 = {"B", "A", "B", "D", "C", "A", "A", "D"};
   fill_bins(data2.begin(), data2.end(), hist2);

   // Tests to see if this was filled correctly
   BOOST_ASSERT(hist2.tally(0) == 3);
   BOOST_ASSERT(hist2.tally(1) == 2);
   BOOST_ASSERT(hist2.tally(2) == 1);
   BOOST_ASSERT(hist2.tally(3) == 2);

}

void test_unordered_map()
{
 
    // Tests the array for construction
    array<string,5> data = {"A","B","C","D","F"};
    histogram<string, bin, unordered_map<unsigned int,int>> hist;

    // Tests insertion in the array
    hist.insert("A");
    BOOST_ASSERT(hist.empty() == false);

    // Tests fill bins with the vector, remember this has 2 A's now
    fill_bins(data.begin(), data.end(), hist);

    // Tests to see if 2 A's are added, 1 B, 1 C, and 1 D, and 1 F
    BOOST_ASSERT(hist.tally(0) == 2);
    BOOST_ASSERT(hist.tally(1) == 1);
    BOOST_ASSERT(hist.tally(2) == 1);
    BOOST_ASSERT(hist.tally(3) == 1);
    BOOST_ASSERT(hist.tally(4) == 1);

   // Tests a more complicated version of the letters
   histogram<string, bin, unordered_map<unsigned int,int>> hist2;

   // Fills the hist out of order with 3 a's, 2 b's, 1 c, 2 d's and 0 f's
   array<string,8> data2 = {"B", "A", "B", "D", "C", "A", "A", "D"};
   fill_bins(data2.begin(), data2.end(), hist2);

   // Tests to see if this was filled correctly
   BOOST_ASSERT(hist2.tally(0) == 3);
   BOOST_ASSERT(hist2.tally(1) == 2);
   BOOST_ASSERT(hist2.tally(2) == 1);
   BOOST_ASSERT(hist2.tally(3) == 2);

}

int main()
{

    // Tests the constructors
    test_constructors();

    // Tests the assignment
    test_assignment();

    // Tests member functions like size and empty
    test_member_funcs();

    // Tests insertions
    test_insertion();

    // Tests the histogram to work with the vector data structure
    test_vector();

    // Tests the histogram to work with the array data structure
    test_array();

    // Tests the histogram to work with the unordered map data structure
    test_unordered_map();


    return 0;
}
