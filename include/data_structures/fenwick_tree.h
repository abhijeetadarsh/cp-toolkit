#pragma once
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

#include <vector>
#include <type_traits>
#include <numeric>
#include <cmath> // For std::log2

namespace cp
{
    /**
     * @brief A Fenwick Tree (or Binary Indexed Tree) for fast prefix sum queries.
     * @tparam T The numeric type of the elements (e.g., int, long long, double).
     *
     * This data structure supports point updates and range sum queries in O(log N) time.
     * It uses 1-based indexing for its internal operations and public-facing API,
     * but can be built from a standard 0-indexed vector.
     */
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    class FenwickTree
    {
    private:
        /// @brief The internal storage for the Fenwick Tree. Size is N+1.
        std::vector<T> m_tree;

        /**
         * @brief Calculates the least significant one bit of an integer.
         * @param i The input integer.
         * @return The value of the least significant bit.
         */
        static constexpr int lso(int i)
        {
            return i & -i;
        }

    public:
        /**
         * @brief Constructs an empty Fenwick Tree of a given size.
         * @param size The number of elements (N). The tree will be size N+1.
         * @complexity O(N)
         */
        FenwickTree(size_t size) : m_tree(size + 1, T(0)) {}

        /**
         * @brief Constructs a Fenwick Tree from an initial list of values.
         * @param values A 0-indexed vector of initial values.
         * @complexity O(N) where N is the number of values.
         */
        FenwickTree(const std::vector<T> &values)
        {
            build(values);
        }

        /**
         * @brief Constructs a Fenwick Tree from a frequency map of items.
         * @param max_value The maximum possible value of an item in the list.
         * @param items A vector of integers representing items to be counted.
         * @complexity O(M + K) where M is max_value and K is the number of items.
         */
        FenwickTree(size_t max_value, const std::vector<int> &items)
        {
            std::vector<T> frequencies(max_value + 1, T(0));
            for (int item : items)
            {
                if (item > 0 && item <= max_value)
                {
                    ++frequencies[item];
                }
            }
            // Build requires a 0-indexed vector, but frequencies are 1-indexed.
            // Create a temporary 0-indexed vector from the frequencies.
            build(std::vector<T>(frequencies.begin() + 1, frequencies.end()));
        }

        /**
         * @brief Re-initializes the tree using the efficient O(N) build algorithm.
         * @param values A 0-indexed vector of initial values.
         */
        void build(const std::vector<T> &values)
        {
            size_t n = values.size();
            m_tree.assign(n + 1, T(0));
            for (size_t i = 0; i < n; ++i)
            {
                m_tree[i + 1] = values[i];
            }

            for (size_t i = 1; i <= n; ++i)
            {
                size_t parent = i + lso(i);
                if (parent <= n)
                {
                    m_tree[parent] += m_tree[i];
                }
            }
        }

        /**
         * @brief Adds a delta value to the element at a given index.
         * @param i The 1-based index of the element to update.
         * @param delta The value to add.
         * @complexity O(log N)
         */
        void add(int i, T delta)
        {
            for (; i < m_tree.size(); i += lso(i))
            {
                m_tree[i] += delta;
            }
        }

        /**
         * @brief Calculates the prefix sum from index 1 up to `j`.
         * @param j The 1-based end index of the range.
         * @return The sum of the range [1, j].
         * @complexity O(log N)
         */
        T query(int j) const
        {
            T sum = T(0);
            for (; j > 0; j -= lso(j))
            {
                sum += m_tree[j];
            }
            return sum;
        }

        /**
         * @brief Calculates the sum of the range [i, j].
         * @param i The 1-based start index of the range.
         * @param j The 1-based end index of the range.
         * @return The sum of elements from index i to j.
         * @complexity O(log N)
         */
        T query_range(int i, int j) const
        {
            if (i > j)
            {
                return T(0);
            }
            return query(j) - query(i - 1);
        }

        /**
         * @brief Finds the smallest index `i` with a prefix sum of at least `k`.
         * Requires all elements in the tree to be non-negative.
         * @param k The target cumulative frequency.
         * @return The 1-based index of the k-th element.
         * @complexity O(log N)
         */
        int find_kth(T k) const
        {
            int pos = 0;
            T current_sum = T(0);

            int log_n = m_tree.empty() ? 0 : static_cast<int>(std::log2(m_tree.size() - 1));

            for (int bit = (1 << log_n); bit > 0; bit >>= 1)
            {
                if (pos + bit < m_tree.size())
                {
                    if (current_sum + m_tree[pos + bit] < k)
                    {
                        current_sum += m_tree[pos + bit];
                        pos += bit;
                    }
                }
            }
            return pos + 1;
        }

        /**
         * @brief Gets the total number of elements the Fenwick Tree was sized for.
         * @return The size (N).
         */
        size_t size() const
        {
            return m_tree.size() - 1;
        }
    };
}

/*
 * Range Update Point Query (RUPQ) Fenwick Tree
 * Operations: O(log m) range update(ui, uj, v) and O(log m) point query(i)
 * Operations: O(log m) range update(ui, uj, v) and O(log m) rsq(i, j)
 */

/*
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