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
#include <algorithm>
#include <tuple>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include <omp.h>
#include <random>
#include <time.h>
#include <thread>

using namespace std;


int rand_int(const int & min, const int & max) {
	static thread_local mt19937* generator = nullptr;
	if (!generator) generator = new mt19937(clock() + std::hash<std::thread::id>()(this_thread::get_id()));
	uniform_int_distribution<int> distribution(min, max);
	return distribution(*generator);
}

int rand_int_nosafe(int min, int max){
	return rand()%(max-min+1)+min;
}


float rand_float(const float & min, const float & max) {
	static thread_local mt19937* generator = nullptr;
	if (!generator) generator = new mt19937(clock() + std::hash<std::thread::id>()(this_thread::get_id()));
	uniform_real_distribution<float> distribution(min, max);
	return distribution(*generator);
}

struct MyRandomNumberGenerator
{
	int operator()(int limit) const
	{
		return rand_int(0, limit-1);
	}
};

struct DisjointSet{
	int * parent;
	int * rank;
	DisjointSet(int N){
		parent = new int[N];
		rank = new int[N];		
		for(int i=0; i<N; i++){
			parent[i] = i;
			rank[i] = 0;
		}
	}
	void _union(int x, int y){
		int xroot = parent[x];
		int yroot = parent[y];
		int xrank = rank[x];
		int yrank = rank[y];
		if(xroot == yroot)
			return;
		else if(xrank < yrank)
			parent[xroot] = yroot;
		else{
			parent[yroot] = xroot;
			if(xrank == yrank)
				rank[xroot] = rank[xroot] + 1;
		}
	}
	int find(int x){
		if(parent[x] != x)
			parent[x] = find(parent[x]);
		return parent[x];
	}

	~DisjointSet() { 
		delete[] parent;
		delete[] rank;
	}
};

struct Edge{
	int v1, v2;
	float weight;
	Edge(){
		v1 = -1;
		v2 = -1;
		weight = -1;
	}
	Edge(int _v1, int _v2, float _weight){
		v1 = _v1;
		v2 = _v2;
		weight = _weight;
	}
	bool operator<(const Edge& e) const {
		return weight < e.weight;
	}
	~Edge() { }
};

template <class T>
struct Matrix{
	T *M;
	int rows, cols;
	Matrix(){
		rows = -1;
		cols = -1;
		M = NULL;
	}
	Matrix(int _rows, int _cols){
		rows = _rows;
		cols = _cols;
    	M = new T[((long int)rows)*cols];
	}

	T* operator[](int i){
		return M+(((long int)i)*cols);
	}
	~Matrix(){
		if(M != NULL)
		 	delete[] M;
		M = NULL;
	}
};


float dist_L2(float* x, float* y, int n){
	float d = 0;
	for(int i=0; i<n; i++)
		d += (x[i]-y[i])*(x[i]-y[i]);
	return d;
}

float sim_COS(float* x, float* y, int n){
	float dot = 0;
	float x2 = 0;
	float y2 = 0;
	for(int i=0; i<n; i++){
		dot += (x[i]*y[i]);
		x2 += x[i]*x[i];
		y2 += y[i]*y[i];
	}
	return dot/sqrt(x2*y2);
}

#define Graph vector<vector< Edge > >
#define AdjList vector<vector< int > >
#define not_in_set(_elto,_set) (_set.find(_elto)==_set.end())
#define in_set(_elto,_set) (_set.find(_elto)!=_set.end())
#define not_in_edges(a, b, t, noedges) (noedges.find(make_pair(min(a, b),max(a, b)))==noedges.end() || noedges[make_pair(min(a, b),max(a, b))]&(1LL<<t))

void sort_edges(Graph &G){
	int N = G.size();
	#pragma omp parallel for
	for(int i=0; i<N; i++)
		sort(G[i].begin(), G[i].end());
}

Graph complete_edges(Graph &adjlist){
	int N = adjlist.size();
	set<pair<int,int> > all;
	Graph new_graph(N);
	for(int i=0; i<N; i++){
		for(Edge &e : adjlist[i]){
			if(not_in_set(make_pair(e.v1, e.v2), all) &&  not_in_set(make_pair(e.v2, e.v1), all)){
				new_graph[e.v1].push_back(Edge(e.v1, e.v2, e.weight));
				new_graph[e.v2].push_back(Edge(e.v2, e.v1, e.weight));
				all.insert(make_pair(e.v1, e.v2));
				all.insert(make_pair(e.v2, e.v1));
			}
			
		}
	}
	return new_graph;
}

