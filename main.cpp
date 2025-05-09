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
    int* pArr{nullptr};
    size_t arrSize{0};
    void Heapify();
  
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


void PriorityQueue::Heapify() {
    
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

