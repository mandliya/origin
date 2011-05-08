/**
 *  begining of basic testing
 *
 * to be defined later
 *
 */

#include<origin/bins.hpp>
#include<boost/foreach.hpp>
#include<cassert>
#include<string>
#include<iostream>

#include<vector>
#include<array>
#include<list>
#include<set>
#include<deque>
#include<stack>
#include<queue>
#include<map>
#include<unordered_map>



using namespace std;

struct Bin
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
template<typename T>
void basic_verification(T);
int main()
{
    basic_verification(bins<string,Bin,vector<vector<string>> >());
    basic_verification(bins<string,Bin,vector<list<string>> >());
    basic_verification(bins<string,Bin,vector<queue<string>> >());
    basic_verification(bins<string,Bin,vector<deque<string>> >());
    basic_verification(bins<string,Bin,vector<stack<string>> >());
    basic_verification(bins<string,Bin,vector<set<string>> >());
        
    basic_verification(bins<string,Bin,array<vector<string>,5>>());
    basic_verification(bins<string,Bin,array<list<string>,5>>());
    basic_verification(bins<string,Bin,array<queue<string>,5>>());
    basic_verification(bins<string,Bin,array<deque<string>,5>>());
    basic_verification(bins<string,Bin,array<stack<string>,5>>());
    basic_verification(bins<string,Bin,array<set<string>,5>>());

    basic_verification(bins<string,Bin,unordered_map<unsigned int,vector<string> > >());
    basic_verification(bins<string,Bin,unordered_map<unsigned int,list<string> > >());
    basic_verification(bins<string,Bin,unordered_map<unsigned int,queue<string> > >());
    basic_verification(bins<string,Bin,unordered_map<unsigned int,deque<string> > >());
    basic_verification(bins<string,Bin,unordered_map<unsigned int,stack<string> > >());
    basic_verification(bins<string,Bin,unordered_map<unsigned int,set<string> > >());
    
    return 0;
}