template<typename SomeType>
float mean_v(vector<SomeType> a){
	float s = 0;
	for(float x: a) s += x;
	return s/a.size();
}

template<typename SomeType>
float sum_v(vector<SomeType> a){
	float s = 0;
	for(float x: a) s += x;
	return s;
}

template<typename SomeType>
float max_v(vector<SomeType> a){
	float mx = a[0];
	for(float x: a) mx = max(mx,x);
	return mx;
}

template<typename SomeType>
float min_v(vector<SomeType> a){
	float mn = a[0];
	for(float x: a) mn = min(mn,x);
	return mn;
}

template<typename SomeType>
float std_v(vector<SomeType> a){
	float m = mean_v(a), s = 0, n = a.size();
	for(float x: a) s += (x-m)*(x-m);
	return sqrt(s/(n-1));
}

template<typename SomeType>
float variance_v(vector<SomeType> a, float mean){
	float o = 0;
	for(float x: a) o += (x-mean)*(x-mean);
	return o/a.size();
}


float get_recall(vector<int> r1, vector<int> r2, int K){
	set<int> a(r1.begin(), r1.begin()+K);
	set<int> b(r2.begin(), r2.begin()+K);
	set<int> result;
	set_intersection(a.begin(), a.end(), b.begin(), b.end(), inserter(result, result.begin()));
	return (float)result.size() / a.size();
}

float get_recall_dist(vector<float> r1, vector<float> r2){
	sort(r1.begin(), r1.end());
	sort(r2.begin(), r2.end());
	int count = 0;
	for(int i=0, j=0; i<r1.size() && j<r2.size(); ){
		if(r1[i]==r2[j]){
			count++;
			i++;
			j++;
		}else if(r1[i]<r2[j])
			i++;
		else
			j++;
	}
	return (float)count / (float)r1.size();
}

long int fsize(FILE *fp){
    long int prev = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long int sz = ftell(fp);
    fseek(fp, prev, SEEK_SET);
    return sz;
}

Matrix<float> read_fvecs(string path_file, int &N, int &Dim){
	FILE * F;
	F = fopen(path_file.c_str(), "rb");
	if(F == NULL){
		printf("Dataset not found\n");
		exit(0);
	}
	int xxx = fread(&Dim, sizeof(int), 1, F);
	long int sizebytes = fsize(F);
	N = sizebytes/(sizeof(float)*(Dim+1));
	rewind(F);
	Matrix<float> data(N, Dim);
	for(int i=0; i<N; i++){
		xxx = fread(&Dim, sizeof(int), 1, F);
		float *vf = new float[Dim];
		xxx = fread(vf, sizeof(float), Dim, F);
		for(int j=0; j<Dim; j++)
			data[i][j] = vf[j];
		delete[] vf;
	}
	fclose(F);
	return data;
}

void write_fvecs(string path_file, Matrix<float> &M){
	FILE * F;
	int N;
	F = fopen(path_file.c_str(), "wb");
	N = M.rows;
	for(int i=0; i<N; i++){
		int Dim = M.cols;
		fwrite(&Dim, sizeof(int), 1, F);
		float* aux = new float[Dim];
		for(int j=0; j<Dim; j++)
			aux[j] = M[i][j];
		fwrite(aux, sizeof(float), Dim, F);
		delete[] aux;
	}
	fclose(F);
}

vector<vector<int> > read_ivecs(string path_file, int &N, int &Dim){
	vector<vector<int> > data;
	FILE * F;
	F = fopen(path_file.c_str(), "rb");
	if(F == NULL){
		printf("Dataset not found\n");
		exit(0);
	}
	int xxx = fread(&Dim, sizeof(int), 1, F);
	long int sizebytes = fsize(F);
	N = sizebytes/(sizeof(int)*(Dim+1));
	rewind(F);
	for(int i=0; i<N; i++){
		xxx = fread(&Dim, sizeof(int), 1, F);
		int *nn = new int[Dim];
		xxx = fread(nn, sizeof(int), Dim, F);
		data.push_back(vector<int>(nn, nn+Dim));
		delete[] nn;
	}
	fclose(F);
	return data;
}

