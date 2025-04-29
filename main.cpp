#include <iostream>
#include <queue>
#include <vector>
#include <stack>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <iomanip>

using namespace std::chrono_literals;

struct Item {
    std::string id;
    float weight;
};

struct ConveyorData {
    std::queue<Item> items;
    bool production_done = false;
    std::mutex mtx;
    std::condition_variable cv;
};

class CarStack {
private:
    std::stack<Item> items;
    float capacity;
    float current_load;

public:
    CarStack(float cap) : capacity(cap), current_load(0.0f) {
        if (cap <= 0) throw std::invalid_argument("Capacity must be positive");
    }

    bool canAdd(float weight) const {
        return (current_load + weight) <= capacity;
    }

    bool push(const Item& item) {
        if (canAdd(item.weight)) {
            items.push(item);
            current_load += item.weight;
            return true;
        }
        return false;
    }

    std::vector<Item> getItems() const {
        std::vector<Item> result;
        auto temp = items;
        
        while (!temp.empty()) {
            result.push_back(temp.top());
            temp.pop();
        }
        
        std::reverse(result.begin(), result.end());
        return result;
    }

    float getCapacity() const { return capacity; }
    float getCurrentLoad() const { return current_load; }
};

void generate_conveyor(ConveyorData& data) {
    std::srand(std::time(nullptr));
    int num_items = std::rand() % 11 + 5;
    
    for (int i = 0; i < num_items; ++i) {
        std::this_thread::sleep_for(500ms); 
        
        Item new_item{
            "p" + std::to_string(i+1),
            (std::rand() % 100 + 1) / 10.0f
        };
        
        {
            std::lock_guard<std::mutex> lock(data.mtx);
            data.items.push(new_item);
        }
        data.cv.notify_one();
    }
    
    {
        std::lock_guard<std::mutex> lock(data.mtx);
        data.production_done = true;
    }
    data.cv.notify_one();
}

int main() {
    ConveyorData conveyor_data;
    std::vector<CarStack*> cars;
    CarStack* current_car = nullptr;

    std::thread producer_thread(generate_conveyor, std::ref(conveyor_data));

    while (true) {
        std::unique_lock<std::mutex> lock(conveyor_data.mtx);
        conveyor_data.cv.wait(lock, [&]{
            return !conveyor_data.items.empty() || conveyor_data.production_done;
        });

        if (!conveyor_data.items.empty()) {
            Item item = conveyor_data.items.front();
            conveyor_data.items.pop();
            lock.unlock();

            if (!current_car) {
                float new_cap = item.weight + (std::rand() % 150 + 1) / 10.0f;
                current_car = new CarStack(new_cap);
            }

            if (!current_car->push(item)) {
                cars.push_back(current_car);
                float new_cap = item.weight + (std::rand() % 150 + 1) / 10.0f;
                current_car = new CarStack(new_cap);
                current_car->push(item);
                std::cout<<"przyjazd nowego samochódu\n";
            }

            std::cout << "Załadowano: " << item.id 
                      << " (" << item.weight << " kg)\n";
            std::cout << "Aktualny samochód: " 
                      << current_car->getCurrentLoad() << "/"
                      << current_car->getCapacity() << " kg\n\n";
        }
        else if (conveyor_data.production_done) {
            break;
        }
    }

    if (current_car) {
        cars.push_back(current_car);
    }

    producer_thread.join();

    std::cout << "\nPodsumowanie:\n";
    for (size_t i = 0; i < cars.size(); ++i) {
        auto items = cars[i]->getItems();
        std::cout << "Samochód " << i+1 << " (" 
                  << cars[i]->getCurrentLoad() << "/"
                  << cars[i]->getCapacity() << " kg): "<<"\n\n";

        std::cout<<"      /----|  ";
        int ile=0;
        for (const auto& item : items) {
            ile++;
            std::cout << item.weight << "  ";
        }
        std::cout << "\n";
        std::cout << "      |----| ";
        for (int j=0;j<ile;++j){
            std::cout<<"-----";
        }
        std::cout << "\n";
    
        std::cout << "       o  o "; 
        for (int j=0;j<ile;++j){
            std::cout<<" o  o ";
        }
        std::cout << "\n\n";
    }

    for (auto& car : cars) delete car;

    return 0;
}