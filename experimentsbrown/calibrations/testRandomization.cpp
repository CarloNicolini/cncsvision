#include <iostream>
#include <vector>
#include <algorithm>

#include "Util.h"
#include "Mathcommon.h"
#include "BalanceFactor.h"

using namespace std;
using namespace util;
using namespace mathcommon;

int main()
{
	cerr << "Press 1 to set random seed as \"srand(time(0))\", 0 to set fixed seed \"srand(0)\"" << endl;
	int val; 
	cin >> val;
	if (val == 1 )
		srand(time(0));
	else
		srand(0);
	
	
	BalanceFactor<int> balance;
	balance.init(1,1);
	
	balance.addFactor( "FactorA",  vlist_of<int>(0)(1) );
	balance.addFactor( "FactorB", vlist_of<int>(2)(3)  );
	//balance.addFactor( "FactorC", vlist_of<int>(4)(5)  );

	
	balance.print();
	
	cerr << "Press any key then enter to exit" << endl;
	char x='a';
	while (x=='a')
	{
		cin >> x;
	}
	
	return 0;
}
