//Shanti Gharib
//L10

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>

enum class MenuOptions {
    INVENTORY_INFO = 1, 
    ALL_ORDERS, 
    QUIT,
}

class PriorityQueue {
private:
    static const int GROWTH_FACTOR{2};
    int* pArr{nullptr};
    size_t arrSize{0}, maxSize{0};
    void Heapify();
    bool Swap(int parent, int mxChild);
    inline int GetGreatestChildIdx(int idxL, int idxR);
    inline bool IsSizeLimited() const;
    bool AllocateMemory(int* &pContainer);
    void DeallocateMemory(int* &pContainer);
  
public:
    
    PriorityQueue();
    ~PriorityQueue();
  
  
  
    
};



void HandleDefaultMenuCase();
void DisplayMenu();


int main() {
    
    int option{0};
    while (option != static_cast<int>(MenuOptions::QUIT)) {
        
        DisplayMenu();
        std::cin >> option;
        
        switch (option) {
            case static_cast<int>(MenuOptions::INVENTORY_INFO): {
                //TODO
                break;
            }
            case static_cast<int>(MenuOptions::ALL_ORDERS): {
                //TODO
                break;
            }
            case static_cast<int>(MenuOptions::QUIT) {
                std::cout << "\nProgram now closed...\n";
                break;
            }
            default: {
                HandleDefaultMenuCase();
                break;
            }
        }
        
    }
    
}


void PriorityQueue::Heapify(int idx) {
    
    const int leftChild = idx * 2;
    const int rightChild = leftChild + 1;
    
    if (rightChild < arrSize) {
        const int mxChild = GetGreatestChildIdx(leftChild, rightChild);
        if (Swap(idx, mxChild)) {
            Heapify(mxChild);
        }

    }
    else if (leftChild < arrSize && Swap(idx, leftChild)) {
        Heapify(leftChild);
    }
}


inline int PriorityQueue::GetGreatestChildIdx(int idxL, int idxR) {
    
    return (pArr[idxL] >= pArr[idxR]) ? idxL : idxR;
    
}


bool PriorityQueue::Swap(int parent, int mxChild) {
    
    const bool result = pArr[parent] < pArr[mxChild]; 
    
    if (result) {
            
        const int temp = pArr[mxChild];
        pArr[mxChild] = pArr[parent];
        pArr[parent] = temp;
        
    }
    
    return result;
    
}

bool PriorityQueue::AllocateMemory() {
    int* pTemp = nullptr;
    
    try {
        const int NEW_SIZE = max(1, currSize * 2);
        pTemp = new int[NEW_SIZE];
        for (int i = 0; i < currSize; ++i) {
            pTemp[i] = pArr[i];
        }
        DeallocateMemory(pArr);
        pArr = pTemp;
    }
    catch (std::bad_alloc &err) {
        std::cout << "Not enough Continguous Memory Space to add more elements\n";
        DeallocateMemory(pTemp);
    }
}
void PriorityQueue::DeallocateMemory(int* &pContainer) {
    if (pContainer != nullptr) {
        delete[] pContainer;
        pContainer = nullptr;
    }
} 

inline bool PriorityQueue::IsSizeLimited() const {
    return currSize < maxSize;
}









void HandleDefaultMenuCase() {
    std::cout << "Invalid Menu option selected\n";
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.clear();
    
}

void DisplayMenu() {
    
    std::cout << "\nSelect an option from the following"
        
        "\n1. Show Current Inventory Info"
        
        "\n2. Display all Processed & Unprocessed Orders"
        
        "\n3. Quit\n\n";
        
}

