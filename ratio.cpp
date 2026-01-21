#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip> // Required for setprecision

using namespace std;

const double INF = 1e18; // Use a very large number for double comparison

struct Edge {
    int u, v;
    double w; // Changed to double for ratio calculation
};

bool comparator(const Edge &a, const Edge &b) {
    return a.w < b.w;
}

struct DSU {
    vector<int> parent, rnk;
    int num_components; // [ADDED] To track connectivity

    DSU(int n) {
        // [MODIFIED] Resize to n+1 to handle 1-based indexing safely
        parent.resize(n + 1);
        rnk.assign(n + 1, 0);
        num_components = n; // Initially N disjoint components
        for (int i = 0; i <= n; i++) { 
            parent[i] = i;
        }
    }

    int find_set(int v) {
        if (parent[v] == v) return v;
        else return parent[v] = find_set(parent[v]);
    }

    bool unite(int a, int b) {
        a = find_set(a);
        b = find_set(b);
        if (a == b) return false;
        
        // [ADDED] Decrease component count on successful merge
        num_components--; 

        if (rnk[a] < rnk[b]) {
            swap(a, b);
        }
        parent[b] = a;
        if (rnk[a] == rnk[b]) rnk[a]++;
        return true;
    }
};

// [NEW] Logic for Minimum Ratio Problem
void solve_min_ratio(int n, vector<Edge> &edges) {
    int m = edges.size();
    
    // Step 1: Sort edges by weight (diameter)
    sort(edges.begin(), edges.end(), comparator);

    double min_ratio = INF;
    bool found = false;

    // Step 2: Sliding Window Strategy
    // 'i' is the index of the "Smallest Edge" in our current window
    for (int i = 0; i < m; i++) {
        
        // Optimization: If we don't have enough edges left to connect N nodes, stop.
        if (m - i < n - 1) break;

        DSU dsu(n);
        
        // 'j' is the index of the "Largest Edge" we are trying to add
        for (int j = i; j < m; j++) {
            dsu.unite(edges[j].u, edges[j].v);

            // Check if the graph is fully connected
            if (dsu.num_components == 1) {
                double current_ratio = edges[j].w / edges[i].w;
                
                if (current_ratio < min_ratio) {
                    min_ratio = current_ratio;
                }
                found = true;
                
                // IMPORTANT: Since edges are sorted, extending 'j' further 
                // will only increase the max weight (numerator), making the ratio worse.
                // So we break and try the next starting position 'i'.
                break; 
            }
        }
    }

    if (found) {
        // Output with precision (usually required for ratio problems)
        cout << fixed << setprecision(3) << min_ratio << endl;
    } else {
        cout << "-1" << endl;
    }
}

int main() {
    // Fast I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<Edge> edges(m);

    for (int i = 0; i < m; i++) {
        int u, v;
        double w;
        cin >> u >> v >> w;
        
        Edge e;
        e.u = u;
        e.v = v;
        e.w = w;
        edges[i] = e;
    }
    
    // Call the specific solver
    solve_min_ratio(n, edges);

    return 0;
}