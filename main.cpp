//Shanti Gharib
//L10

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>


struct SalesItem;
class Inventory;
enum class RushStatusTier;
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
public:
    Inventory();
    Inventory(const Inventory& source);
    
    ~Inventory();

    void operator=(const Inventory& source);
    void LoadFromFile();
    int GetIdIdx(const std::string& id) const;
    void DisplayDetails() const;

    friend SalesOrder;
    friend PriorityQueue;
};


enum class RushStatusTier {
    EXTREME = 1,
    EXPEDITE,
    STANDARD,

    SIZE
};

class SalesOrder {
private:
    std::string itemId;
    long long orderNumber{-1};
    int quantity{0}, rushStatus{static_cast<int>(RushStatusTier::STANDARD)};

public:
    SalesOrder();
    SalesOrder(const SalesOrder& source);

    ~SalesOrder();

    void operator=(const SalesOrder& source);
    bool operator<(const SalesOrder& rhs) const;
    bool operator>(const SalesOrder& rhs) const;
    bool operator<=(const SalesOrder& rhs) const;
    bool operator>=(const SalesOrder& rhs) const;
    bool operator==(const SalesOrder& rhs) const;
    bool operator!=(const SalesOrder& rhs) const;
    void OutputOrder(Inventory& onHand, double& markupValue, double& revenue, double& cost, std::ostream& outStream) const;

    friend PriorityQueue;
    friend void PrintToBackOrderFile(const SalesOrder& order, std::ofstream& backOrderFile);
    friend void DisplayBackOrderFile();
};

class PriorityQueue {
private:
    static constexpr const int GROWTH_FACTOR{2};
    SalesOrder* pHeap{nullptr};
    enum { MIN = 0, MAX = 1 };
    bool minOrMax{MIN};
    size_t currSize{0}, maxSize{0};
    bool Heapify(int idx);
    inline bool AvailableSpace() const;
    bool AllocateMoreMemory();
    void DeallocateMemory(SalesOrder*& pContainer);

public:
    PriorityQueue();
    PriorityQueue(const PriorityQueue& source);

    ~PriorityQueue();

    void operator=(const PriorityQueue& source);
    void ProcessOrders(Inventory& onHand);
    bool AddOrderToQueue(const SalesOrder& source);
    inline const SalesOrder& Top() const;
    void Pop();
    inline bool IsEmpty() const;
    void DisplayOrders() const;
    bool ReserveMemory(size_t numOfItems);
};


void HandleDefaultMenuCase();
void DisplayMenu();
void PrintToBackOrderFile(const SalesOrder& order, std::ofstream& backOrderFile);
void DisplayBackOrderFile();

static const std::string INPUT_INVENTORY_FILE{"in_inventory.txt"}, INPUT_ORDERS_FILE{"in_orders.txt"}, BACK_ORDER_FILE{"out_backOrder.txt"};

static const double rushStatusMarkup[static_cast<int>(RushStatusTier::SIZE)]{0, 0.68, 0.45, 0.38};
static const std::string rushStatusStrs[static_cast<int>(RushStatusTier::SIZE)]{"", "Extreme", "Expedite", "Standard"};

