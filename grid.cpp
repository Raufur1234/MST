#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>

using namespace std;

// Structure to represent an edge in the grid
struct Edge {
    int u, v; // Node IDs (flattened index)
    int w;    // Weight: |Height[u] - Height[v]|
};

// Comparator for sorting edges
bool compareEdges(const Edge& a, const Edge& b) {
    return a.w < b.w;
}

// Disjoint Set Union (DSU) structure
struct DSU {
    vector<int> parent;
    int num_components; // Track number of disjoint sets

    DSU(int n) {
        parent.resize(n);
        iota(parent.begin(), parent.end(), 0);
        num_components = n; // Initially, every node is its own component
    }

    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]);
    }

    bool unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            parent[root_i] = root_j;
            num_components--; // Two components merged into one
            return true;
        }
        return false;
    }
};

int main() {
    // Optimization for faster I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int R, C;
    long long K; // K can be large, use long long for cost calculations
    if (!(cin >> R >> C >> K)) return 0;

    // Use a 1D vector to store the grid for easier indexing
    // Index = r * C + c
    vector<int> heights(R * C);
    for (int i = 0; i < R * C; ++i) {
        cin >> heights[i];
    }

    vector<Edge> edges;
    edges.reserve(2 * R * C); // Reserve memory to avoid reallocations

    // Helper lambda to get 1D index
    auto getIdx = [&](int r, int c) {
        return r * C + c;
    };

    // Step 1: Build Grid Edges
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            int u = getIdx(r, c);

            // Check Right Neighbor
            if (c + 1 < C) {
                int v = getIdx(r, c + 1);
                int w = abs(heights[u] - heights[v]);
                // Optimization: If edge is already >= K, we will never pick it 
                // over a teleporter. We can discard it early (optional but good).
                if (w < K) {
                    edges.push_back({u, v, w});
                }
            }

            // Check Bottom Neighbor
            if (r + 1 < R) {
                int v = getIdx(r + 1, c);
                int w = abs(heights[u] - heights[v]);
                if (w < K) {
                    edges.push_back({u, v, w});
                }
            }
        }
    }

    // Step 2: Sort valid grid edges
    sort(edges.begin(), edges.end(), compareEdges);

    // Step 3: Kruskal's Algorithm
    DSU dsu(R * C);
    long long total_cost = 0;
    
    for (const auto& edge : edges) {
        // Since we filtered edges >= K above, we don't need to check w < K here.
        if (dsu.unite(edge.u, edge.v)) {
            total_cost += edge.w;
        }
    }

    // Step 4: Connect remaining components with Teleporters
    // We currently have 'dsu.num_components' disjoint sets.
    // To connect C components, we need C - 1 edges.
    // Since we ran out of "cheap" grid edges, all remaining connections cost K.
    
    long long teleporters_needed = dsu.num_components - 1;
    total_cost += teleporters_needed * K;

    cout << total_cost << endl;

    return 0;
}
