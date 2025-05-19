//Shanti Gharib
//L10

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>

//Load from file
//Add orders to a temp file
//process orders from temp file into priority queue
struct SalesItem;
enum class RushStatusTier;
class Inventory;
class SalesOrder;
class PriorityQueue;



struct SalesItem {
    std::string id, name;
    int quantity{0};
    double price{0.0};
};


class Inventory {
private:
    static constexpr const int MAX_SIZE{5};
    int currSize{0};
    SalesItem items[MAX_SIZE];
    void DisplayItem(int idx) const;
public:
    Inventory();
    void LoadFromFile();
    void DisplayDetails() const;
};


enum class RushStatusTier {
    EXTREME = 1,
    EXPEDITE,
    STANDARD,

    SIZE
};

class SalesOrder {
private:
    SalesItem item;
    long long orderNumber{-1};
    RushStatusTier rushStatus{RushStatusTier::STANDARD};

public:
    SalesOrder();
    bool operator<(const SalesOrder& rhs) const;
    bool operator>(const SalesOrder& rhs) const;
    bool operator<=(const SalesOrder& rhs) const;
    bool operator>=(const SalesOrder& rhs) const;
    bool operator==(const SalesOrder& rhs) const;
    bool operator!=(const SalesOrder& rhs) const;
    
    friend PriorityQueue;
};

class PriorityQueue {
private:
    static constexpr const int GROWTH_FACTOR{2};
    SalesOrder* pHeap{nullptr};
    size_t currSize{0}, maxSize{0};
    bool Heapify(int idx);
    inline bool AvailableSpace() const;
    bool AllocateMoreMemory();
    void DeallocateMemory(SalesOrder* &pContainer);

public:
    void ProcessOrders();
    void DisplayOrders() const;
    PriorityQueue();
    ~PriorityQueue();

    bool AddOrderToQueue(SalesOrder &source);
};


void HandleDefaultMenuCase();
void DisplayMenu();

const std::string INPUT_FILE{"in_inventory.txt"}, BACK_ORDER_FILE{"out_backOrder.txt"};

const double rushStatusMarkup[static_cast<int>(RushStatusTier::SIZE)]{0, 0.68, 0.45, 0.38};
const std::string rushStatusStrs[static_cast<int>(RushStatusTier::SIZE)]{"", "Extreme", "Expedite", "Standard"};

