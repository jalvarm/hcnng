// compile with: g++ hcnng.cpp -o hcnng -std=c++11 -fopenmp -O3

#if defined (_MSC_VER)  // Visual studio
    #define thread_local __declspec( thread )
#elif defined (__GCC__) // GCC
    #define thread_local __thread
#endif

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <tuple>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include "common.h"
#include <omp.h>
#include <random>
#include <time.h>
#include <thread>

using namespace std;

int xxx = 0;

tuple<Graph, float> kruskal(vector<Edge> &edges, int N, Matrix<float> &points, int max_mst_degree){
	sort(edges.begin(), edges.end());
	Graph MST(N);
	DisjointSet *disjset = new DisjointSet(N);
	float cost = 0;
	for(Edge &e : edges){
		if(disjset->find(e.v1) != disjset->find(e.v2) && MST[e.v1].size()<max_mst_degree && MST[e.v2].size()<max_mst_degree){
			MST[e.v1].push_back(e);
			MST[e.v2].push_back(Edge(e.v2, e.v1, e.weight));
			disjset->_union(e.v1, e.v2);
			cost += e.weight;
			
		}
	}
	delete disjset;
	return make_tuple(MST, cost);
}

Graph create_exact_mst(Matrix<float> &points, int *idx_points, int left, int right, int max_mst_degree){
	int N = right-left+1;
	if(N==1){
		xxx++;
		printf("%d\n", xxx);
	}
	float cost;
	vector<Edge> full;
	Graph mst;
	full.reserve(N*(N-1));
	for(int i=0; i<N; i++){
		for(int j=0; j<N; j++)
			if(i!=j)
				full.push_back(Edge(i, j, dist_L2(points[idx_points[left+i]], points[idx_points[left+j]], points.cols)));
	}
	tie(mst, cost) = kruskal(full, N, points, max_mst_degree);
	return mst;
}

bool check_in_neighbors(int u, vector<Edge> &neigh){
	for(int i=0; i<neigh.size(); i++)
		if(neigh[i].v2 == u)
			return true;
	return false;
}

void create_clusters(Matrix<float> &points, int *idx_points, int left, int right, Graph &graph, int minsize_cl, vector<omp_lock_t> &locks, int max_mst_degree){
	int num_points = right - left + 1;

	if(num_points<minsize_cl){
		Graph mst = create_exact_mst(points, idx_points, left, right, max_mst_degree);
		for(int i=0; i<num_points; i++){
			for(int j=0; j<mst[i].size(); j++){
				omp_set_lock(&locks[idx_points[left+i]]);
				if(!check_in_neighbors(idx_points[left+mst[i][j].v2], graph[idx_points[left+i]]))
					graph[idx_points[left+i]].push_back(Edge(idx_points[left+i], idx_points[left+mst[i][j].v2], mst[i][j].weight));
				omp_unset_lock(&locks[idx_points[left+i]]);
			}
		}
	}else{
		int x = rand_int(left, right);
		int y = rand_int(left, right);
		while(y==x) y = rand_int(left, right);

		vector<pair<float,int> > dx(num_points);
		vector<pair<float,int> > dy(num_points);
		unordered_set<int> taken;
		for(int i=0; i<num_points; i++){
			dx[i] = make_pair(dist_L2(points[idx_points[x]], points[idx_points[left+i]], points.cols), idx_points[left+i]);
			dy[i] = make_pair(dist_L2(points[idx_points[y]], points[idx_points[left+i]], points.cols), idx_points[left+i]);
		}
		sort(dx.begin(), dx.end());
		sort(dy.begin(), dy.end());
		int i = 0, j = 0, turn = rand_int(0, 1), p = left, q = right;
		while(i<num_points || j<num_points){
			if(turn == 0){
				if(i<num_points){
					if(not_in_set(dx[i].second, taken)){
						idx_points[p] = dx[i].second;
						taken.insert(dx[i].second);
						p++;
						turn = (turn+1)%2;
					}
					i++;
				}else{
					turn = (turn+1)%2;
				}
			}else{
				if(j<num_points){
					if(not_in_set(dy[j].second, taken)){
						idx_points[q] = dy[j].second;
						taken.insert(dy[j].second);
						q--;
						turn = (turn+1)%2;
					}
					j++;
				}else{
					turn = (turn+1)%2;
				}
			}
		}
		
		dx.clear();
		dy.clear();
		taken.clear();
		vector<pair<float,int> >().swap(dx);
		vector<pair<float,int> >().swap(dy);

		create_clusters(points, idx_points, left, p-1, graph, minsize_cl, locks, max_mst_degree);
		create_clusters(points, idx_points, p, right, graph, minsize_cl, locks, max_mst_degree);
	}
}

Graph HCNNG_create_graph(Matrix<float> &points, int Dim, int num_cl, int minsize_cl, int max_mst_degree){
	int N = points.rows;
	Graph G(N);
	vector<omp_lock_t> locks(N);
	for(int i=0; i<N; i++){
		omp_init_lock(&locks[i]);
		G[i].reserve(max_mst_degree*num_cl);
	}
	printf("creating clusters...\n");
	#pragma omp parallel for
	for(int i=0; i<num_cl; i++){
		int * idx_points = new int[N];
		for(int j=0; j<N; j++)
			idx_points[j] = j;
		create_clusters(points, idx_points, 0 , N-1, G, minsize_cl, locks, max_mst_degree);
		printf("end cluster %d\n", i);
		delete[] idx_points;
	}
	printf("sorting...\n");
	sort_edges(G);
	print_stats_graph(G);
	return G;
}

int main(int argc, char** argv){

	// argv[1]: dataset file
	// argv[2]: min size cluster
	// argv[3]: number of clusters
	// argv[4]: graph file

	int N, Dim;

	string file_dataset(argv[1]);
	int minsize_cl = atoi(argv[2]);
	int num_cl = atoi(argv[3]);
	int max_mst_degree = 3;
	string file_graph(argv[4]);
	

	printf("\n***************************\n");
	printf("MIN SZ CL:\t%d\n", minsize_cl);
	printf("NUMBER CL:\t%d\n", num_cl);
	printf("***************************\n\n");

	Matrix<float> points = read_fvecs(file_dataset, N, Dim);
	printf("base read (%d,%d) ...\n", N, Dim);
	Graph nngraph = HCNNG_create_graph(points, Dim, num_cl, minsize_cl, max_mst_degree);
	write_graph(file_graph, nngraph);
	return 0;
}
