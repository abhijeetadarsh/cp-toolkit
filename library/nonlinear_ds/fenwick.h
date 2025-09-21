/*
 * Fenwick (Binary Indexed) Tree
 * Operation: O(log m) rsq(j)
 * Operation: O(log m) rsq(i, j)
 * Operation: O(log m) update(i, v)
 * Operation: O(n + m) build(frequency-array f)
 * Operation: O(log2 m) select(rank k)
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

// The basic version of PURQ Fenwick Tree supports both RSQ (range query) and (point)
// update operations in just O(m) space and O(log m) time per RSQ/Point Update given
// a set of n integer keys that ranges from [1..m]. In the complete implementation, we
// add the slightly more complex alternative constructors from frequency array, the O(log2 m)
// select(k) operation, and the RUPQ and RURQ variants of Fenwick Tree

#include <bits/stdc++.h>
using namespace std;

#define LSOne(S) ((S) & -(S))

class FenwickTree
{
private:
    vector<long long> ft;

public:
    FenwickTree(int m) { ft.assign(m + 1, 0); }
    void build(const vector<long long> &f)
    {
        int m = (int)f.size() - 1;
        ft.assign(m + 1, 0);
        for (int i = 1; i <= m; ++i)
        {
            ft[i] += f[i];
            if (i + LSOne(i) <= m)
                ft[i + LSOne(i)] += ft[i];
        }
    }
    FenwickTree(const vector<long long> &f) { build(f); }
    FenwickTree(int m, const vector<int> &s)
    {
        vector<long long> f(m + 1, 0);
        for (int i = 0; i < (int)s.size(); ++i)
            ++f[s[i]];
        build(f);
    }
    long long rsq(int j)
    {
        long long sum = 0;
        for (; j; j -= LSOne(j))
            sum += ft[j];
        return sum;
    }
    long long rsq(int i, int j) { return rsq(j) - rsq(i - 1); } // inc/exclusion
    // updates value of the i-th element by v (v can be +ve/inc or -ve/dec)
    void update(int i, long long v)
    {
        for (; i < (int)ft.size(); i += LSOne(i))
            ft[i] += v;
    }
    int select(long long k)
    {
        int lo = 1, hi = ft.size() - 1;
        for (int i = 0; i < 30; ++i)
        {
            int mid = (lo + hi) / 2;
            (rsq(1, mid) < k) ? lo = mid : hi = mid;
        }
        return hi;
    }
};

/*
 * Range Update Point Query (RUPQ) Fenwick Tree
 * Operations: O(log m) range update(ui, uj, v) and O(log m) point query(i)
 * Operations: O(log m) range update(ui, uj, v) and O(log m) rsq(i, j)
 */

class RUPQ
{
private:
    FenwickTree ft;

public:
    RUPQ(int m) : ft(FenwickTree(m)) {}
    void range_update(int ui, int uj, int v)
    {
        ft.update(ui, v);
        ft.update(uj + 1, -v);
    }
    long long point_query(int i) { return ft.rsq(i); }
};
// RUPQ variant
// internalong longy use PURQ FT
// [ui, ui+1, .., m] +v
// [uj+1, uj+2, .., m] -v
// [ui, ui+1, .., uj] +v
// rsq(i) is sufficient
class RURQ
{
    // RURQ variant
private:
    // needs two helper FTs
    RUPQ rupq;
    // one RUPQ and
    FenwickTree purq;
    // one PURQ
public:
    RURQ(int m) : rupq(RUPQ(m)), purq(FenwickTree(m)) {} // initialization
    void range_update(int ui, int uj, int v)
    {
        rupq.range_update(ui, uj, v);
        // [ui, ui+1, .., uj] +v
        purq.update(ui, v * (ui - 1));
        // -(ui-1)*v before ui
        purq.update(uj + 1, -v * uj);
        // +(uj-ui+1)*v after uj
    }
    long long rsq(int j)
    {
        return rupq.point_query(j) * j - purq.rsq(j);
    }
    long long rsq(int i, int j) { return rsq(j) - rsq(i - 1); } // standard
};

/*
int main()
{
    vector<long long> f = {0, 0, 1, 0, 1, 2, 3, 2, 1, 1, 0}; // index 0 is always 0

    FenwickTree ft(f);

    printf("%long longd\n", ft.rsq(1, 6));  // 7 => ft[6]+ft[4] = 5+2 = 7
    printf("%d\n", ft.select(7));    // index 6, rsq(1, 6) == 7, which is >= 7
    ft.update(5, 1);                 // update demo
    printf("%long longd\n", ft.rsq(1, 10)); // now 12


    printf("=====\n");
    RUPQ rupq(10);
    RURQ rurq(10);

    rupq.range_update(2, 9, 7); // indices in [2, 3, .., 9] updated by +7
    rurq.range_update(2, 9, 7); // same as rupq above
    rupq.range_update(6, 7, 3); // indices 6&7 are further updated by +3 (10)
    rurq.range_update(6, 7, 3); // same as rupq above

    // idx = 0 (unused) | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |10
    // val = - | 0 | 7 | 7 | 7 | 7 |10 |10 | 7 | 7 | 0
    for (int i = 1; i <= 10; i++) {
        printf("%d -> %long longd\n", i, rupq.point_query(i));
    }

    printf("RSQ(1, 10) = %long longd\n", rurq.rsq(1, 10)); // 62
    printf("RSQ(6, 7) = %long longd\n", rurq.rsq(6, 7));   // 20
    return 0;
}
*/