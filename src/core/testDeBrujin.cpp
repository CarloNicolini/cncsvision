// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2014 Carlo Nicolini <carlo.nicolini@iit.it>
//
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CNCSVision is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// CNCSVision. If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <deque>
#include <algorithm>
#include <boost/function.hpp>

using namespace std;

/**
    De Bruijn sequences can be described by two parameters:
    k  the number of entities in the alphabet e.g. {0,1,2,3,4,5,6,7,8,9} for k=10
    n  the order (length of sub-sequence required) e.g. n=4 for a four digit long PIN.
These are typically describe by the representation B(k,n).
For our PIN example, the notation would be B(10,4)
**/
static std::deque< std::map<std::string, string> > result;
/**
 * @brief debruijn
 * @param t
 * @param p
 * @param k is the input string ariety. i.e. the number of different symbols (alphabet)
 * @param n is the length of the sub-sequence
 * @param a
 * @param callback
 */
void debruijn(unsigned int t, unsigned int p, const unsigned int k, const unsigned int n, unsigned int* a, boost::function<void (unsigned int*,unsigned int*)> printerFunction)
{
    if (t > n)
    {
        // we want only necklaces, not pre-necklaces or Lyndon words
        if (n % p == 0) {
            printerFunction(a+1, a+p+1);
        }
    }
    else
    {
        a[t] = a[t-p];
        debruijn(t+1, p, k, n, a, printerFunction);
        for (unsigned int j = a[t-p]+1; j < k; ++j)
        {
            a[t] = j;
            debruijn(t+1, t, k, n, a, printerFunction);
        }
    }
}

/**
 * @brief The seq_printer struct
 */
struct seq_printer
{
    const std::vector<char>& _alpha;
    seq_printer(const std::vector<char>& alpha) : _alpha(alpha) {}
    void operator() (unsigned int* a, unsigned int* a_end) const {
        for (unsigned int* i = a; i < a_end; ++i)
        {
            std::cout << _alpha[*i];
        }
    }
};


/**
 * @brief The trial_printer struct
 */
struct trial_printer
{
    const std::deque< std::map<std::string, string> > & _alpha;
    trial_printer(const std::deque< std::map<std::string, string>  > &alpha) : _alpha(alpha){}
    void operator() (unsigned int* a, unsigned int* a_end) const
    {
        for (unsigned int* i = a; i < a_end; ++i)
        {
            std::map<std::string, string> xx = (_alpha[*i]);
            for ( std::map<std::string, string>::iterator iter=xx.begin(); iter!=xx.end();++iter )
            {
                //cout << iter->first << ": " << iter->second << endl;
                cout << (iter->second);
                result.push_back(_alpha[*i]);
            }
        }
    }
};

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    int N=2;

    std::vector<char> alpha;
    alpha.push_back('0');
    alpha.push_back('1');
    alpha.push_back('2');
    alpha.push_back('3');
    alpha.push_back('4');

    std::deque< std::map<string, string> > extractions;

    std::map<string, string> trial0,trial1,trial2,trial3,trial4;
    trial0["color"]="0";
    //trial0["size"]="large ";

    trial1["color"]="1";
    //trial1["size"]="small";

    trial2["color"]="2";
    //trial2["size"]="medium";

    trial3["color"]="3";
    trial4["color"]="4";

    extractions.push_back(trial0);
    //extractions.push_back(trial1);
    //extractions.push_back(trial2);
    //extractions.push_back(trial3);
    //extractions.push_back(trial4);

    unsigned int* a = new unsigned int[N+1];
    a[0] = 0;

    //cout << "sequence alpha: ";
    //debruijn(1, 1, alpha.size(), N, a, seq_printer(alpha));
    cout << "\nsequence extractions: ";
    debruijn(1, 1, extractions.size(), N, a, trial_printer(extractions));

    std::cerr << std::endl;

    // To wrap the sequence !
    if ( N > 1 )
        std::cerr << alpha[0];
    std::cout << std::endl;
    std::cerr << std::endl;
/*
    extractions.clear();
    for ( int i=0; i<result.size(); i++ )
    {
        extractions.push_back( result[i]);
    }

    for ( int i=0; i<extractions.size(); i++ )
    {
        std::map<std::string, string> xx = (extractions[i]);
        for ( std::map<std::string, string>::iterator iter=xx.begin(); iter!=xx.end();++iter )
            cerr << iter->second ;
        //cout << iter->first << " " << iter->second ;
        cerr << endl;
    }
*/
    extractions = result;
    for ( int i=0; i<extractions.size(); i++ )
    {
        std::map<std::string, string> xx = (extractions[i]);
        for ( std::map<std::string, string>::iterator iter=xx.begin(); iter!=xx.end();++iter )
            cerr << iter->second ;
        //cout << iter->first << " " << iter->second ;
        cerr << endl;
    }
    delete[] a;
}
