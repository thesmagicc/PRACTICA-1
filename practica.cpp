#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;

// PREPARACIÓN DEL DATASET
void generateRandomDataset(const string& inputFile, const string& outputFile, int count = 100000) {
    ifstream inFile(inputFile);
    vector<string> words;
    string word;
    
    if (!inFile) {
        cerr << "Error: No se pudo abrir " << inputFile << ". Asegurate de tener words_alpha.txt en la misma carpeta." << endl;
        return;
    }
    
    while (inFile >> word) {
        words.push_back(word);
    }
    inFile.close();

    // Mezclar aleatoriamente el dataset original que ya venía ordenado
    random_device rd;
    mt19937 g(rd());
    shuffle(words.begin(), words.end(), g);

    ofstream outFile(outputFile);
    int limit = min(count, (int)words.size());
    for (int i = 0; i < limit; i++) {
        outFile << words[i] << "\n";
    }
    outFile.close();
    cout << "Dataset de " << limit << " palabras aleatorias generado en " << outputFile << endl;
}

vector<string> loadDataset(const string& filename) {
    vector<string> data;
    ifstream inFile(filename);
    string word;
    while (inFile >> word) {
        data.push_back(word);
    }
    return data;
}

// 1. QUICKSORT (A prueba de Stack Overflow)
int partition(vector<string>& arr, int low, int high) {
    int mid = low + (high - low) / 2;
    swap(arr[mid], arr[high]); // Pivote en la mitad
    
    string pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) { 
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void quickSort(vector<string>& arr, int low, int high) {
    while (low < high) {
        int pi = partition(arr, low, high);
        
        if (pi - low < high - pi) {
            quickSort(arr, low, pi - 1);
            low = pi + 1;
        } else {
            quickSort(arr, pi + 1, high);
            high = pi - 1;
        }
    }
}

// 2. HEAPSORT 
void heapify(vector<string>& arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;

    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(vector<string>& arr) {
    int n = arr.size();
    // Construir el Binary Heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Extraer elementos del heap para ordenar
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

// 3. ÁRBOL AVL
struct AVLNode {
    string key;
    AVLNode* left;
    AVLNode* right;
    int height;
};

int height(AVLNode* N) {
    if (N == NULL) return 0;
    return N->height;
}

AVLNode* newNode(string key) {
    AVLNode* node = new AVLNode();
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

AVLNode* rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}

AVLNode* leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}

int getBalance(AVLNode* N) {
    if (N == NULL) return 0;
    return height(N->left) - height(N->right);
}

AVLNode* insertAVL(AVLNode* node, string key) {
    if (node == NULL) return newNode(key);

    if (key < node->key)
        node->left = insertAVL(node->left, key);
    else if (key > node->key)
        node->right = insertAVL(node->right, key);
    else return node; // No se permiten duplicados

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);

    // Casos de rotación
    if (balance > 1 && key < node->left->key) return rightRotate(node);
    if (balance < -1 && key > node->right->key) return leftRotate(node);
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

// Recorrido Inorder
void inorderTraversal(AVLNode* root, vector<string>& result) {
    if (root != NULL) {
        inorderTraversal(root->left, result);
        result.push_back(root->key);
        inorderTraversal(root->right, result);
    }
}

// FUNCIÓN PRINCIPAL
int main() {
    vector<string> originalData = loadDataset("dataset.txt");
    if (originalData.empty()) {
        cerr << "El dataset esta vacio o no existe. Revisa que dataset.txt este en la carpeta." << endl;
        return 1;
    }

    vector<string> dataQuick = originalData;
    vector<string> dataHeap = originalData;
    vector<string> dataAVL;

    cout << "--- ANALISIS DE RENDIMIENTO ---" << endl;
    cout << "Ordenando " << originalData.size() << " palabras..." << endl;

    auto start = high_resolution_clock::now();
    quickSort(dataQuick, 0, dataQuick.size() - 1);
    auto stop = high_resolution_clock::now();
    auto durationQuick = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo QuickSort: " << durationQuick.count() << " ms" << endl;

    start = high_resolution_clock::now();
    heapSort(dataHeap);
    stop = high_resolution_clock::now();
    auto durationHeap = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo HeapSort: " << durationHeap.count() << " ms" << endl;

    start = high_resolution_clock::now();
    AVLNode* root = NULL;
    for (const string& word : originalData) {
        root = insertAVL(root, word);
    }
    inorderTraversal(root, dataAVL); 
    stop = high_resolution_clock::now();
    auto durationAVL = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo AVL Tree (Insercion + Inorder): " << durationAVL.count() << " ms" << endl;

    // --- Estimación de Memoria ---
    cout << "\n--- ESTIMACION DE MEMORIA APROXIMADA ---" << endl;
    
    size_t vectorMem = originalData.capacity() * sizeof(string);
    size_t stringsMem = 0;
    for(const string& s : originalData) stringsMem += s.capacity();
    
    cout << "Estructura Base (std::vector para Quick/Heap): ~" << (vectorMem + stringsMem) / 1024 << " KB" << endl;
 
    size_t avlNodeSize = sizeof(AVLNode);
    size_t avlTotalMem = originalData.size() * avlNodeSize;
    cout << "Estructura Adicional (Nodos de Arbol AVL): ~" << avlTotalMem / 1024 << " KB adicionales" << endl;

    return 0;
}