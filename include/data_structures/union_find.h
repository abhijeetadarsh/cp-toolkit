#pragma once
/*
 * Union-Find Disjoint Sets
 * UFDS Operation: O(1) findSet(i)
 * UFDS Operation: O(1) isSameSet(i, j)
 * UFDS Operation: O(1) unionSet(i, j)
 */
#include <vector>

class UnionFind
{
private:
    std::vector<int> p, rank, setSize;
    int numSets;

public:
    UnionFind(int N)
    {
        p.assign(N, 0);
        for (int i = 0; i < N; ++i)
        {
            p[i] = i;
        }
        rank.assign(N, 0);
        setSize.assign(N, 1);
        numSets = N;
    }

    int findSet(int i)
    {
        return (p[i] == i) ? i : (p[i] = findSet(p[i]));
    }

    bool isSameSet(int i, int j)
    {
        return findSet(i) == findSet(j);
    }

    int numDisjointSets()
    {
        return numSets;
    }

    int sizeOfSet(int i)
    {
        return setSize[findSet(i)];
    }

    void unionSet(int i, int j)
    {
        if (isSameSet(i, j))
        {
            return;
        }

        int x = findSet(i), y = findSet(j);
        if (rank[x] > rank[y])
        {
            std::swap(x, y);
        }

        p[x] = y;
        if (rank[x] == rank[y])
        {
            ++rank[y];
        }

        setSize[y] += setSize[x];
        --numSets;
    }
};

/*
int main()
{
    UnionFind UF(5); // create 5 disjoint sets

    printf("%d\n", UF.numDisjointSets()); // 5
    UF.unionSet(0, 1);
    printf("%d\n", UF.numDisjointSets()); // 4
    UF.unionSet(2, 3);
    printf("%d\n", UF.numDisjointSets()); // 3
    UF.unionSet(4, 3);

    printf("%d\n", UF.numDisjointSets());                 // 2
    printf("isSameSet(0, 3) = %d\n", UF.isSameSet(0, 3)); // 0 (false)
    printf("isSameSet(4, 3) = %d\n", UF.isSameSet(4, 3)); // 1 (true)

    // 1 for {0, 1} and 3 for {2, 3, 4}
    for (int i = 0; i < 5; ++i)
    {
        printf("findSet(%d) = %d, sizeOfSet(%d) = %d\n", i, UF.findSet(i), i, UF.sizeOfSet(i));
    }

    UF.unionSet(0, 3);
    printf("%d\n", UF.numDisjointSets()); // 1

    // 3 for {0, 1, 2, 3, 4}
    for (int i = 0; i < 5; ++i)
    {
        printf("findSet(%d) = %d, sizeOfSet(%d) = %d\n", i, UF.findSet(i), i, UF.sizeOfSet(i));
    }
    return 0;
}
*/