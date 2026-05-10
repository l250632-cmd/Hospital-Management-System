#pragma once
#include "Person.h"
#include <iostream>

class Doctor : public Person {
private:
    char specialization[100];
    float fee;

public:
    Doctor() : Person() {
        specialization[0] = '\0';
        fee = 0.0f;
    }

    Doctor(int id, const char* name, const char* spec, const char* contact,
           const char* pass, float fee)
        : Person(id, name, pass, contact)
    {
        myStrCopy(this->specialization, spec);
        this->fee = fee;
    }

    // Pure virtual implementations
    void displayMenu() override {
        std::cout << "\n=== Doctor Menu ===\n";
        std::cout << "1. View Today's Appointments\n";
        std::cout << "2. Mark Appointment Complete\n";
        std::cout << "3. Mark Appointment No-Show\n";
        std::cout << "4. Write Prescription\n";
        std::cout << "5. View Patient Medical History\n";
        std::cout << "6. Logout\n";
    }

    void displayInfo() const override {
        std::cout << "Dr. " << name
                  << " | Spec: " << specialization
                  << " | Fee: PKR " << fee << "\n";
    }

    // Getters
    const char* getSpecialization() const { return specialization; }
    float getFee() const { return fee; }

    // Setters
    void setSpecialization(const char* s) { myStrCopy(specialization, s); }
    void setFee(float f) { fee = f; }

    // Compare doctors by ID
    bool operator==(const Doctor& other) const {
        return this->id == other.id;
    }

    // Display doctor info
    friend std::ostream& operator<<(std::ostream& out, const Doctor& d) {
        out << "ID: " << d.id
            << " | Name: " << d.name
            << " | Specialization: " << d.specialization
            << " | Contact: " << d.contact
            << " | Fee: PKR " << d.fee;
        return out;
    }
};
