#include "CarStack.h"
#include <cassert>
#include <iostream>

void test_car_creation() {
    CarStack car(10.0f);
    assert(car.getCapacity() == 10.0f);
    assert(car.getCurrentLoad() == 0.0f);
    std::cout << "test_car_creation: OK\n";
}

void test_item_adding() {
    CarStack car(5.0f);
    Item item1{"p1", 2.5f};
    Item item2{"p2", 1.5f};
    
    assert(car.canAdd(item1.weight) == true);
    assert(car.push(item1) == true);
    assert(car.getCurrentLoad() == 2.5f);
    
    assert(car.canAdd(item2.weight) == true);
    assert(car.push(item2) == true);
    assert(car.getCurrentLoad() == 4.0f);
    
    std::cout << "test_item_adding: OK\n";
}

void test_capacity_limits() {
    CarStack car(3.0f);
    Item item1{"p1", 2.5f};
    Item item2{"p2", 1.0f};
    
    assert(car.push(item1) == true);
    assert(car.canAdd(item2.weight) == false);
    assert(car.push(item2) == false);
    assert(car.getCurrentLoad() == 2.5f);
    
    std::cout << "test_capacity_limits: OK\n";
}

void test_item_order() {
    CarStack car(10.0f);
    car.push({"p1", 1.0f});
    car.push({"p2", 2.0f});
    car.push({"p3", 3.0f});
    
    auto items = car.getItems();
    assert(items.size() == 3);
    assert(items[0].weight == 1.0f);
    assert(items[1].weight == 2.0f);
    assert(items[2].weight == 3.0f);
    
    std::cout << "test_item_order: OK\n";
}

void test_edge_cases() {
    // Test pełnego wykorzystania capacity
    CarStack car1(5.0f);
    assert(car1.push({"p1", 5.0f}) == true);
    assert(car1.getCurrentLoad() == 5.0f);
    
    // Test zerowej pojemności
    try {
        CarStack car2(0.0f);
        assert(false); // Nie powinno się tu dostać
    } catch (const std::invalid_argument&) {
        assert(true);
    }
    
    std::cout << "test_edge_cases: OK\n";
}

int main() {
    test_car_creation();
    test_item_adding();
    test_capacity_limits();
    test_item_order();
    test_edge_cases();
    
    std::cout << "All tests passed!\n";
    return 0;
}