void write_ivecs(string path_file, vector<vector<int> > &GT){
	FILE * F;
	int N;
	F = fopen(path_file.c_str(), "wb");
	N = GT.size();
	for(int i=0; i<N; i++){
		int K = GT[i].size();
		fwrite(&K, sizeof(int), 1, F);
		int* aux = new int[K];
		for(int j=0; j<K; j++)
			aux[j] = GT[i][j];
		fwrite(aux, sizeof(int), K, F);
		delete[] aux;
	}
	fclose(F);
}

vector<int> get_sizeadj(Graph &G){
	vector<int> NE(G.size());
	for(int i=0; i<G.size(); i++)
		NE[i] = G[i].size();
	return NE;
}

vector<int> get_sizeadj(AdjList &G){
	vector<int> NE(G.size());
	for(int i=0; i<G.size(); i++)
		NE[i] = G[i].size();
	return NE;
}

void print_stats_graph(Graph &G){
	vector<int> sizeadj;
	sizeadj = get_sizeadj(G);
	printf("num edges:\t%.0lf\n", sum_v(sizeadj)/2);
	printf("max degree:\t%.0lf\n", max_v(sizeadj));
	printf("min degree:\t%.0lf\n", min_v(sizeadj));
	printf("avg degree:\t%.2lf\n", mean_v(sizeadj));
	printf("std degree:\t%.2lf\n\n", std_v(sizeadj));
}

void print_stats_graph(AdjList &G){
	vector<int> sizeadj;
	sizeadj = get_sizeadj(G);
	printf("num edges:\t%.0lf\n", sum_v(sizeadj)/2);
	printf("max degree:\t%.0lf\n", max_v(sizeadj));
	printf("min degree:\t%.0lf\n", min_v(sizeadj));
	printf("avg degree:\t%.2lf\n", mean_v(sizeadj));
	printf("std degree:\t%.2lf\n\n", std_v(sizeadj));
}

void write_graph(string path_file, Graph &G){
	FILE * F;
	int N;
	F = fopen(path_file.c_str(), "wb");
	N = G.size();
	for(int i=0; i<N; i++){
		int degree = G[i].size();
		fwrite(&degree, sizeof(int), 1, F);
		int* aux = new int[degree];
		for(int j=0; j<degree; j++)
			aux[j] = G[i][j].v2;
		fwrite(aux, sizeof(int), degree, F);
		delete[] aux;
	}
	fclose(F);
}

Graph read_graph(string path_file, Matrix<float> &points, bool verbose){
	FILE * F;
	int N = points.rows;
	F = fopen(path_file.c_str(), "rb");
	if(F == NULL){
		printf("Graph not found\n");
		exit(0);
	}
	Graph G(N);
	int i = 0;
	int degree;
	while(fread(&degree, sizeof(int), 1, F)==1){
		int *nn = new int[degree];
		int xxx = fread(nn, sizeof(int), degree, F);
		G[i].reserve(degree);
		for(int j=0; j<degree; j++){
			if(i!=nn[j]){
				float d = dist_L2(points[i], points[nn[j]], points.cols);
				G[i].push_back(Edge(i, nn[j], d));
			}
		}
		delete[] nn;
		i++;
	}
	fclose(F);
	sort_edges(G);
	if(verbose)
		print_stats_graph(G);
	return G;
}

AdjList read_adjlist(string path_file, Matrix<float> &points, bool verbose){
	FILE * F;
	int N = points.rows;
	F = fopen(path_file.c_str(), "rb");
	if(F == NULL){
		printf("Graph not found\n");
		exit(0);
	}
	AdjList G(N);
	int i = 0;
	int degree;
	while(fread(&degree, sizeof(int), 1, F)==1){
		int *nn = new int[degree];
		int xxx = fread(nn, sizeof(int), degree, F);
		G[i].reserve(degree);
		for(int j=0; j<degree; j++){
			if(i!=nn[j]){
				G[i].push_back(nn[j]);
			}
		}
		delete[] nn;
		i++;
	}
	fclose(F);
	if(verbose)
		print_stats_graph(G);
	return G;
}