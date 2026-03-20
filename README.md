# Practice I – Sorting Large Dataset (C++)

**ST0245 / SI001 — Data Structures and Algorithms**  
School of Applied Sciences and Engineering — EAFIT University  
February 2026

---

**Students**

| Name                  | Program                   |
|-----------------------|---------------------------|
| Sebastián Sánchez     | Systems Engineering       |
| Diego Caballero       | Systems Engineering       |

---

## 1. Project Description

This practice implements and compares three algorithmic strategies to sort a dataset of **100,000 English words** in C++. The objective is to analyze how different algorithms and data structures behave under large volumes of data, measuring both execution time and memory usage.

The three strategies implemented are:

- **QuickSort** — implemented from scratch with 3-way partitioning over a `std::vector<string>`.
- **HeapSort** — implemented from scratch using a Binary Heap structure.
- **AVL Tree** — balanced BST where the sorted result is obtained via inorder traversal.

---

## 2. Project Structure

```
project/
├── main.cpp           # Main source file — all implementations included
├── words_alpha.txt    # Source dictionary (370,000+ English words)
├── dataset.txt        # Generated dataset (100,000 randomized words)
└── README.md          # This file
```

---

## 3. Compilation and Execution

**Requirements**

- C++ compiler with C++11 support or higher (g++, clang++)
- `words_alpha.txt` placed in the same directory as the executable

**Compile**

```bash
g++ -O2 -std=c++11 -o sorting main.cpp
```

**Generate the dataset (first run only)**

Uncomment the following line at the beginning of `main()` to generate `dataset.txt`:

```cpp
generateRandomDataset("words_alpha.txt", "dataset.txt", 100000);
```

**Execute**

```bash
./sorting
```

**Expected output**

```
--- ANALISIS DE RENDIMIENTO ---
Ordenando 100000 palabras...
Tiempo QuickSort: ~130 ms
Tiempo HeapSort: ~180 ms
Tiempo AVL Tree (Insercion + Inorder): ~258 ms

--- ESTIMACION DE MEMORIA APROXIMADA ---
Estructura Base (std::vector para Quick/Heap): ~4800 KB
Estructura Adicional (Nodos de Arbol AVL): ~3900 KB adicionales
```

---

## 4. Dataset Preparation

Since `words_alpha.txt` is already sorted alphabetically, it must be randomized before applying any sorting algorithm to ensure a fair comparison. The function `generateRandomDataset()` accomplishes this using `std::shuffle` with a Mersenne Twister random engine (`mt19937`), then writes 100,000 randomly selected words into `dataset.txt`.

---

## 5. Algorithm Implementations

### 5.1 QuickSort — 3-Way Partitioning

QuickSort is a divide-and-conquer algorithm that recursively partitions the array around a pivot element.

**Why 3-way partitioning?**

The standard 2-partition scheme (`arr[j] < pivot`) degrades to **O(n²)** when the dataset contains many duplicate strings — all elements equal to the pivot remain on one side, producing maximally unbalanced partitions. During testing with `words_alpha.txt`, this caused QuickSort to take over **98,000 ms** on 200,000 words while HeapSort solved the same input in ~180 ms.

The **3-way (Dutch National Flag) partition** divides the array into three zones per call:

```
[ elements < pivot | elements == pivot | elements > pivot ]
```

Elements equal to the pivot are placed in their final position immediately and excluded from both recursive calls, eliminating the degenerate behavior caused by duplicates entirely.

**Pivot selection:**

The pivot is chosen as the **median of three** — comparing the first, middle, and last elements and using the median. This reduces worst-case probability on sorted or nearly-sorted inputs.

**Complexity:**

| Case | Time | Space |
|------|------|-------|
| Average | O(n log n) | O(log n) |
| Worst case (no duplicates) | O(n²) | O(n) |
| With many duplicates | O(n log n) | O(log n) |

---

### 5.2 HeapSort

HeapSort operates in two phases over the same `std::vector`, requiring no additional memory allocation.

**Phase 1 — Build Max-Heap:** Starting from the last internal node (index `n/2 - 1`), `heapify` is called bottom-up to transform the array into a valid max-heap in O(n).

**Phase 2 — Extract elements:** The root (maximum element) is swapped with the last unsorted element. The heap size is reduced by one and `heapify` restores the heap property. This is repeated n-1 times.

**Complexity:**

| Case | Time | Space |
|------|------|-------|
| All cases | O(n log n) | O(1) |

HeapSort is **in-place** and guarantees O(n log n) regardless of input distribution or duplicates.

---

### 5.3 AVL Tree

An AVL Tree is a self-balancing Binary Search Tree that maintains a **balance factor** (difference of subtree heights) in {-1, 0, 1} at every node. After each insertion, the tree is rebalanced through single or double rotations.

**Sorting process:**

1. All 100,000 words are inserted into the AVL tree. Each insertion is O(log n). Duplicate words are silently ignored (BST property).
2. An **inorder traversal** (left → root → right) visits all nodes in ascending alphabetical order and collects the result into a `std::vector<string>`.

**Four rotation cases:**

