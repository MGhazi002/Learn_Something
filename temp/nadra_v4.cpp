#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

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

class HashTableLinearProbing {
private:
    string table[HASH_TABLE_SIZE];
    bool isDeleted[HASH_TABLE_SIZE]; // To handle deletion

public:
    HashTableLinearProbing() {
        fill_n(table, HASH_TABLE_SIZE, "");
        fill_n(isDeleted, HASH_TABLE_SIZE, false);
    }

    void insertId(string id) {
        int key = getKeyFromId(id);
        while (table[key] != "" && table[key] != id) {
            key = (key + 1) % HASH_TABLE_SIZE;
        }
        table[key] = id;
        isDeleted[key] = false;
    }

    // Modified searchId method to accept 'comparisons' parameter
    bool searchId(string id, int& comparisons) {
        int key = getKeyFromId(id);
        comparisons = 0; // Initialize comparisons to 0 for each search
        while (table[key] != "") {
            comparisons++; // Increment comparisons on each check
            if (table[key] == id && !isDeleted[key]) {
                return true;
            }
            key = (key + 1) % HASH_TABLE_SIZE;
        }
        return false;
    }

    void deleteId(string id) {
        int key = getKeyFromId(id);
        while (table[key] != "") {
            if (table[key] == id && !isDeleted[key]) {
                isDeleted[key] = true;
                return;
            }
            key = (key + 1) % HASH_TABLE_SIZE;
        }
    }
};


struct ListNode {
    string id;
    ListNode* next;
    ListNode(string id) : id(id), next(nullptr) {}
};

class HashTableLinkedList {
private:
    ListNode* table[HASH_TABLE_SIZE];

public:
    HashTableLinkedList() {
        fill_n(table, HASH_TABLE_SIZE, nullptr);
    }

    void insertId(string id) {
        int key = getKeyFromId(id);
        ListNode* newNode = new ListNode(id);
        newNode->next = table[key];
        table[key] = newNode;
    }

bool searchId(string id, int& comparisons) {
        int key = getKeyFromId(id);
        ListNode* node = table[key];
        comparisons = 0; // Initialize the number of comparisons

        while (node != nullptr) {
            comparisons++; // Increment comparisons on each check
            if (node->id == id) {
                return true;
            }
            node = node->next;
        }
        return false;
    }

    void deleteId(string id) {
        int key = getKeyFromId(id);
        ListNode *node = table[key], *prev = nullptr;
        while (node != nullptr && node->id != id) {
            prev = node;
            node = node->next;
        }
        if (node == nullptr) return;
        if (prev == nullptr) {
            table[key] = node->next;
        } else {
            prev->next = node->next;
        }
        delete node;
    }
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

    // Hash function to convert a key into an index
    int hashFunction(int key) {
        return key % HASH_TABLE_SIZE;
    }

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
        comparisons = 0; // Reset comparisons
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
    string part1 = id.substr(0, 5); // First 5 characters
    string part2 = id.substr(6, 7); // Next 7 characters

    int num1 = stoi(part1);
    int num2 = stoi(part2);

    int key = num1 * 2654435761u ^ num2; // Example of a more complex operation

    return key % HASH_TABLE_SIZE; // Ensure the key fits within table size
}


// Random ID Generator
string generateRandomID() {
    stringstream ss;
    ss << setw(5) << setfill('0') << rand() % 100000 << "-" 
       << setw(7) << setfill('0') << rand() % 10000000 << "-" 
       << rand() % 10;
    return ss.str();
}

int main() {
    // Create instances of each hash table type
    HashTableBST hashTableBST;
    HashTableLinearProbing hashTableLinearProbing;
    HashTableLinkedList hashTableLinkedList;

    const int NUM_INSERTIONS = 1000;
    const int NUM_SEARCHES = 100;
    srand(time(nullptr)); // Initialize random number generator

    // Inserting random IDs into each hash table type
    for (int i = 0; i < NUM_INSERTIONS; ++i) {
        string id = generateRandomID();
        hashTableBST.insertId(id);
        hashTableLinearProbing.insertId(id);
        hashTableLinkedList.insertId(id);
    }

    // Searching and collecting statistics for each hash table type
    int totalComparisonsBST = 0, totalComparisonsLinearProbing = 0, totalComparisonsLinkedList = 0;
    int comparisons = 0;

    for (int i = 0; i < NUM_SEARCHES; ++i) {
        string id = generateRandomID();

        // For BST
        comparisons = 0;
        hashTableBST.searchId(id, comparisons);
        totalComparisonsBST += comparisons;

        // For Linear Probing
        comparisons = 0;
        hashTableLinearProbing.searchId(id, comparisons);
        totalComparisonsLinearProbing += comparisons;

        // For Linked List
        comparisons = 0;
        hashTableLinkedList.searchId(id, comparisons);
        totalComparisonsLinkedList += comparisons;
    }

    // Displaying results for each hash table type
    cout << "BST-Based Chaining:" << endl;
    cout << "Total Collisions during Insertion: " << hashTableBST.getTotalCollisions() << endl;
    cout << "Average Comparisons per Search: " 
         << static_cast<double>(totalComparisonsBST) / NUM_SEARCHES << endl << endl;

    cout << "Linear Probing:" << endl;
    cout << "Total Collisions during Insertion: " << /* Add method to get total collisions for Linear Probing */ endl;
    cout << "Average Comparisons per Search: " 
         << static_cast<double>(totalComparisonsLinearProbing) / NUM_SEARCHES << endl << endl;

    cout << "Linked-List Chaining:" << endl;
    cout << "Total Collisions during Insertion: " << /* Add method to get total collisions for Linked List */ endl;
    cout << "Average Comparisons per Search: " 
         << static_cast<double>(totalComparisonsLinkedList) / NUM_SEARCHES << endl;

    return 0;
}
