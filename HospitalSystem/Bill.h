#pragma once
#include "MyString.h"
#include <iostream>

class Bill {
private:
    int billId;
    int patientId;
    int appointmentId;
    float amount;
    char status[20];   // unpaid, paid, cancelled
    char date[20];     // DD-MM-YYYY

public:
    Bill() {
        billId = 0;
        patientId = 0;
        appointmentId = 0;
        amount = 0.0f;
        myStrCopy(status, "unpaid");
        date[0] = '\0';
    }

    Bill(int bid, int pid, int aid, float amt, const char* stat, const char* dt) {
        billId = bid;
        patientId = pid;
        appointmentId = aid;
        amount = amt;
        myStrCopy(status, stat);
        myStrCopy(date, dt);
    }

    // Getters
    int getBillId() const { return billId; }
    int getPatientId() const { return patientId; }
    int getAppointmentId() const { return appointmentId; }
    float getAmount() const { return amount; }
    const char* getStatus() const { return status; }
    const char* getDate() const { return date; }

    // Setters
    void setBillId(int id) { billId = id; }
    void setPatientId(int id) { patientId = id; }
    void setAppointmentId(int id) { appointmentId = id; }
    void setAmount(float a) { amount = a; }
    void setStatus(const char* s) { myStrCopy(status, s); }
    void setDate(const char* d) { myStrCopy(date, d); }
};
