# HCNNG
This is the implementation of the Hierarquical Clustering-based Nearest Neighbors Graphs. There are two principal files:

- **hcnng.cpp**: this file creates the graph-based index. The compiled executable will receive the following parameters (in the same order):
  - File containing the dataset feature vectors for creation of the index.
  - Desired size of clusters.
  - Number of clustering executions.
  - File where the graph will be saved.
- **search.cpp**: this file perform the search over the index created by **hcnng.cpp**. The compiled executable will receive the following parameters (in the same order):
  - File containing the dataset feature vectors.
  - File containing the test set of queries.
  - File containing the ground truth of queries.
  - File containing the graph-based index created by **hcnng.cpp**.
  - The number of neighbors to search in each query.
  - The maximum number of vectors to explore in the dataset.

The input files format for these code source files are:

- Files containing the dataset feature vectors (for index construction) need to be in the format of binary ".fvecs" files.
- Files containing the test set feature vectors (the queries) need to be in the format of binary ".fvecs" files.
- Files containing the ground truth of queries need to be in the format of binary ".ivecs" files.
- Files containing the graph-based index will be saved in the format of binary ".ivecs" files.

For details of those formats, please visit http://corpus-texmex.irisa.fr .

This implementation still not contain the *Guided* search described in the paper, neither the Multi-KDtree-based initial vertex selection, but they will be included soon. Also, I plan to incorporate a wrapper for python.

## Requirements

Each file contains the command line for compilation. There is no other fancy library required other than the compiler of C++ version 11 or newer versions. This implementation support multi-thread execution based on OpenMP, so if you want to accelerate the construction of the graphs, this is the only library you will need to install.

## Use

Here is an example of the command needed for execution of the source code. For this purpose, please download the *ANN_SIFT1M* dataset from http://corpus-texmex.irisa.fr and unzip the same directory of the source code.

For index construction run:
```
$ ./hcnng sift_base.fvecs 1000 20 hcnng_sift.ivecs
```
I recomend to use this parameters for index contruction on million-size datasets, that also were used in the paper, 1000 as expected size of clusters and 20 clustering executions.

For evaluation on a test set of queries run:
```
$ ./search sift_base.fvecs sift_query.fvecs sift_groundtruth.ivecs hcnng_sift.ivecs 1 -1
```
this execution should be shown something like:
```
base read (1000000,128) ...
queries read (10000,128)...
groundtruth read...
num edges:	11769062
max degree:	52
min degree:	2
avg degree:	23.54
std degree:	6.12

Recall@1(100):	1.20
Recall@1(125):	2.93
Recall@1(158):	7.44
Recall@1(199):	15.59
Recall@1(251):	31.03
Recall@1(316):	47.94
Recall@1(398):	65.61
Recall@1(501):	77.46
Recall@1(630):	86.05
Recall@1(794):	91.15
Recall@1(999):	94.43
Recall@1(1258):	96.42
Recall@1(1584):	97.85
Recall@1(1995):	98.70
Recall@1(2511):	99.23
Recall@1(3162):	99.59
Recall@1(3981):	99.73
Recall@1(5011):	99.85
Recall@1(6309):	99.97
Recall@1(7943):	99.98
Recall@1(9999):	99.99
```
by setting the maximum-number-of-calculation parameter to -1, the code will print the recall obtained for the points showed in the results Figures of the paper.

## Reference

Reference to cite when you use HCNNG in a research paper:

```
@article{HCNNG,
title = "Hierarchical Clustering-Based Graphs for Large Scale Approximate Nearest Neighbor Search",
author = "Javier Vargas Muñoz and Marcos A. Gonçalves and Zanoni Dias and Ricardo da S. Torres"
journal = "Pattern Recognition",
volume = "96",
pages = "106970",
year = "2019",
issn = "0031-3203",
doi = "https://doi.org/10.1016/j.patcog.2019.106970",
}
```