template<typename T>
void basic_verification(T)
{
    {


        //begining test.
        T b1;
        
        b1.insert("A");
        assert(b1.empty()==false);
        assert(b1.size()!=0);
        

        b1.insert("B");
        assert(b1.empty()==false);
        assert(b1.size()!=0);
        
        assert(b1.bin("A") == 0);
        assert(b1.empty()==false);
        assert(b1.size()!= 0);
         
        assert(b1.tally(0) == 1);
        assert(b1.tally(1) == 1);
        assert(b1.empty()==false);
        assert(b1.size()!= 0);
        
        //calling count
        assert(b1.count("A") == 1);
        assert(b1.count("B") == 1);
        // making sure nothing changed.
        assert(b1.empty()==false);
        assert(b1.size()!= 0);
        
        //create copy
        T b2(b1);
        //make sure that copy shares same traits as previous
        assert(b2.count("A") == 1);
        assert(b2.count("B") == 1);
        assert(b2.tally(0) == 1);
        assert(b2.tally(1) == 1);
        assert(b2.empty()==false);
        assert(b1.size()!= 0);
        
        b2.insert("F");
        
        // making sure nothing changed in origonal structure when copy is changed
        assert(b1.count("A") == 1);
        assert(b1.count("B") == 1);
        assert(b1.tally(0) == 1);
        assert(b1.tally(1) == 1);
        assert(b1.empty()==false);
        assert(b1.size()!= 0);
        
        assert(b2.count("A") == 1);
        assert(b2.count("B") == 1);
        assert(b2.count("C") == 0);
        assert(b2.count("D") == 0);
        assert(b2.count("F") == 1);
        assert(b2.tally(0) == 1);
        assert(b2.tally(1) == 1);
        assert(b2.tally(2) == 0);
        assert(b2.tally(3) == 0);
        assert(b2.tally(4) == 1);
        assert(b2.empty()==false);
        assert(b1.size()!= 0);
        
        // swap is being tested with move.... 
        // because move construction uses swap and since move construction
        // requires swap to work then and move construction is just one call i 
        // figured why not just call the move constructor instead
        
        // going to be used to compair agains after a move is completed
        T b3(b2);
        T b4(move(b2));        
       
        assert(b4.count("A") == b3.count("A"));
        assert(b4.count("B") == b3.count("B"));
        assert(b4.count("C") == b3.count("C"));
        assert(b4.count("D") == b3.count("D"));
        assert(b4.count("F") == b3.count("F"));
        assert(b4.tally(0) == b3.tally(0));
        assert(b4.tally(1) == b3.tally(1));
        assert(b4.tally(2) == b3.tally(2));
        assert(b4.tally(3) == b3.tally(3));
        assert(b4.tally(4) == b3.tally(4));
        assert(b4.empty()==false);
        assert(b1.size()!= 0);
    }


    {    
        vector<string> data={"A","B","C","D","F"};
        T b1(data.begin(),data.end());

        //verifying that data was measured correctly
        assert(b1.count("A") == 1);
        assert(b1.count("B") == 1);
        assert(b1.count("C") == 1);
        assert(b1.count("D") == 1);
        assert(b1.count("F") == 1);
        assert(b1.tally(0) == 1);
        assert(b1.tally(1) == 1);
        assert(b1.tally(2) == 1);
        assert(b1.tally(3) == 1);
        assert(b1.tally(4) == 1);
        assert(b1.empty()==false);
        assert(b1.size()!= 0);
    }

    {
        //create a bins instance and adding stuff to it
        vector<string> data={"A","B","C","D","F"};
        T b(data.begin(),data.end());
        
        //create an empty bins instance for stuff to be assigned to
        T b1;
        b1 = b;
        
        //making sure that it copied correctly        
        vector<int> against={1,1,1,1,1,5};
        assert(b1.count("A") == against[0]);
        assert(b1.count("B") == against[1]);
        assert(b1.count("C") == against[2]);
        assert(b1.count("D") == against[3]);
        assert(b1.count("F") == against[4]);

        //checking tally
        assert(b1.tally(0)   == against[0]);
        assert(b1.tally(1)   == against[1]);
        assert(b1.tally(2)   == against[2]);
        assert(b1.tally(3)   == against[3]);
        assert(b1.tally(4)   == against[4]);

        //verifing not empty
        assert(b1.empty()    == false     );

        //checking size
        assert(b1.size()!= 0);
        
        //checking bin function
        assert(b1.bin("A")   == 0);
        assert(b1.bin("B")   == 1);
        assert(b1.bin("C")   == 2);
        assert(b1.bin("D")   == 3);
        assert(b1.bin("F")   == 4);
        
        //reassurance that nothing changed in b
        assert(b.count("A") == against[0]);
        assert(b.count("B") == against[1]);
        assert(b.count("C") == against[2]);
        assert(b.count("D") == against[3]);
        assert(b.count("F") == against[4]);
        //checking tally
        assert(b.tally(0)   == against[0]);
        assert(b.tally(1)   == against[1]);
        assert(b.tally(2)   == against[2]);
        assert(b.tally(3)   == against[3]);
        assert(b.tally(4)   == against[4]);

        //verifing not empty
        assert(b.empty()    == false     );

        //checking size
        assert(b1.size()!= 0);
        
        //checking bin function
        assert(b.bin("A")   == 0);
        assert(b.bin("B")   == 1);
        assert(b.bin("C")   == 2);
        assert(b.bin("D")   == 3);
        assert(b.bin("F")   == 4);
        
        T b2;
        b2 = move(b1);
        // verifying that move successed
        assert(b2.count("A") == against[0]);
        assert(b2.count("B") == against[1]);
        assert(b2.count("C") == against[2]);
        assert(b2.count("D") == against[3]);
        assert(b2.count("F") == against[4]);

        //checking tally
        assert(b2.tally(0)   == against[0]);
        assert(b2.tally(1)   == against[1]);
        assert(b2.tally(2)   == against[2]);
        assert(b2.tally(3)   == against[3]);
        assert(b2.tally(4)   == against[4]);

        //verifing not empty
        assert(b2.empty()    == false     );

        //checking size
        assert(b2.size()!= 0);
        
        //checking bin function
        assert(b2.bin("A")   == 0);
        assert(b2.bin("B")   == 1);
        assert(b2.bin("C")   == 2);
        assert(b2.bin("D")   == 3);
        assert(b2.bin("F")   == 4);
        
        //reassurance that nothing changed in b
                //reassurance that nothing changed in b
        assert(b.count("A") == against[0]);
        assert(b.count("B") == against[1]);
        assert(b.count("C") == against[2]);
        assert(b.count("D") == against[3]);
        assert(b.count("F") == against[4]);
        //checking tally
        assert(b.tally(0)   == against[0]);
        assert(b.tally(1)   == against[1]);
        assert(b.tally(2)   == against[2]);
        assert(b.tally(3)   == against[3]);
        assert(b.tally(4)   == against[4]);

        //verifing not empty
        assert(b.empty()    == false     );

        //checking size
        assert(b.size()!= 0);
        
        //checking bin function
        assert(b.bin("A")   == 0);
        assert(b.bin("B")   == 1);
        assert(b.bin("C")   == 2);
        assert(b.bin("D")   == 3);
        assert(b.bin("F")   == 4);
        
    }
}
