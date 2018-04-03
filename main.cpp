#include <iostream>
#include "unordered_set.h"
using namespace std;

void printSet(unordered_set<string>& set){
    cout << "myset contains:";
    for (auto it = set.begin(); it != set.end(); it++)
        cout << " " << *it;
    cout << endl;
}

int main ()
{
    unordered_set<string> myset = {"Mercury","Venus","Earth","Mars","Jupiter","Saturn","Uranus","Neptune"};

    printSet(myset);
    printSet(myset);
    printSet(myset);
    cout<<endl;

}
