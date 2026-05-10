#pragma once
#include "Person.h"
#include <iostream>

class Admin : public Person {
public:
    Admin() : Person() {}

    Admin(int id, const char* name, const char* pass)
        : Person(id, name, pass, "N/A")
    {}

    void displayMenu() override {
        std::cout << "\n=== Admin Panel - MediCore ===\n";
        std::cout << "1. Add Doctor\n";
        std::cout << "2. Remove Doctor\n";
        std::cout << "3. View All Patients\n";
        std::cout << "4. View All Doctors\n";
        std::cout << "5. View All Appointments\n";
        std::cout << "6. View Unpaid Bills\n";
        std::cout << "7. Discharge Patient\n";
        std::cout << "8. View Security Log\n";
        std::cout << "9. Generate Daily Report\n";
        std::cout << "10. Logout\n";
    }

    void displayInfo() const override {
        std::cout << "Admin: " << name << "\n";
    }
};
