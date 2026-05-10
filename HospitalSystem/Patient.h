#pragma once
#include "Person.h"
#include <iostream>

class Patient : public Person {
private:
    int age;
    char gender[5]; // M or F
    float balance;

public:
    Patient() : Person() {
        age = 0;
        gender[0] = '\0';
        balance = 0.0f;
    }

    Patient(int id, const char* name, int age, const char* gender,
            const char* contact, const char* pass, float balance)
        : Person(id, name, pass, contact)
    {
        this->age = age;
        myStrCopy(this->gender, gender);
        this->balance = balance;
    }

    // Pure virtual implementations
    void displayMenu() override {
        std::cout << "\n=== Patient Menu ===\n";
        std::cout << "1. Book Appointment\n";
        std::cout << "2. Cancel Appointment\n";
        std::cout << "3. View My Appointments\n";
        std::cout << "4. View My Medical Records\n";
        std::cout << "5. View My Bills\n";
        std::cout << "6. Pay Bill\n";
        std::cout << "7. Top Up Balance\n";
        std::cout << "8. Logout\n";
    }

    void displayInfo() const override {
        std::cout << "Patient: " << name << " | Age: " << age
                  << " | Balance: PKR " << balance << "\n";
    }

    // Getters
    int getAge() const { return age; }
    const char* getGender() const { return gender; }
    float getBalance() const { return balance; }

    // Setters
    void setAge(int a) { age = a; }
    void setGender(const char* g) { myStrCopy(gender, g); }
    void setBalance(float b) { balance = b; }

    // Overloaded operators
    // Compare patients by ID
    bool operator==(const Patient& other) const {
        return this->id == other.id;
    }

    // Add to balance
    Patient& operator+=(float amount) {
        balance += amount;
        return *this;
    }

    // Deduct from balance
    Patient& operator-=(float amount) {
        balance -= amount;
        return *this;
    }

    // Display patient info
    friend std::ostream& operator<<(std::ostream& out, const Patient& p) {
        out << "ID: " << p.id
            << " | Name: " << p.name
            << " | Age: " << p.age
            << " | Gender: " << p.gender
            << " | Contact: " << p.contact
            << " | Balance: PKR " << p.balance;
        return out;
    }
};
