# HCNNG
This is the implementation of the Hierarquical Clustering-based Nearest Neighbors Graphs. There are two principal files:

- **hcnng.cpp**: this file creates the graph-based index.
- **search.cpp**: this file perform the search over the index created by **hcnng.cpp**.

The input files format are:

- Files containing the dataset feature vectors (for index construction) need to be in the format of binary ".fvecs" files.
- Files containing the test set feature vectors (the queries) need to be in the format of binary ".fvecs" files.
- Files containing the ground truth of queries need to be in the format of binary ".ivecs" files.

For details of this formats, please visit http://corpus-texmex.irisa.fr .

## Reference

Reference to cite when you use HCNNG in a research paper:

```
@article{VARGASMUNOZ2019106970,
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
