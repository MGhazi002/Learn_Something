#include <iostream>
#include <string>

using namespace std;

// Define the size of the Hash Table
const int HASH_TABLE_SIZE = 1000;

// BST Node
struct BSTNode {
    string id; // ID Card Number
    BSTNode* left;
    BSTNode* right;
    BSTNode(string id) : id(id), left(nullptr), right(nullptr) {}
};

// BST class for handling BST operations
class BST {
public:
    BSTNode* root;

    BST() : root(nullptr) {}

    // Insert a new ID
    void insert(BSTNode*& node, string id) {
        if (node == nullptr) {
            node = new BSTNode(id);
            return;
        }

        if (id < node->id) {
            insert(node->left, id);
        } else if (id > node->id) {
            insert(node->right, id);
        }
    }

    // Search for an ID
bool search(BSTNode* node, string id, int& comparisons) {
        comparisons++;

        if (node == nullptr) {
            return false;
        }

        if (id == node->id) {
            return true;
        } else if (id < node->id) {
            return search(node->left, id, comparisons);
        } else {
            return search(node->right, id, comparisons);
        }
    }

    // Delete an ID
    void deleteId(BSTNode*& node, string id) {
        if (node == nullptr) return;

        if (id < node->id) {
            deleteId(node->left, id);
        } else if (id > node->id) {
            deleteId(node->right, id);
        } else {
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                node = nullptr;
            } else if (node->left != nullptr && node->right == nullptr) {
                BSTNode* temp = node;
                node = node->left;
                delete temp;
            } else if (node->left == nullptr && node->right != nullptr) {
                BSTNode* temp = node;
                node = node->right;
                delete temp;
            } else {
                BSTNode* temp = findMin(node->right);
                node->id = temp->id;
                deleteId(node->right, temp->id);
            }
        }
    }

    // Find the minimum node in the BST
    BSTNode* findMin(BSTNode* node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }
};

// Hash Table using BST Chaining
class HashTableBST {
private:
    BST table[HASH_TABLE_SIZE];
    int totalCollisions; // To track total collisions during insertion
    int totalComparisons; // To track total comparisons during search

public:
    HashTableBST() : totalCollisions(0), totalComparisons(0) {}

    // ... Existing methods ...

    // Modified Insert method to track collisions
    void insertId(string id) {
        int key = getKeyFromId(id);
        int index = hashFunction(key);

        if (table[index].root != nullptr) {
            totalCollisions++; // Increment collision counter
        }

        table[index].insert(table[index].root, id);
    }

    // Modified Search method to track comparisons
    bool searchId(string id, int& comparisons) {
        int key = getKeyFromId(id);
        int index = hashFunction(key);
        return table[index].search(table[index].root, id, comparisons);
    }

    // Getters for collision and comparison data
    int getTotalCollisions() const {
        return totalCollisions;
    }

    int getTotalComparisons() const {
        return totalComparisons;
    }
};




// Function to extract key from ID
int getKeyFromId(string id) {
    // Assuming ID format is "aaaaa-fffffff-g"

    // Extract the parts of the ID
    string part1 = id.substr(0, 5); // First 5 characters
    string part2 = id.substr(6, 7); // Next 7 characters

    // Convert these parts to integers
    int num1 = stoi(part1);
    int num2 = stoi(part2);

    // Combine these numbers to form a key
    // This can be a simple addition, multiplication, or any other operation
    // that can mix the bits of num1 and num2 to give a good distribution
    int key = num1 + num2; // This is a basic example

    // Optionally, you can apply further hashing like modulo with a prime number
    // to ensure the key is within a specific range

    return key;
}


int main() {
    HashTableBST hashTable;
    const int NUM_INSERTIONS = 1000;
    const int NUM_SEARCHES = 100;
    
    srand(time(nullptr)); // Initialize random number generator

    // Inserting random IDs
    for (int i = 0; i < NUM_INSERTIONS; ++i) {
        hashTable.insertId(generateRandomID());
    }

    int totalComparisons = 0;
    int comparisons = 0;

    // Searching for random IDs
    for (int i = 0; i < NUM_SEARCHES; ++i) {
        hashTable.searchId(generateRandomID(), comparisons);
        totalComparisons += comparisons;
    }

    // Displaying results
    cout << "Total Collisions during Insertion: " << hashTable.getTotalCollisions() << endl;
    cout << "Average Collisions per Insertion: " << static_cast<double>(hashTable.getTotalCollisions()) / NUM_INSERTIONS << endl;
    cout << "Total Comparisons during Search: " << totalComparisons << endl;
    cout << "Average Comparisons per Search: " << static_cast<double>(totalComparisons) / NUM_SEARCHES << endl;

    return 0;
}
