#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

const int LOG = 20; // Sufficient for N up to ~10^6

struct Edge {
    int u, v, w;
    bool operator<(const Edge& other) const {
        return w < other.w;
    }
};

// DSU for Kruskal's Algorithm
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
        if (root_i != root_j) parent[root_i] = root_j;
    }
};

// Global variables for LCA
vector<vector<pair<int, int>>> mst_adj;
vector<int> depth;
vector<vector<int>> up;
vector<vector<int>> max_w;
vector<int> component; // To handle disconnected graphs

// DFS to build Binary Lifting table
void dfs(int u, int p, int w, int d, int comp_id) {
    depth[u] = d;
    component[u] = comp_id;
    up[u][0] = p;
    max_w[u][0] = w;

    for (int i = 1; i < LOG; i++) {
        up[u][i] = up[up[u][i-1]][i-1];
        // The max weight for 2^i is the max of the two 2^(i-1) segments
        max_w[u][i] = max(max_w[u][i-1], max_w[up[u][i-1]][i-1]);
    }

    for (auto& edge : mst_adj[u]) {
        int v = edge.first;
        int weight = edge.second;
        if (v != p) {
            dfs(v, u, weight, d + 1, comp_id);
        }
    }
}

// Function to find max edge weight on path between u and v
int get_max_on_path(int u, int v) {
    if (depth[u] < depth[v]) swap(u, v);

    int res = 0;

    // 1. Lift u to the same depth as v
    for (int i = LOG - 1; i >= 0; i--) {
        if (depth[u] - (1 << i) >= depth[v]) {
            res = max(res, max_w[u][i]);
            u = up[u][i];
        }
    }

    if (u == v) return res;

    // 2. Lift both until they are just below LCA
    for (int i = LOG - 1; i >= 0; i--) {
        if (up[u][i] != up[v][i]) {
            res = max(res, max_w[u][i]);
            res = max(res, max_w[v][i]);
            u = up[u][i];
            v = up[v][i];
        }
    }

    // 3. Account for the final edges to LCA
    res = max(res, max_w[u][0]);
    res = max(res, max_w[v][0]);
    
    return res;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<Edge> edges(m);
    for (int i = 0; i < m; i++) {
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
    }

    // Sort edges for Kruskal's
    sort(edges.begin(), edges.end());

    // Build MST
    DSU dsu(n);
    mst_adj.resize(n + 1);
    int edges_count = 0;
    
    for (const auto& e : edges) {
        if (dsu.find(e.u) != dsu.find(e.v)) {
            dsu.unite(e.u, e.v);
            mst_adj[e.u].push_back({e.v, e.w});
            mst_adj[e.v].push_back({e.u, e.w});
            edges_count++;
        }
    }

    // Initialize LCA structures
    depth.resize(n + 1);
    component.resize(n + 1, 0);
    up.assign(n + 1, vector<int>(LOG));
    max_w.assign(n + 1, vector<int>(LOG));

    // Run DFS on each component (forest handling)
    int comp_id = 0;
    for (int i = 1; i <= n; i++) {
        if (component[i] == 0) { // If node not visited
            comp_id++;
            // Root of component has parent as itself with weight 0 (or -1)
            dfs(i, i, 0, 0, comp_id);
        }
    }

    int q;
    cin >> q;
    while (q--) {
        int u, v, w;
        cin >> u >> v >> w;

        // If nodes are in different connected components of the original graph,
        // the edge bridges them and is definitely in the MST.
        if (component[u] != component[v]) {
            cout << "Yes\n";
        } else {
            // Check the max weight on the existing path
            int max_path_weight = get_max_on_path(u, v);
            
            // If new edge is cheaper or equal, it can replace the heaviest edge
            if (w <= max_path_weight) {
                cout << "Yes\n";
            } else {
                cout << "No\n";
            }
        }
    }

    return 0;
}
