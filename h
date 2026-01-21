#include <bits/stdc++.h>
using namespace std;

const int INF = 1e9;


struct node{
	int id, cost;
	bool has_elec;
};


vector<int> node_costs; 


long long solvewith_prim(int n, int root, const vector<vector<pair<int,int>>> &adj, vector<node> &a) {
	vector<int> best(n + 1, INF); 
	vector<int> parent(n + 1, -1);
	vector<bool> used(n + 1, false);

	priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

	best[root] = 0;
	pq.push({0, root});

	long long totalWeight = 0;

	while (!pq.empty()) {
		auto cur = pq.top();
		pq.pop();

		int u = cur.second;
		int w = cur.first;

		if (used[u]) continue;
		used[u] = true;
		totalWeight += w;

		for (auto &edge : adj[u]) {
			int v = edge.first;
			int w = edge.second;
            
           
			if (!a[v].has_elec && w < best[v] && w < a[v].cost) {
				best[v] = w;
				parent[v] = u;
				pq.push({best[v], v});
			}
		}
	}

	return totalWeight;
}

bool comparator2(const node &a, const node &b) {
    return a.cost < b.cost;
}

int main() {
	int n;
	cin>>n;
	

	vector<vector<pair<int,int>>> adj(n + 1);
	vector<node> a(n + 1);
    node_costs.resize(n + 1); 

	
	for(int i = 1; i <= n; i++){
		cin >> a[i].cost;
		a[i].id = i;
		a[i].has_elec = false;
        node_costs[i] = a[i].cost; 
	}

	sort(a.begin() + 1, a.end(), comparator2); 
	int m;
	cin >> m;
	for (int i = 0; i < m; i++) {
		int u, v, w;
		cin >> u >> v >> w;
		
        
		if(node_costs[u] < w && node_costs[v] < w){
			continue;
		}
		else{
		    adj[u].push_back({v, w});
		    adj[v].push_back({u, w});
		}
	}


    
	long long total = 0;

	for(int i = 1; i <= n; i++){
        int u = a[i].id; 
        
        if(!a[i].has_elec || i==1){ {
            a[i].has_elec = true;
            total += a[i].cost;
            
           
            total += solvewith_prim(n, u, adj, a);
        }
	}
}
	cout << total;

	return 0;
}