int main() {

    enum { INVENTORY_DETAILS = 1, DISPLAY_ORDERS, PROCESS_ORDERS, QUIT };
    int option{0};

    Inventory salesInventory;
    PriorityQueue salesOrders;

    while (option != QUIT) {

        DisplayMenu();
        std::cin >> option;

        switch (option) {
        
            case INVENTORY_DETAILS: {
                salesInventory.DisplayDetails();
                break;
            }
            
            case DISPLAY_ORDERS: {
                salesOrders.DisplayOrders();
                break;
            }
            
            case PROCESS_ORDERS: {
                salesOrders.ProcessOrders();
                break;
            }
            
            case QUIT: {
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



Inventory::Inventory() {}

void Inventory::LoadFromFile() {
    std::ifstream inputData(INPUT_FILE);
    if (inputData.is_open()) {
        std::string data;
        while (std::getline(inputData, data)) {
            std::istringstream inData(data);
            SalesItem temp;
            inData >> temp.id >> temp.name >> temp.price >> temp.quantity;
            ++currSize;
        }
        inputData.close();
    }
    else {
        std::cout << "Could not check inventory records file. Closing Program...\n";
        exit(EXIT_FAILURE);
    }
}



SalesOrder::SalesOrder() {}

bool SalesOrder::operator< (const SalesOrder& rhs) const {
    return (orderNumber < rhs.orderNumber) && (static_cast<int>(rushStatus) < static_cast<int>(rhs.rushStatus));
}

bool SalesOrder::operator> (const SalesOrder& rhs) const {
    return (static_cast<int>(rushStatus) > static_cast<int>(rhs.rushStatus)) || (orderNumber > rhs.orderNumber && static_cast<int>(rushStatus) == static_cast<int>(rhs.rushStatus));
}

bool SalesOrder::operator<=(const SalesOrder& rhs) const {
    return !operator>(rhs);
}

bool SalesOrder::operator>=(const SalesOrder& rhs) const {
    return !operator<(rhs);
}

bool SalesOrder::operator==(const SalesOrder& rhs) const {
    return (orderNumber == rhs.orderNumber) && (static_cast<int>(rushStatus) == static_cast<int>(rhs.rushStatus));
}

bool SalesOrder::operator!=(const SalesOrder& rhs) const {
    return !operator==(rhs);
}






PriorityQueue::PriorityQueue() {}

PriorityQueue::~PriorityQueue() {
    currSize = maxSize = 0;
    DeallocateMemory(pHeap);
}

bool PriorityQueue::AddOrderToQueue(SalesOrder& source) {
    const bool state = AvailableSpace() || AllocateMoreMemory();
    if (state) {
        pHeap[currSize] = source;
        int parent = (currSize - 1) / 2;
        while (parent >= 0 && Heapify(parent)) {
            parent = (parent - 1) / 2;
        }
        ++currSize;
    }
    return state;
}

bool PriorityQueue::Heapify(int idx) {

    const int leftChild = idx * 2 + 1;
    const int rightChild = leftChild + 1;
    bool state = false;

    if (leftChild < currSize) {
        const int mxChild = (rightChild < currSize && pHeap[rightChild] >= pHeap[leftChild]) ? rightChild : leftChild;
        
        if (pHeap[idx] < pHeap[mxChild]) {
            const SalesOrder temp = pHeap[mxChild];
            pHeap[mxChild] = pHeap[idx];
            pHeap[idx] = temp;
            state = true;
            Heapify(mxChild);
        }
    }
    return state;
}

bool PriorityQueue::AllocateMoreMemory() {
    SalesOrder* pTemp = nullptr;

    try {
        const int NEW_SIZE = std::max(1ull, currSize * GROWTH_FACTOR);
        pTemp = new SalesOrder[NEW_SIZE];
        for (size_t i = 0; i < currSize; ++i) {
            pTemp[i] = pHeap[i];
        }
        DeallocateMemory(pHeap);
        pHeap = pTemp;
        pTemp = nullptr;
    }
    catch (std::bad_alloc& err) {
        std::cout << "Not enough Continguous Memory Space to add more elements\n";
        DeallocateMemory(pTemp);
    }
}

void PriorityQueue::DeallocateMemory(SalesOrder* &pContainer) {
    if (pContainer != nullptr) {
        delete[] pContainer;
        pContainer = nullptr;
    }
}

inline bool PriorityQueue::AvailableSpace() const {
    return currSize < maxSize;
}









void HandleDefaultMenuCase() {
    std::cout << "Invalid Menu option selected\n";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.clear();

}

void DisplayMenu() {

    std::cout << "\nSelect an option from the following"

        "\n1. Show Current Inventory Details"

        "\n2. Display all Placed Orders"

        "\n3. Process Orders"

        "\n4. Quit\n\n";

}

void PriorityQueue::ProcessOrders() {

}

void Inventory::DisplayDetails() const {
    std::cout << std::left << std::setw(15) << "Item Id" << "|" << std::setw(15) << "Item Name" << "|" << std::setw(15) << "Quantity" << "|" << std::setw(15) << "Price" << "|" << std::right << "\n";
    std::cout << std::setfill("-") << std::setw(63) << "" << std::setfill(' ') << "\n";
    for (int i = 0; i < MAX_SIZE; ++i) {
        items[i].Display();
    }
}

void Inventory::DisplayItem(int idx) const {
    if (idx >= 0 && idx < MAX_SIZE) {
        std::cout << std::setw(15) << items[idx].id << std::left << std::setw(15) << items[idx].name << std::setw(15) << std::right << items[idx].quantity
    }
}

void PriorityQueue::DisplayOrders() const {

}
