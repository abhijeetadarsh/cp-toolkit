/*
 * Segment Tree
 * Segment Tree Operation: O(n) build from an Array A
 * Segment Tree Operation: O(log n) RMQ(i, j)
 * Segment Tree Operation: O(log n) Point update(i, i, v)
 * Segment Tree Operation: O(log n) Range update(i, j, v)
 */

// ========================================================================= //
//             Comparison Between Fenwick Tree and Segment Tree              //
// ========================================================================= //
// Feature                 | Fenwick Tree           | Segment Tree           //
// ------------------------|------------------------|----------------------- //
// Build Tree from Array   | O(n + m)               | O(n)                   //
// Static RSQ              | Overkill               | Overkill               //
// Dynamic RMin/MaxQ       | Limited                | Yes                    //
// Dynamic RSQ             | Yes                    | Yes                    //
// Range Query Complexity  | O(log m)               | O(log n)               //
// Point Update Complexity | O(log m)               | O(log n)               //
// Range Update Complexity | O(log m), RURQ variant | O(log n), Lazy Update  //
// Length of Code (Basic)  | Much shorter           | Much longer            //
// Length of Code (Full)   | Long                   | Long                   //
// ========================================================================= //

#include <bits/stdc++.h>
using namespace std;

class SegmentTree
{
private:
    int n;
    vector<int> A, st, lazy;
    // 99P style
    // n = (int)A.size()
    // the arrays
    int l(int p) { return p << 1; }
    int r(int p) { return (p << 1) + 1; }
    // go to left child
    // go to right child
    int conquer(int a, int b)
    {
        if (a == -1)
            return b;
        if (b == -1)
            return a;
        return min(a, b);
    }
    // corner case
    // RMQ
    void build(int p, int L, int R)
    {
        if (L == R)
            st[p] = A[L];
        else
        {
            int m = (L + R) / 2;
            build(l(p), L, m);
            build(r(p), m + 1, R);
            st[p] = conquer(st[l(p)], st[r(p)]);
        }
    }
    // O(n)
    // base case
    void propagate(int p, int L, int R)
    {
        if (lazy[p] != -1)
        {
            st[p] = lazy[p];
            if (L != R)
                lazy[l(p)] = lazy[r(p)] = lazy[p];
            else
                A[L] = lazy[p];
            lazy[p] = -1;
        }
    }
    // has a lazy flag
    // [L..R] has same value
    // not a leaf
    // propagate downwards
    // L == R, a single index
    // time to update this
    // erase lazy flag
    int RMQ(int p, int L, int R, int i, int j)
    {
        // O(log n)
        propagate(p, L, R);
        // lazy propagation
        if (i > j)
            return -1;
        // infeasible
        if ((L >= i) && (R <= j))
            return st[p];
        // found the segment
        int m = (L + R) / 2;
        return conquer(RMQ(l(p), L, m, i, min(m, j)),
                       RMQ(r(p), m + 1, R, max(i, m + 1), j));
    }
    void update(int p, int L, int R, int i, int j, int val)
    { // O(log n)
        propagate(p, L, R);
        // lazy propagation
        if (i > j)
            return;
        if ((L >= i) && (R <= j))
        {
            // found the segment
            lazy[p] = val;
            // update this
            propagate(p, L, R);
            // lazy propagation
        }
        else
        {
            int m = (L + R) / 2;
            update(l(p), L, m, i, min(m, j), val);
            update(r(p), m + 1, R, max(i, m + 1), j, val);
            int lsubtree = (lazy[l(p)] != -1) ? lazy[l(p)] : st[l(p)];
            int rsubtree = (lazy[r(p)] != -1) ? lazy[r(p)] : st[r(p)];
            st[p] = (lsubtree <= rsubtree) ? st[l(p)] : st[r(p)];
        }
    }

public:
    SegmentTree(int sz) : n(sz), st(4 * n), lazy(4 * n, -1) {}
    SegmentTree(const vector<int> &initialA) : SegmentTree((int)initialA.size())
    {
        A = initialA;
        build(1, 0, n - 1);
    }
    void update(int i, int j, int val) { update(1, 0, n - 1, i, j, val); }
    int RMQ(int i, int j) { return RMQ(1, 0, n - 1, i, j); }
};

/*
int main()
{
    vector<int> A = {18, 17, 13, 19, 15, 11, 20, 99}; // make n a power of 2
    SegmentTree st(A);

    printf(" Idx : 0, 1, 2, 3, 4, 5, 6, 7\n");
    printf("A is {18,17,13,19,15,11,20,99}\n");

    printf("RMQ(1, 3) = %d\n", st.RMQ(1, 3)); // 13
    printf("RMQ(4, 7) = %d\n", st.RMQ(4, 7)); // 11
    printf("RMQ(3, 4) = %d\n", st.RMQ(3, 4)); // 15

    st.update(5, 5, 77); // update A[5] to 77

    printf("               Idx : 0, 1, 2, 3, 4, 5, 6, 7\n");
    printf("Now, modify A into {18,17,13,19,15,77,20,99}\n");

    printf("RMQ(1, 3) = %d\n", st.RMQ(1, 3)); // remains 13
    printf("RMQ(4, 7) = %d\n", st.RMQ(4, 7)); // now 15
    printf("RMQ(3, 4) = %d\n", st.RMQ(3, 4)); // remains 15

    st.update(0, 3, 30); // update A[0..3] to 30

    printf("               Idx : 0, 1, 2, 3, 4, 5, 6, 7\n");
    printf("Now, modify A into {30,30,30,30,15,77,20,99}\n");

    printf("RMQ(1, 3) = %d\n", st.RMQ(1, 3)); // now 30
    printf("RMQ(4, 7) = %d\n", st.RMQ(4, 7)); // remains 15
    printf("RMQ(3, 4) = %d\n", st.RMQ(3, 4)); // remains 15

    st.update(3, 3, 7); // update A[3] to 7

    printf("               Idx : 0, 1, 2, 3, 4, 5, 6, 7\n");
    printf("Now, modify A into {30,30,30, 7,15,77,20,99}\n");

    printf("RMQ(1, 3) = %d\n", st.RMQ(1, 3)); // now 7
    printf("RMQ(4, 7) = %d\n", st.RMQ(4, 7)); // remains 15
    printf("RMQ(3, 4) = %d\n", st.RMQ(3, 4)); // now 7
    return 0;
}
*/