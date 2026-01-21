#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

// Structure to represent a friendship (edge)
struct Edge {
    int u, v;
    long long w; // Strength S
};

// Comparator: Sort descending to prioritize strongest edges
bool compareEdges(const Edge& a, const Edge& b) {
    return a.w > b.w; // Descending order
}

struct DSU {
    vector<int> parent;
    int num_components; // Track number of clusters

    DSU(int n) {
        parent.resize(n + 1);
        num_components = n; // Initially N disjoint people
        iota(parent.begin(), parent.end(), 0);
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
            num_components--; // 1 merge = 1 less component
            return true;
        }
        return false;
    }
};

void solve() {
    int N, M, K;
    if (!(cin >> N >> M >> K)) return;

    vector<Edge> edges(M);
    for (int i = 0; i < M; ++i) {
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
    }

    // Step 1: Sort descending (Maximum Spanning logic)
    sort(edges.begin(), edges.end(), compareEdges);

    DSU dsu(N);
    long long total_strength = 0;
    
    // Step 2: Kruskal's - Stop when we hit K components
    for (const auto& edge : edges) {
        // Optimization: If we already have K clusters, we don't need more edges.
        // Adding more edges would reduce components below K.
        if (dsu.num_components == K) break;

        if (dsu.unite(edge.u, edge.v)) {
            total_strength += edge.w;
        }
    }

    // Check if we actually reached K components
    if (dsu.num_components > K) {
        // This happens if the graph was too disconnected to begin with
        // (e.g., requested 2 clusters but graph has 5 isolated islands)
        cout << "-1" << endl; 
    } else {
        cout << total_strength << endl;
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    solve();
    return 0;
}
