#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

// Structure to represent an edge
struct Edge {
    int u, v, w;
    int id; // To store original index
};

// Comparator to sort edges by weight
bool compareEdges(const Edge& a, const Edge& b) {
    return a.w < b.w;
}

// Disjoint Set Union (DSU) structure with Path Compression
struct DSU {
    vector<int> parent;
    
    DSU(int n) {
        parent.resize(n + 1);
        // Initialize parent of each node to itself
        iota(parent.begin(), parent.end(), 0);
    }
    
    // Find with path compression
    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]);
    }
    
    // Union two sets
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            parent[root_i] = root_j;
        }
    }
};

int main() {
    // Optimize I/O operations for speed
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<Edge> edges(m);
    for (int i = 0; i < m; ++i) {
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
        edges[i].id = i; // Store 0-based index
    }

    // Sort edges by weight
    sort(edges.begin(), edges.end(), compareEdges);

    DSU dsu(n);
    vector<string> results(m);

    int i = 0;
    while (i < m) {
        int j = i;
        // Identify the range [i, j) of edges with the same weight
        while (j < m && edges[j].w == edges[i].w) {
            j++;
        }

        // Phase 1: Check connectivity for all edges in this weight group
        // We check against the graph formed ONLY by edges strictly smaller than the current weight
        for (int k = i; k < j; ++k) {
            if (dsu.find(edges[k].u) != dsu.find(edges[k].v)) {
                results[edges[k].id] = "YES";
            } else {
                results[edges[k].id] = "NO";
            }
        }

        // Phase 2: Add these edges to the DSU
        // This prepares the graph for the next heavier weight group
        for (int k = i; k < j; ++k) {
            dsu.unite(edges[k].u, edges[k].v);
        }

        // Move to the next group
        i = j;
    }

    // Output results in the original input order
    for (int k = 0; k < m; ++k) {
        cout << results[k] << "\n";
    }

    return 0;
}