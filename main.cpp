#include <iostream>
#include "set.h"

int main()
{

    set<int> set;

    for (auto e : { 0,7,6,2,3 })
        set.insert(e);

    for (auto e : set) {
        printf("%d ",e);
    }

    printf("\n");
    set.erase(2);

    for (auto &e : set) {
        printf("%d ", e);
    }

    printf("\n%d %d", *set.lower_bound(2), *set.upper_bound(2));
    return 0;
}