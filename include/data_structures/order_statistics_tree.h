#pragma once
/*
 * Order Statistics Tree, dynamic data
 * O(n log n) pre-processing and O(log n) algorithm using balanced BST
 * finding the k-th smallest element (find_by_order)
 * counting elements strictly less than a given value (order_of_key)
 */
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

template <
    typename Key,
    typename Value = __gnu_pbds::null_type,
    typename Cmp_Fn = std::less<Key>>

using OrderStatisticsTree = __gnu_pbds::tree<
    Key,
    Value,
    Cmp_Fn,
    __gnu_pbds::rb_tree_tag,
    __gnu_pbds::tree_order_statistics_node_update>;

/*
int main()
{
    int n = 9;
    int A[] = {2, 4, 7, 10, 15, 23, 50, 65, 71};
    OrderStatisticsTree<int> tree;
    for (int i = 0; i < n; ++i)
    {
        tree.insert(A[i]);
    }

    // O(log n) select
    printf("%d", *tree.find_by_order(0));
    printf("%d", *tree.find_by_order(n - 1));
    printf("%d", *tree.find_by_order(4));

    // O(log n) rank
    printf("%d", tree.order_of_key(2));
    printf("%d", tree.order_of_key(71));
    printf("%d", tree.order_of_key(15));
    return 0;
}
*/