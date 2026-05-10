#pragma once
#include "MyString.h"

// Abstract base class for all user types in the system
class Person {
protected:
    int id;
    char name[100];
    char password[100];
    char contact[20];

public:
    Person() {
        id = 0;
        name[0] = '\0';
        password[0] = '\0';
        contact[0] = '\0';
    }

    Person(int id, const char* name, const char* pass, const char* contact) {
        this->id = id;
        myStrCopy(this->name, name);
        myStrCopy(this->password, pass);
        myStrCopy(this->contact, contact);
    }

    // Pure virtual functions - must be implemented by derived classes
    virtual void displayMenu() = 0;
    virtual void displayInfo() const = 0;

    // Getters
    int getId() const { return id; }
    const char* getName() const { return name; }
    const char* getPassword() const { return password; }
    const char* getContact() const { return contact; }

    // Setters
    void setId(int i) { id = i; }
    void setName(const char* n) { myStrCopy(name, n); }
    void setPassword(const char* p) { myStrCopy(password, p); }
    void setContact(const char* c) { myStrCopy(contact, c); }

    virtual ~Person() {}
};
