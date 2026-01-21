#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

// Structure for original graph edges
struct Edge {
    int u, v, w;
    bool operator<(const Edge& other) const {
        return w < other.w;
    }
};

// Structure for queries to keep track of original index
struct Query {
    int u, v, w;
    int id; // To print answers in the correct order
    bool operator<(const Query& other) const {
        return w < other.w;
    }
};

// Standard DSU
struct DSU {
    vector<int> parent;
    DSU(int n) {
        parent.resize(n + 1);
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            parent[root_i] = root_j;
        }
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<Edge> edges(m);
    for (int i = 0; i < m; i++) {
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
    }

    // 1. Sort original edges by weight
    sort(edges.begin(), edges.end());

    int q;
    cin >> q;
    vector<Query> queries(q);
    for (int i = 0; i < q; i++) {
        cin >> queries[i].u >> queries[i].v >> queries[i].w;
        queries[i].id = i;
    }

    // 2. Sort queries by weight
    sort(queries.begin(), queries.end());

    DSU dsu(n);
    vector<string> results(q);
    int edge_idx = 0;

    // 3. Process queries
    for (const auto& query : queries) {
        // Add all original edges that are strictly lighter than the current query
        while (edge_idx < m && edges[edge_idx].w < query.w) {
            dsu.unite(edges[edge_idx].u, edges[edge_idx].v);
            edge_idx++;
        }

        // Check connectivity without modifying DSU for the query edge
        if (dsu.find(query.u) != dsu.find(query.v)) {
            results[query.id] = "Yes";
        } else {
            results[query.id] = "No";
        }
    }

    // 4. Output results in original order
    for (const string& res : results) {
        cout << res << "\n";
    }

    return 0;
}
