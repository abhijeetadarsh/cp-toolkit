#pragma once
/*
 * Order Statistics Tree, dynamic data
 * O(n log n) pre-processing and O(log n) algorithm using balanced BST
 * finding the k-th smallest element (find_by_order)
 * counting elements strictly less than a given value (order_of_key)
 */
#include <bits/extc++.h>

using namespace __gnu_pbds;
typedef tree<int, null_type, std::less<int>, rb_tree_tag, tree_order_statistics_node_update> ost;

/*
int main()
{
    int n = 9;
    int A[] = {2, 4, 7, 10, 15, 23, 50, 65, 71};
    ost tree;
    for (int i = 0; i < n; ++i) {
        tree.insert(A[i]);
    }


    // O(log n) select
    cout << *tree.find_by_order(0) << "\n";
    cout << *tree.find_by_order(n - 1) << "\n";
    cout << *tree.find_by_order(4) << "\n";

    // O(log n) rank
    cout << tree.order_of_key(2) << "\n";
    cout << tree.order_of_key(71) << "\n";
    cout << tree.order_of_key(15) << "\n";
    return 0;
}
*/