| Case | Condition | Fix |
|------|-----------|-----|
| Left-Left | `balance > 1` and `key < node->left->key` | Right rotation |
| Right-Right | `balance < -1` and `key > node->right->key` | Left rotation |
| Left-Right | `balance > 1` and `key > node->left->key` | Left + Right rotation |
| Right-Left | `balance < -1` and `key < node->right->key` | Right + Left rotation |

**Complexity:**

| Operation | Time | Space |
|-----------|------|-------|
| Single insertion | O(log n) | — |
| Total insertions (n elements) | O(n log n) | — |
| Inorder traversal | O(n) | — |
| Memory (nodes) | — | O(n) |

---

## 6. Performance Analysis

### 6.1 Execution Time (measured results)

Times measured using `std::chrono::high_resolution_clock`.

| Algorithm | Measured Time | Notes |
|-----------|--------------|-------|
| QuickSort (original 2-way) | **98,582 ms** | O(n²) due to duplicates — bug |
| QuickSort (3-way fixed) | **~130 ms** | O(n log n) — corrected |
| HeapSort | **180 ms** | Unaffected by duplicates |
| AVL Tree (insert + inorder) | **258 ms** | Duplicates silently skipped |

> The original 2-way partition implementation produced **~550× slowdown** compared to HeapSort on the same dataset, caused entirely by duplicate words triggering degenerate partition behavior.

### 6.2 Memory Usage

Memory estimated using `sizeof` and capacity of each structure.

| Algorithm | Base Structure | Additional Memory |
|-----------|---------------|-------------------|
| QuickSort | `std::vector` (~11,648 KB) | O(log n) stack |
| HeapSort | `std::vector` (~11,648 KB) | O(1) — in-place |
| AVL Tree | `std::vector` (~11,648 KB) | ~10,937 KB (AVL nodes) |

Each `AVLNode` stores: one `std::string` (key), two pointers (`left`, `right`), and one `int` (height). On a 64-bit system, `sizeof(AVLNode)` ≈ 40 bytes → `200,001 × 40 ≈ 10,937 KB` of additional memory.

---

## 7. Comparative Analysis

| Criterion | QuickSort (3-way) | HeapSort | AVL Tree |
|-----------|-------------------|----------|----------|
| Measured time | Fastest (~130 ms) | Moderate (180 ms) | Slowest (258 ms) |
| Worst-case guarantee | No (O(n²) possible) | Yes — always O(n log n) | Yes — always O(n log n) |
| Handles duplicates | Yes (3-way) | Yes | Yes (skips them) |
| Additional memory | O(log n) stack | O(1) in-place | O(n) nodes |
| Post-sort search | Not supported | Not supported | O(log n) |

**Why does QuickSort outperform HeapSort in practice despite identical average complexity?**

Both algorithms have O(n log n) average complexity. QuickSort benefits from better **cache locality**: it accesses elements sequentially within a contiguous block of memory (the vector). HeapSort accesses parent and child indices that are far apart in memory (`i` vs `2i+1`, `2i+2`), producing frequent **cache misses** — especially for large datasets. This explains the ~38% performance gap observed (130 ms vs 180 ms).

**Why is AVL Tree slower for pure sorting despite O(n log n) complexity?**

The AVL tree requires `100,000 individual heap allocations` (one `new AVLNode` per word). Dynamic memory allocation carries non-trivial overhead. Furthermore, pointer-based tree traversal has poor cache behavior compared to array indexing. The AVL approach is not designed as a primary sorting algorithm — its advantage lies in supporting **O(log n) search and insertion** after the dataset is loaded.

**Why did the original QuickSort implementation fail?**

The original 2-way partition used a strict less-than comparison (`arr[j] < pivot`). When the dataset contains many duplicate words, all elements equal to the pivot remain on the right partition — producing splits such as `[0 elements | pivot | n-1 elements]` repeatedly. This degrades the algorithm to **O(n²)**, which is consistent with the observed 98,582 ms runtime.

---

## 8. Conclusions

- **QuickSort** with 3-way partitioning is the most practical choice for sorting large string datasets. The median-of-three pivot and Dutch National Flag partition make it robust against both nearly-sorted data and datasets with many duplicates.
- **HeapSort** is the safest choice when a guaranteed O(n log n) worst case is required and no extra memory can be allocated. It is immune to input distribution issues.
- **AVL Tree** is the least efficient option for pure sorting due to allocation overhead and poor cache behavior. However, it is the only structure among the three that supports **O(log n) search** after the dataset is loaded, making it appropriate when the dataset will be queried repeatedly.

For the specific task of sorting 100,000 words once with no further lookups, **QuickSort with 3-way partitioning is the recommended approach**.

---

## 9. References

- Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). *Introduction to Algorithms* (3rd ed.). MIT Press. Chapter 7 (Quicksort), Chapter 6 (Heapsort).
- Sedgewick, R., & Wayne, K. (2011). *Algorithms* (4th ed.). Addison-Wesley. Section 2.3 (Quicksort — 3-way partitioning).
- cppreference.com — `std::chrono`, `std::vector`, `std::shuffle` documentation.

---

> **Note:** To reproduce the results from Section 6.1, compile with `-O2` and ensure `dataset.txt` contains exactly 100,000 words generated by `generateRandomDataset()`.
