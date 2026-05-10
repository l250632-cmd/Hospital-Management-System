#pragma once
#include "MyString.h"
#include <iostream>

class Appointment {
private:
    int appointmentId;
    int patientId;
    int doctorId;
    char date[20];       // DD-MM-YYYY
    char timeSlot[10];   // HH:MM
    char status[20];     // pending, completed, cancelled, noshow

public:
    Appointment() {
        appointmentId = 0;
        patientId = 0;
        doctorId = 0;
        date[0] = '\0';
        timeSlot[0] = '\0';
        myStrCopy(status, "pending");
    }

    Appointment(int aid, int pid, int did, const char* date,
                const char* slot, const char* status) {
        this->appointmentId = aid;
        this->patientId = pid;
        this->doctorId = did;
        myStrCopy(this->date, date);
        myStrCopy(this->timeSlot, slot);
        myStrCopy(this->status, status);
    }

    // Getters
    int getAppointmentId() const { return appointmentId; }
    int getPatientId() const { return patientId; }
    int getDoctorId() const { return doctorId; }
    const char* getDate() const { return date; }
    const char* getTimeSlot() const { return timeSlot; }
    const char* getStatus() const { return status; }

    // Setters
    void setAppointmentId(int id) { appointmentId = id; }
    void setPatientId(int id) { patientId = id; }
    void setDoctorId(int id) { doctorId = id; }
    void setDate(const char* d) { myStrCopy(date, d); }
    void setTimeSlot(const char* s) { myStrCopy(timeSlot, s); }
    void setStatus(const char* s) { myStrCopy(status, s); }

    // Detects if two appointments conflict:
    // same doctor, same date, same time slot, neither is cancelled
    bool operator==(const Appointment& other) const {
        if (this->doctorId != other.doctorId) return false;
        if (!myStrEqual(this->date, other.date)) return false;
        if (!myStrEqual(this->timeSlot, other.timeSlot)) return false;
        if (myStrEqual(this->status, "cancelled")) return false;
        if (myStrEqual(other.status, "cancelled")) return false;
        return true;
    }

    // Display appointment info
    friend std::ostream& operator<<(std::ostream& out, const Appointment& a) {
        out << "ID: " << a.appointmentId
            << " | Patient: " << a.patientId
            << " | Doctor: " << a.doctorId
            << " | Date: " << a.date
            << " | Time: " << a.timeSlot
            << " | Status: " << a.status;
        return out;
    }
};
