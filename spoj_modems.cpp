#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>

using namespace std;

// Structure to represent a town's coordinates
struct Point {
    int x, y;
};

// Structure to represent a potential connection (Edge)
struct Edge {
    int u, v;       // The indices of the two towns
    double weight;  // The calculated cost (distance * U or V)
    bool is_utp;    // True if UTP, False if Fiber

    // Overload < operator for sorting edges by weight
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

// Disjoint Set Union (DSU) structure for Kruskal's Algorithm
struct DSU {
    vector<int> parent;
    int num_components;

    DSU(int n) {
        parent.resize(n);
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
        num_components = n;
    }

    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]);
    }

    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            parent[root_i] = root_j;
            num_components--;
        }
    }
};

double get_dist(Point p1, Point p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void solve(int tc) {
    int N, R, W, U, V;
    if (!(cin >> N >> R >> W >> U >> V)) return;

    vector<Point> towns(N);
    for (int i = 0; i < N; i++) {
        cin >> towns[i].x >> towns[i].y;
    }

    // Edge case: If we have enough modems for every town, cost is 0
    if (W >= N) {
        cout << "Caso #" << tc << ": 0.000 0.000" << endl;
        return;
    }

    vector<Edge> edges;
    edges.reserve(N * (N - 1) / 2);

    // Generate all possible edges
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            double d = get_dist(towns[i], towns[j]);
            double cost;
            bool is_utp;

            if (d <= R) {
                cost = d * U;
                is_utp = true;
            } else {
                cost = d * V;
                is_utp = false;
            }
            edges.push_back({i, j, cost, is_utp});
        }
    }

    // Sort edges by cost (Kruskal's greedy step)
    sort(edges.begin(), edges.end());

    DSU dsu(N);
    double cost_utp = 0;
    double cost_fiber = 0;

    // Process edges
    for (const auto& edge : edges) {
        if (dsu.find(edge.u) != dsu.find(edge.v)) {
            dsu.unite(edge.u, edge.v);
            
            if (edge.is_utp) {
                cost_utp += edge.weight;
            } else {
                cost_fiber += edge.weight;
            }
        }
        
        // Stop when we have reduced the graph to W connected components
        if (dsu.num_components == W) {
            break;
        }
    }

    cout << "Caso #" << tc << ": " 
         << fixed << setprecision(3) << cost_utp << " " 
         << cost_fiber << endl;
}

int main() {
    int t;
    if (cin >> t) {
        for (int i = 1; i <= t; i++) {
            solve(i);
        }
    }
    return 0;
}