int main() {

    std::cout << std::fixed << std::setprecision(2);

    enum { INVENTORY_DETAILS = 1, DISPLAY_ORDERS, PROCESS_ORDERS, QUIT };
    int option{0};

    Inventory salesInventory;
    salesInventory.LoadFromFile();

    PriorityQueue salesOrders;

    while (option != QUIT) {
        std::cout << std::fixed << std::setprecision(2);
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
                salesOrders.ProcessOrders(salesInventory);
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

    return EXIT_SUCCESS;

}



Inventory::Inventory() {}

Inventory::Inventory(const Inventory& source) {
    operator=(source);
}

void Inventory::operator=(const Inventory& source) {
    if (this != &source) {
        for (int i = 0; i < MAX_SIZE; ++i) {
            items[i] = source.items[i];
        }
        currSize = source.currSize;
    }
}

Inventory::~Inventory() {}

void Inventory::LoadFromFile() {
    std::ifstream inputData(INPUT_INVENTORY_FILE);
    if (inputData.is_open()) {
        std::string data;
        while (std::getline(inputData, data)) {
            std::istringstream inData(data);
            SalesItem temp;
            inData >> temp.id >> temp.name >> temp.quantity >> temp.price;
            items[currSize] = temp;
            ++currSize;
        }
        inputData.close();
    }
    else {
        std::cerr << "Could not check inventory records file. Closing Program...\n";
        exit(EXIT_FAILURE);
    }
}

int Inventory::GetIdIdx(const std::string& id) const {
    int matchIdx{-1};
    for (int i = 0; matchIdx == -1 && i < currSize; ++i) {
        if (id == items[i].id) {
            matchIdx = i;
        }
    }
    return matchIdx;
}
void Inventory::DisplayDetails() const {
    std::cout << std::left << std::setw(15) << "Item Id" << "|" << std::setw(15) << "Item Name" << "|" << std::setw(15) << "Quantity" << "|" << std::setw(15) << "Price" << "|" << std::right << "\n";
    std::cout << std::setfill('-') << std::setw(64) << "" << std::setfill(' ') << "\n";
    for (int idx = 0; idx < MAX_SIZE; ++idx) {
        std::cout << std::left << std::setw(15) << items[idx].id << "|" << std::setw(15) << items[idx].name << "|" << std::setw(15) << std::right << items[idx].quantity << "|" << std::setw(15) << items[idx].price << "|\n";
    }
}



SalesOrder::SalesOrder() {}

SalesOrder::SalesOrder(const SalesOrder& source) {
    operator=(source);
}

void SalesOrder::operator=(const SalesOrder& source) {
    if (this != &source) {
        itemId = source.itemId;
        orderNumber = source.orderNumber;
        quantity = source.quantity;
        rushStatus = source.rushStatus;
    }
}

SalesOrder::~SalesOrder() {}

void SalesOrder::OutputOrder(Inventory& onHand, double& markupValue, double& revenue, double& cost, std::ostream& outStream) const {
    const int QUANTITY_IDX = onHand.GetIdIdx(itemId);
    if (QUANTITY_IDX >= 0) {
        const double price = onHand.items[QUANTITY_IDX].price;
        markupValue = price * quantity * rushStatusMarkup[rushStatus];
        revenue = price * quantity + markupValue;
        cost = revenue - markupValue;
    }
    outStream
        << std::setw(15) << orderNumber << "|" << std::left
        << std::setw(15) << itemId << "|"
        << std::setw(15) << rushStatusStrs[rushStatus] << "|" << std::right
        << std::setw(20) << quantity << "|"
        << std::setw(14) << rushStatusMarkup[rushStatus] * 100 << "%|"
        << std::setw(15) << markupValue << "|"
        << std::setw(15) << revenue << "|"
        << std::setw(20) << cost << "|\n";
}

bool SalesOrder::operator<(const SalesOrder& rhs) const {
    return (static_cast<int>(rushStatus) < static_cast<int>(rhs.rushStatus)) || (orderNumber < rhs.orderNumber && static_cast<int>(rushStatus) == static_cast<int>(rhs.rushStatus));
}

bool SalesOrder::operator>(const SalesOrder& rhs) const {
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

PriorityQueue::PriorityQueue(const PriorityQueue& source) {
    operator=(source);
}

void PriorityQueue::operator=(const PriorityQueue& source) {
    if (this != &source) {
        PriorityQueue temp;
        if (temp.ReserveMemory(source.currSize)) {
            for (int i = 0; i < source.currSize; ++i) {
                temp.pHeap[i] = source.pHeap[i];
            }
            DeallocateMemory(pHeap);
            pHeap = temp.pHeap;
            temp.pHeap = nullptr;
            currSize = maxSize = source.currSize;
            minOrMax = source.minOrMax;
        }
        else {
            std::cerr << "Could not properly duplicate the source priority queue. The destination queue will be left unmodified\n";
        }
    }
}

PriorityQueue::~PriorityQueue() {
    currSize = maxSize = 0;
    DeallocateMemory(pHeap);
}

bool PriorityQueue::AddOrderToQueue(const SalesOrder& source) {
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
        const int MAX_GAIN_CHILD = (rightChild < currSize && (minOrMax == MIN && pHeap[rightChild] <= pHeap[leftChild]) || (minOrMax == MAX && pHeap[rightChild] >= pHeap[leftChild])) ? rightChild : leftChild;

        if ((minOrMax == MIN && pHeap[idx] > pHeap[MAX_GAIN_CHILD]) || (minOrMax == MAX && pHeap[idx] < pHeap[MAX_GAIN_CHILD])) {
            const SalesOrder temp = pHeap[MAX_GAIN_CHILD];
            pHeap[MAX_GAIN_CHILD] = pHeap[idx];
            pHeap[idx] = temp;
            state = true;
            Heapify(MAX_GAIN_CHILD);
        }
    }
    return state;
}

bool PriorityQueue::AllocateMoreMemory() {
    SalesOrder* pTemp = nullptr;
    bool state = false;
    try {
        const size_t NEW_SIZE = std::max(1ull, currSize * GROWTH_FACTOR);
        pTemp = new SalesOrder[NEW_SIZE];
        for (size_t i = 0; i < currSize; ++i) {
            pTemp[i] = pHeap[i];
        }
        DeallocateMemory(pHeap);
        pHeap = pTemp;
        state = true;
        pTemp = nullptr;
    }
    catch (std::bad_alloc) {
        std::cout << "Not enough Continguous Memory Space to add more elements\n";
    }
    return state;
}

bool PriorityQueue::ReserveMemory(size_t numOfItems) {
    bool success{true};
    if (numOfItems > currSize) {
        PriorityQueue temp;
        success = false;
        try {
            temp.pHeap = new SalesOrder[numOfItems];
            success = (temp.pHeap != nullptr);
        }
        catch (std::bad_alloc) {
            std::cerr << "Could not Reserve Requested Memory Size\n";
        }
        if (success) {
            for (int i = 0; i < currSize; ++i) {
                temp.pHeap[i] = pHeap[i];
            }
            DeallocateMemory(pHeap);
            pHeap = temp.pHeap;
            temp.pHeap = nullptr;
            maxSize = numOfItems;
        }
    }
    return success;
}

void PriorityQueue::DeallocateMemory(SalesOrder*& pContainer) {
    if (pContainer != nullptr) {
        delete[] pContainer;
        pContainer = nullptr;
    }
}

inline bool PriorityQueue::AvailableSpace() const {
    return currSize < maxSize;
}


void PriorityQueue::ProcessOrders(Inventory& onHand) {
    static std::string header;
    if (header.empty()) {
        std::ostringstream heading;
        heading << std::left
            << std::setw(15) << "Order Number" << "|"
            << std::setw(15) << "Item Name" << "|"
            << std::setw(15) << "Rush Status" << "|"
            << std::setw(20) << "Quantity Ordered" << "|"
            << std::setw(15) << "Markup %" << "|"
            << std::setw(15) << "Markup Value" << "|"
            << std::setw(15) << "Revenue" << "|"
            << std::setw(20) << "Manufacturer Cost" << "|\n"
            << std::setfill('-') << std::setw(138) << "" << std::setfill(' ') << std::right << "\n";
        header = heading.str();
    }
    static std::string outMessage;
    if (outMessage.empty()) {
        std::ofstream backOrders(BACK_ORDER_FILE, std::ios_base::out | std::ios_base::trunc);
        if (!backOrders.is_open()) {
            std::cerr << "The directory for storing back-orders couldn't be accessed. To avoid data loss, program now closing...\n";
            exit(EXIT_FAILURE);
        }
        std::fstream ordersFile(INPUT_ORDERS_FILE);
        if (ordersFile.is_open()) {
            PriorityQueue maxHeaps[onHand.MAX_SIZE];
            for (int i = 0; i < onHand.MAX_SIZE; ++i) {
                maxHeaps[i].minOrMax = MAX;
            }
            int quantities[onHand.MAX_SIZE]{};

            SalesOrder order;
            while (ordersFile >> order.orderNumber >> order.itemId >> order.quantity >> order.rushStatus) {
                const int QUANTITY_IDX = onHand.GetIdIdx(order.itemId);
                if (QUANTITY_IDX >= 0) {
                    quantities[QUANTITY_IDX] += order.quantity;
                    maxHeaps[QUANTITY_IDX].AddOrderToQueue(order);
                    if (quantities[QUANTITY_IDX] > onHand.items[QUANTITY_IDX].quantity) {
                        quantities[QUANTITY_IDX] -= maxHeaps[QUANTITY_IDX].Top().quantity;
                        PrintToBackOrderFile(maxHeaps[QUANTITY_IDX].Top(), backOrders);
                        maxHeaps[QUANTITY_IDX].Pop();
                    }
                }
            }
            ordersFile.close();
            PriorityQueue maxHeap;
            for (int i = 0; i < onHand.MAX_SIZE; ++i) {
                while (!maxHeaps[i].IsEmpty()) {
                    AddOrderToQueue(maxHeaps[i].Top());
                    maxHeap.AddOrderToQueue(maxHeaps[i].Top());
                    maxHeaps[i].Pop();
                }
            }
            {
                double totalCost{0}, totalRevenue{0}, totalMarkup{0};
                std::ostringstream processedData;
                while (!maxHeap.IsEmpty()) {
                    const SalesOrder top{maxHeap.Top()};
                    maxHeap.Pop();
                    double markupValue{0}, revenue{0}, cost{0};

                    top.OutputOrder(onHand, markupValue, revenue, cost, processedData);
                    totalRevenue += revenue;
                    totalCost += cost;
                    totalMarkup += markupValue;
                }
                processedData << std::setfill('-') << std::setw(138) << "" << std::setfill(' ') << std::right << "\n"
                    << std::setw(85) << "" << std::setw(15) << totalMarkup << "|" << std::setw(15) << totalRevenue << "|" << std::setw(20) << totalCost << "|\n";
                outMessage = processedData.str();

                ordersFile.open(INPUT_ORDERS_FILE, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
                if (ordersFile.is_open()) {
                    ordersFile.close();
                }
            }
        }
        else {
            std::cerr << "Input File for Unprocessed Orders Couldn't be Opened. As a Result, Cannot Process Orders\n";
        }
        backOrders.close();
    }
    if (!outMessage.empty()) {
        std::cout << header << outMessage << "\n\nBack-Orders:\n\n";
        DisplayBackOrderFile();
    }
}


void PriorityQueue::DisplayOrders() const {
    std::ifstream unprocessedOrders(INPUT_ORDERS_FILE);
    if (unprocessedOrders.is_open()) {
        std::cout << std::left << std::setw(15) << "Order Number" << "|" << std::setw(15) << "Item Id" << "|" << std::setw(15) << "Quantity" << "|" << std::setw(15) << "Rush Status" << "|" << std::right << "\n";
        std::cout << std::setfill('-') << std::setw(64) << "" << std::setfill(' ') << "\n";
        SalesOrder temp;
        while (unprocessedOrders >> temp.orderNumber >> temp.itemId >> temp.quantity >> temp.rushStatus) {
            std::cout << std::setw(15) << temp.orderNumber << "|" << std::left << std::setw(15) << temp.itemId << "|" << std::setw(15) << std::right << temp.quantity << "|" << std::left << std::setw(15) << rushStatusStrs[temp.rushStatus] << std::right << "|\n";
        }
        unprocessedOrders.close();
    }
    else {
        std::cerr << "Could not load unprocessed orders. File inaccessible\n";
    }
}

inline const SalesOrder& PriorityQueue::Top() const {
    return pHeap[0];
}

void PriorityQueue::Pop() {
    --currSize;
    pHeap[0] = pHeap[currSize];
    Heapify(0);
}

inline bool PriorityQueue::IsEmpty() const {
    return currSize == 0;
}



void PrintToBackOrderFile(const SalesOrder& order, std::ofstream& backOrderFile) {
    backOrderFile << order.orderNumber << " " << order.itemId << " " << order.quantity << " " << order.rushStatus << "\n";
}

void DisplayBackOrderFile() {
    std::ifstream backOrders(BACK_ORDER_FILE);
    if (backOrders.is_open()) {
        std::cout << std::left << std::setw(15) << "Order Number" << "|" << std::setw(15) << "Item Id" << "|" << std::setw(15) << "Quantity" << "|" << std::setw(15) << "Rush Status" << "|" << std::right << "\n";
        std::cout << std::setfill('-') << std::setw(64) << "" << std::setfill(' ') << "\n";
        SalesOrder temp;
        while (backOrders >> temp.orderNumber >> temp.itemId >> temp.quantity >> temp.rushStatus) {
            std::cout << std::setw(15) << temp.orderNumber << "|" << std::left << std::setw(15) << temp.itemId << "|" << std::setw(15) << std::right << temp.quantity << "|" << std::left << std::setw(15) << rushStatusStrs[temp.rushStatus] << std::right << "|\n";
        }
        backOrders.close();
    }
    else {
        std::cerr << "Could not load back-orders data. File inaccessible\n";
    }
}

void HandleDefaultMenuCase() {
    std::cout << "Invalid Menu option selected\n";

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

}

void DisplayMenu() {

    std::cout << "\nSelect an option from the following"

        "\n1. Show Current Inventory Details"

        "\n2. Display all Placed Orders"

        "\n3. Process Orders"

        "\n4. Quit\n\n";

}



//TestCases:
//
//TestCase 1: Inventory Details
//Output
// 
//Select an option from the following
//1. Show Current Inventory Details
//2. Display all Placed Orders
//3. Process Orders
//4. Quit
//
//1
//Item Id        |Item Name      |Quantity       |Price          |
//----------------------------------------------------------------
//AS34DF         |Stroller       |             25|         138.79|
//AP34DF         |Dirtbike       |             12|        2795.95|
//AQ34DF         |Bus            |              4|       19999.95|
//AR34DF         |Motorcycle     |             16|         879.95|
//AY34DF         |Bicycle        |             45|         399.95|
//


//TestCase2: Display Unprocessed Orders Before Processed
//Output:
// 
//Select an option from the following
//1. Show Current Inventory Details
//2. Display all Placed Orders
//3. Process Orders
//4. Quit
//
//2
//Order Number   |Item Id        |Quantity       |Rush Status    |
//----------------------------------------------------------------
//              1|AS34DF         |              6|Extreme        |
//              2|AP34DF         |              9|Standard       |
//              3|AQ34DF         |              2|Standard       |
//              4|AR34DF         |              7|Expedite       |
//              5|AY34DF         |              3|Extreme        |
//              6|AS34DF         |              4|Extreme        |
//              7|AP34DF         |              5|Standard       |
//              8|AQ34DF         |              3|Standard       |
//              9|AR34DF         |             12|Expedite       |
//             10|AY34DF         |              6|Extreme        |
//



//TestCase3: Process Orders
//Output:
// 
//Select an option from the following
//1. Show Current Inventory Details
//2. Display all Placed Orders
//3. Process Orders
//4. Quit
//
//3
//Order Number   |Item Name      |Rush Status    |Quantity Ordered    |Markup %       |Markup Value   |Revenue        |Manufacturer Cost   |
//------------------------------------------------------------------------------------------------------------------------------------------
//              1|         AS34DF|        Extreme|                   6|           0.68|        566.263|           1399|              832.74|
//              5|         AY34DF|        Extreme|                   3|           0.68|        815.898|        2015.75|             1199.85|
//              6|         AS34DF|        Extreme|                   4|           0.68|        377.509|        932.669|              555.16|
//             10|         AY34DF|        Extreme|                   6|           0.68|         1631.8|         4031.5|              2399.7|
//              9|         AR34DF|       Expedite|                  12|           0.45|        4751.73|        15311.1|             10559.4|
//              7|         AP34DF|       Standard|                   5|           0.38|        5312.31|        19292.1|             13979.8|
//              8|         AQ34DF|       Standard|                   3|           0.38|        22799.9|        82799.8|             59999.9|
//------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                             36255.4|         125782|             89526.5|
//
//
//Back-Orders:
//
//Order Number   |Item Id        |Quantity       |Rush Status    |
//----------------------------------------------------------------
//              2|AP34DF         |              9|Standard       |
//              3|AQ34DF         |              2|Standard       |
//              4|AR34DF         |              7|Expedite       |
//



//TestCase4: Display Unprocessed Orders After Processed
//Select an option from the following
//1. Show Current Inventory Details
//2. Display all Placed Orders
//3. Process Orders
//4. Quit
//
//2
//Order Number   |Item Id        |Quantity       |Rush Status    |
//----------------------------------------------------------------
//


//TestCase5: Quit
//Output:
// 
//Select an option from the following
//1. Show Current Inventory Details
//2. Display all Placed Orders
//3. Process Orders
//4. Quit
//
//4
//
//Program now closed...