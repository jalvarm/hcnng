# HCNNG
This is the implementation of the Hierarquical Clustering-based Nearest Neighbors Graphs. There are two principal files:

- **hcnng.cpp**: this file creates the graph-based index. The compiled executable will receive the following parameters:
  - File containing the dataset feature vectors for creation of the index.
  - Desired size of clusters.
  - Number of clustering executions.
  - File where the graph will be saved.
- **search.cpp**: this file perform the search over the index created by **hcnng.cpp**. The compiled executable will receive the following parameters:
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

This implementation still not contain the *Guided* search described in the paper, neither the Multi-KDtree-based initial vertex selection, but it will included soon. Also, I plan to incorporate a wrapper for python.

## Requirements

Each file contains the command line for compilation. There is no other fancy library required other than the compiler of C++ version 11 or newer versions. This implementation support multi-thread execution based on OpenMP, so if you want to accelerate the construction of the graphs, this is the only library you will need to install.

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
