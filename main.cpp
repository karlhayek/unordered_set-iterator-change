#include <iostream>
#include "unordered_set.h"
using namespace std;

void printSet(unordered_set<string>& set){
    cout << "myset contains:";
    for (auto it = set.begin(); it != set.end(); it++)
        cout << " " << *it;
    cout << endl;
}

void printRandomSet(unordered_set<string>& set){
    cout << "myset contains:";
    for (auto it = set.begin_random(); it != set.end(); it++)
        cout << " " << *it;
    cout<<endl;
}

int main ()
{
    unordered_set<string> myset = {"Mercury","Venus","Earth","Mars","Jupiter","Saturn","Uranus","Neptune"};

    printSet(myset);
    printRandomSet(myset);
    printRandomSet(myset);
    printRandomSet(myset);
    cout<<endl;

    auto it = myset.begin();
    auto it_rand = myset.begin_random();

    myset.insert("Pluto");

    for (; it != myset.end(); it++) {
        cout<<*it<<" ";         // Pluto will not be printed
    }
    cout<<endl;

    for (; it_rand != myset.end(); it_rand++) {
        cout<<*it_rand<<" ";    // Pluto will not be printed
    }
    cout<<endl;

    printSet(myset);            // Pluto will be printed


}
