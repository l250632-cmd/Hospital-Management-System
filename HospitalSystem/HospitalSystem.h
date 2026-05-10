#pragma once
// Disable MSVC unsafe function warnings (localtime, etc.)
#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif
#include <ctime>
#include <cstdio>
#include <iostream>
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "FileHandler.h"
#include "Validator.h"
#include "HospitalException.h"
#include "MyString.h"

// Main system class that ties everything together
class HospitalSystem {
private:
    Storage<Patient>      patients;
    Storage<Doctor>       doctors;
    Storage<Appointment>  appointments;
    Storage<Bill>         bills;
    Storage<Prescription> prescriptions;
    Admin                 admin;

    // ---- Helper: get today's date as DD-MM-YYYY ----
    void getTodayDate(char* buf) const {
        time_t now = time(nullptr);
        struct tm* t = localtime(&now);
        strftime(buf, 20, "%d-%m-%Y", t);
    }

    // ---- Helper: get current year ----
    int getCurrentYear() const {
        time_t now = time(nullptr);
        struct tm* t = localtime(&now);
        return t->tm_year + 1900;
    }

    // ---- Helper: find patient by ID ----
    Patient* findPatient(int id) {
        for (int i = 0; i < patients.size(); i++) {
            if (patients.getAt(i)->getId() == id)
                return patients.getAt(i);
        }
        return nullptr;
    }

    // ---- Helper: find doctor by ID ----
    Doctor* findDoctor(int id) {
        for (int i = 0; i < doctors.size(); i++) {
            if (doctors.getAt(i)->getId() == id)
                return doctors.getAt(i);
        }
        return nullptr;
    }

    // ---- Helper: find appointment by ID ----
    Appointment* findAppointment(int id) {
        for (int i = 0; i < appointments.size(); i++) {
            if (appointments.getAt(i)->getAppointmentId() == id)
                return appointments.getAt(i);
        }
        return nullptr;
    }

    // ---- Helper: find bill by ID ----
    Bill* findBill(int id) {
        for (int i = 0; i < bills.size(); i++) {
            if (bills.getAt(i)->getBillId() == id)
                return bills.getAt(i);
        }
        return nullptr;
    }

    // ---- Helper: get next appointment ID ----
    int getNextAppointmentId() {
        int maxId = 0;
        for (int i = 0; i < appointments.size(); i++) {
            if (appointments.getAt(i)->getAppointmentId() > maxId)
                maxId = appointments.getAt(i)->getAppointmentId();
        }
        return maxId + 1;
    }

    // ---- Helper: get next bill ID ----
    int getNextBillId() {
        int maxId = 0;
        for (int i = 0; i < bills.size(); i++) {
            if (bills.getAt(i)->getBillId() > maxId)
                maxId = bills.getAt(i)->getBillId();
        }
        return maxId + 1;
    }

    // ---- Helper: get next doctor ID ----
    int getNextDoctorId() {
        int maxId = 0;
        for (int i = 0; i < doctors.size(); i++) {
            if (doctors.getAt(i)->getId() > maxId)
                maxId = doctors.getAt(i)->getId();
        }
        return maxId + 1;
    }

    // ---- Helper: get next prescription ID ----
    int getNextPrescriptionId() {
        int maxId = 0;
        for (int i = 0; i < prescriptions.size(); i++) {
            if (prescriptions.getAt(i)->getPrescriptionId() > maxId)
                maxId = prescriptions.getAt(i)->getPrescriptionId();
        }
        return maxId + 1;
    }

    // ---- Simple bubble sort for appointments by date (ascending) ----
    void sortAppointmentsByDateAsc(Appointment** arr, int n) {
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                // Compare dates - format DD-MM-YYYY, convert to YYYY-MM-DD for comparison
                // Extract year, month, day manually
                const char* d1 = arr[j]->getDate();
                const char* d2 = arr[j+1]->getDate();
                // compare year first (chars 6-9), then month (3-4), then day (0-1)
                int cmp = 0;
                for (int k = 6; k < 10 && cmp == 0; k++) cmp = d1[k] - d2[k];
                for (int k = 3; k < 5 && cmp == 0; k++) cmp = d1[k] - d2[k];
                for (int k = 0; k < 2 && cmp == 0; k++) cmp = d1[k] - d2[k];
                // also compare time slot if dates equal
                if (cmp == 0) cmp = arr[j]->getTimeSlot()[0] - arr[j+1]->getTimeSlot()[0];
                if (cmp > 0) {
                    Appointment* tmp = arr[j];
                    arr[j] = arr[j+1];
                    arr[j+1] = tmp;
                }
            }
        }
    }

    // ---- Sort appointments by date descending ----
    void sortAppointmentsByDateDesc(Appointment** arr, int n) {
        sortAppointmentsByDateAsc(arr, n);
        // reverse
        for (int i = 0, j = n - 1; i < j; i++, j--) {
            Appointment* tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
        }
    }

    // ---- Check if a slot is taken for doctor on date ----
    bool isSlotTaken(int doctorId, const char* date, const char* slot) {
        for (int i = 0; i < appointments.size(); i++) {
            Appointment* a = appointments.getAt(i);
            if (a->getDoctorId() == doctorId &&
                myStrEqual(a->getDate(), date) &&
                myStrEqual(a->getTimeSlot(), slot) &&
                !myStrEqual(a->getStatus(), "cancelled")) {
                return true;
            }
        }
        return false;
    }

    // ---- Parse date difference in days ----
    // date format DD-MM-YYYY
    double dateDiffFromToday(const char* date) {
        // parse the date
        char dayBuf[3], monBuf[3], yrBuf[5];
        dayBuf[0] = date[0]; dayBuf[1] = date[1]; dayBuf[2] = '\0';
        monBuf[0] = date[3]; monBuf[1] = date[4]; monBuf[2] = '\0';
        yrBuf[0]  = date[6]; yrBuf[1]  = date[7];
        yrBuf[2]  = date[8]; yrBuf[3]  = date[9]; yrBuf[4]  = '\0';

        struct tm t = {};
        t.tm_mday = myStrToInt(dayBuf);
        t.tm_mon  = myStrToInt(monBuf) - 1;
        t.tm_year = myStrToInt(yrBuf) - 1900;
        time_t then = mktime(&t);
        time_t now  = time(nullptr);
        return difftime(now, then) / 86400.0;
    }

public:
    // ---- Startup: load everything from files ----
    void loadAll() {
        FileHandler::loadPatients(patients);
        FileHandler::loadDoctors(doctors);
        FileHandler::loadAdmin(admin);
        FileHandler::loadAppointments(appointments);
        FileHandler::loadBills(bills);
        FileHandler::loadPrescriptions(prescriptions);
    }

    // ---- Login validation ----
    // returns 0=fail, 1=patient, 2=doctor, 3=admin
    int loginPatient(int id, const char* pass, char* outName) {
        Patient* p = findPatient(id);
        if (!p) return 0;
        if (!myStrEqual(p->getPassword(), pass)) return 0;
        myStrCopy(outName, p->getName());
        return 1;
    }

    int loginDoctor(int id, const char* pass, char* outName) {
        Doctor* d = findDoctor(id);
        if (!d) return 0;
        if (!myStrEqual(d->getPassword(), pass)) return 0;
        myStrCopy(outName, d->getName());
        return 1;
    }

    int loginAdmin(int id, const char* pass) {
        if (admin.getId() != id) return 0;
        if (!myStrEqual(admin.getPassword(), pass)) return 0;
        return 1;
    }

    // ---- Patient operations ----

    // Returns result message in outMsg
    void bookAppointment(int patientId, int doctorId, const char* date,
                         const char* slot, char* outMsg) {
        Patient* p = findPatient(patientId);
        Doctor*  d = findDoctor(doctorId);

        if (!p) { myStrCopy(outMsg, "Patient not found."); return; }
        if (!d) { myStrCopy(outMsg, "Doctor not found."); return; }

        if (!Validator::isValidDate(date, getCurrentYear())) {
            myStrCopy(outMsg, "Invalid date. Use format DD-MM-YYYY.");
            return;
        }
        if (!Validator::isValidTimeSlot(slot)) {
            myStrCopy(outMsg, "Invalid time slot.");
            return;
        }

        // Check slot availability
        if (isSlotTaken(doctorId, date, slot)) {
            try {
                throw SlotUnavailableException("Slot is already booked. Please choose another.");
            } catch (SlotUnavailableException& e) {
                myStrCopy(outMsg, e.what());
                return;
            }
        }

        // Check balance
        if (p->getBalance() < d->getFee()) {
            try {
                throw InsufficientFundsException("Insufficient balance. Please top up.");
            } catch (InsufficientFundsException& e) {
                myStrCopy(outMsg, e.what());
                return;
            }
        }

        // Deduct fee
        *p -= d->getFee();

        // Create appointment
        int newAid = getNextAppointmentId();
        Appointment a(newAid, patientId, doctorId, date, slot, "pending");
        appointments.add(a);
        FileHandler::appendAppointment(a);

        // Create bill
        int newBid = getNextBillId();
        Bill b(newBid, patientId, newAid, d->getFee(), "unpaid", date);
        bills.add(b);
        FileHandler::appendBill(b);

        // Update patient file
        FileHandler::saveAllPatients(patients);

        myStrCopy(outMsg, "Appointment booked successfully. Appointment ID: ");
        char idBuf[10];
        myIntToStr(newAid, idBuf);
        myStrCat(outMsg, idBuf);
    }

    void cancelAppointment(int patientId, int appointmentId, char* outMsg) {
        Patient*     p = findPatient(patientId);
        Appointment* a = findAppointment(appointmentId);

        if (!a || a->getPatientId() != patientId || !myStrEqual(a->getStatus(), "pending")) {
            myStrCopy(outMsg, "Invalid appointment ID.");
            return;
        }

        Doctor* d = findDoctor(a->getDoctorId());
        float refund = d ? d->getFee() : 0.0f;

        a->setStatus("cancelled");

        // Refund
        if (p) *p += refund;

        // Cancel corresponding bill
        for (int i = 0; i < bills.size(); i++) {
            Bill* b = bills.getAt(i);
            if (b->getAppointmentId() == appointmentId) {
                b->setStatus("cancelled");
                break;
            }
        }

        FileHandler::saveAllAppointments(appointments);
        FileHandler::saveAllBills(bills);
        FileHandler::saveAllPatients(patients);

        myStrCopy(outMsg, "Appointment cancelled. PKR ");
        char fbuf[20];
        myFloatToStr(refund, fbuf);
        myStrCat(outMsg, fbuf);
        myStrCat(outMsg, " refunded to your balance.");
    }

    void topUpBalance(int patientId, float amount, char* outMsg) {
        if (amount <= 0) {
            try {
                throw InvalidInputException("Amount must be greater than 0.");
            } catch (InvalidInputException& e) {
                myStrCopy(outMsg, e.what());
                return;
            }
        }
        Patient* p = findPatient(patientId);
        if (!p) { myStrCopy(outMsg, "Patient not found."); return; }

        *p += amount;
        FileHandler::saveAllPatients(patients);

        myStrCopy(outMsg, "Balance updated. New balance: PKR ");
        char buf[20];
        myFloatToStr(p->getBalance(), buf);
        myStrCat(outMsg, buf);
    }

    void payBill(int patientId, int billId, char* outMsg) {
        Patient* p = findPatient(patientId);
        Bill* b = findBill(billId);

        if (!b || b->getPatientId() != patientId || !myStrEqual(b->getStatus(), "unpaid")) {
            myStrCopy(outMsg, "Invalid bill ID.");
            return;
        }

        if (p->getBalance() < b->getAmount()) {
            try {
                throw InsufficientFundsException("Insufficient balance to pay this bill.");
            } catch (InsufficientFundsException& e) {
                myStrCopy(outMsg, e.what());
                return;
            }
        }

        *p -= b->getAmount();
        b->setStatus("paid");

        FileHandler::saveAllBills(bills);
        FileHandler::saveAllPatients(patients);

        myStrCopy(outMsg, "Bill paid successfully. Remaining balance: PKR ");
        char buf[20];
        myFloatToStr(p->getBalance(), buf);
        myStrCat(outMsg, buf);
    }

    // ---- Doctor operations ----

    void markAppointmentComplete(int doctorId, int appointmentId, char* outMsg) {
        Appointment* a = findAppointment(appointmentId);
        char today[20];
        getTodayDate(today);

        if (!a || a->getDoctorId() != doctorId ||
            !myStrEqual(a->getStatus(), "pending") ||
            !myStrEqual(a->getDate(), today)) {
            myStrCopy(outMsg, "Invalid appointment ID.");
            return;
        }

        a->setStatus("completed");
        FileHandler::saveAllAppointments(appointments);
        myStrCopy(outMsg, "Appointment marked as completed.");
    }

    void markAppointmentNoShow(int doctorId, int appointmentId, char* outMsg) {
        Appointment* a = findAppointment(appointmentId);
        char today[20];
        getTodayDate(today);

        if (!a || a->getDoctorId() != doctorId ||
            !myStrEqual(a->getStatus(), "pending") ||
            !myStrEqual(a->getDate(), today)) {
            myStrCopy(outMsg, "Invalid appointment ID.");
            return;
        }

        a->setStatus("noshow");
        // cancel the bill too, no refund
        for (int i = 0; i < bills.size(); i++) {
            Bill* b = bills.getAt(i);
            if (b->getAppointmentId() == appointmentId) {
                b->setStatus("cancelled");
                break;
            }
        }
        FileHandler::saveAllAppointments(appointments);
        FileHandler::saveAllBills(bills);
        myStrCopy(outMsg, "Appointment marked as no-show.");
    }

    void writePrescription(int doctorId, int appointmentId,
                           const char* medicines, const char* notes, char* outMsg) {
        Appointment* a = findAppointment(appointmentId);

        if (!a || a->getDoctorId() != doctorId || !myStrEqual(a->getStatus(), "completed")) {
            myStrCopy(outMsg, "Invalid appointment ID or appointment not completed.");
            return;
        }

        // Check if prescription already exists
        for (int i = 0; i < prescriptions.size(); i++) {
            if (prescriptions.getAt(i)->getAppointmentId() == appointmentId) {
                myStrCopy(outMsg, "Prescription already written for this appointment.");
                return;
            }
        }

        int newPid = getNextPrescriptionId();
        Prescription pr(newPid, appointmentId, a->getPatientId(),
                        doctorId, a->getDate(), medicines, notes);
        prescriptions.add(pr);
        FileHandler::appendPrescription(pr);
        myStrCopy(outMsg, "Prescription saved.");
    }

    // ---- Admin operations ----

    void addDoctor(const char* name, const char* spec, const char* contact,
                   const char* pass, float fee, char* outMsg) {
        if (!Validator::isValidContact(contact)) {
            myStrCopy(outMsg, "Invalid contact number. Must be 11 digits.");
            return;
        }
        if (!Validator::isValidPassword(pass)) {
            myStrCopy(outMsg, "Password must be at least 6 characters.");
            return;
        }
        if (fee <= 0) {
            myStrCopy(outMsg, "Fee must be positive.");
            return;
        }

        int newId = getNextDoctorId();
        Doctor d(newId, name, spec, contact, pass, fee);
        doctors.add(d);
        FileHandler::appendDoctor(d);

        myStrCopy(outMsg, "Doctor added successfully. ID: ");
        char buf[10];
        myIntToStr(newId, buf);
        myStrCat(outMsg, buf);
    }

    void removeDoctor(int doctorId, char* outMsg) {
        // Check for pending appointments
        for (int i = 0; i < appointments.size(); i++) {
            Appointment* a = appointments.getAt(i);
            if (a->getDoctorId() == doctorId && myStrEqual(a->getStatus(), "pending")) {
                myStrCopy(outMsg, "Cannot remove doctor with pending appointments.");
                return;
            }
        }

        bool found = false;
        for (int i = 0; i < doctors.size(); i++) {
            if (doctors.getAt(i)->getId() == doctorId) {
                doctors.removeAt(i);
                found = true;
                break;
            }
        }

        if (!found) { myStrCopy(outMsg, "Doctor not found."); return; }

        FileHandler::saveAllDoctors(doctors);
        myStrCopy(outMsg, "Doctor removed.");
    }

    void dischargePatient(int patientId, char* outMsg) {
        Patient* p = findPatient(patientId);
        if (!p) { myStrCopy(outMsg, "Patient not found."); return; }

        // Check for unpaid bills
        for (int i = 0; i < bills.size(); i++) {
            Bill* b = bills.getAt(i);
            if (b->getPatientId() == patientId && myStrEqual(b->getStatus(), "unpaid")) {
                myStrCopy(outMsg, "Cannot discharge patient with unpaid bills.");
                return;
            }
        }

        // Check for pending appointments
        for (int i = 0; i < appointments.size(); i++) {
            Appointment* a = appointments.getAt(i);
            if (a->getPatientId() == patientId && myStrEqual(a->getStatus(), "pending")) {
                myStrCopy(outMsg, "Cannot discharge patient with pending appointments.");
                return;
            }
        }

        // Archive
        FileHandler::archivePatient(patientId, patients, appointments, bills, prescriptions);

        // Remove from memory and files
        for (int i = 0; i < patients.size(); i++) {
            if (patients.getAt(i)->getId() == patientId) {
                patients.removeAt(i); break;
            }
        }
        // Remove appointments
        for (int i = appointments.size() - 1; i >= 0; i--) {
            if (appointments.getAt(i)->getPatientId() == patientId)
                appointments.removeAt(i);
        }
        // Remove bills
        for (int i = bills.size() - 1; i >= 0; i--) {
            if (bills.getAt(i)->getPatientId() == patientId)
                bills.removeAt(i);
        }
        // Remove prescriptions
        for (int i = prescriptions.size() - 1; i >= 0; i--) {
            if (prescriptions.getAt(i)->getPatientId() == patientId)
                prescriptions.removeAt(i);
        }

        FileHandler::saveAllPatients(patients);
        FileHandler::saveAllAppointments(appointments);
        FileHandler::saveAllBills(bills);
        FileHandler::saveAllPrescriptions(prescriptions);

        myStrCopy(outMsg, "Patient discharged and archived successfully.");
    }

    // ---- Getters for SFML GUI to use ----

    Storage<Patient>& getPatients() { return patients; }
    Storage<Doctor>& getDoctors() { return doctors; }
    Storage<Appointment>& getAppointments() { return appointments; }
    Storage<Bill>& getBills() { return bills; }
    Storage<Prescription>& getPrescriptions() { return prescriptions; }
    Admin& getAdmin() { return admin; }

    // Get patient balance as string
    void getPatientBalance(int id, char* buf) {
        Patient* p = findPatient(id);
        if (p) myFloatToStr(p->getBalance(), buf);
        else myStrCopy(buf, "0.00");
    }

    // Get doctor fee as string
    void getDoctorFee(int id, char* buf) {
        Doctor* d = findDoctor(id);
        if (d) myFloatToStr(d->getFee(), buf);
        else myStrCopy(buf, "0.00");
    }

    // Get today's date string
    void getTodayStr(char* buf) { getTodayDate(buf); }

    // Build appointment list for a patient (for display)
    // Returns number of appointments placed in outArr
    int getPatientAppointments(int patientId, Appointment* outArr, int maxN) {
        int count = 0;
        for (int i = 0; i < appointments.size() && count < maxN; i++) {
            Appointment* a = appointments.getAt(i);
            if (a->getPatientId() == patientId) {
                outArr[count++] = *a;
            }
        }
        return count;
    }

    int getDoctorTodayAppointments(int doctorId, Appointment* outArr, int maxN) {
        char today[20];
        getTodayDate(today);
        int count = 0;
        for (int i = 0; i < appointments.size() && count < maxN; i++) {
            Appointment* a = appointments.getAt(i);
            if (a->getDoctorId() == doctorId && myStrEqual(a->getDate(), today)) {
                outArr[count++] = *a;
            }
        }
        return count;
    }

    int getPatientBills(int patientId, Bill* outArr, int maxN) {
        int count = 0;
        for (int i = 0; i < bills.size() && count < maxN; i++) {
            Bill* b = bills.getAt(i);
            if (b->getPatientId() == patientId) {
                outArr[count++] = *b;
            }
        }
        return count;
    }

    // Get available slots for a doctor on a date
    // Returns slots as single string like "09:00 10:00 14:00"
    void getAvailableSlots(int doctorId, const char* date, char* outBuf) {
        const char* allSlots[8] = {
            "09:00","10:00","11:00","12:00",
            "13:00","14:00","15:00","16:00"
        };
        outBuf[0] = '\0';
        bool first = true;
        for (int i = 0; i < 8; i++) {
            if (!isSlotTaken(doctorId, date, allSlots[i])) {
                if (!first) myStrCat(outBuf, " ");
                myStrCat(outBuf, allSlots[i]);
                first = false;
            }
        }
        if (outBuf[0] == '\0') myStrCopy(outBuf, "No slots available");
    }

    // Generate daily report as a string
    void generateDailyReport(char* outBuf, int maxLen) {
        char today[20];
        getTodayDate(today);

        int total = 0, pending = 0, completed = 0, noshow = 0, cancelled = 0;
        float revenue = 0.0f;

        for (int i = 0; i < appointments.size(); i++) {
            Appointment* a = appointments.getAt(i);
            if (!myStrEqual(a->getDate(), today)) continue;
            total++;
            if (myStrEqual(a->getStatus(), "pending"))   pending++;
            if (myStrEqual(a->getStatus(), "completed")) completed++;
            if (myStrEqual(a->getStatus(), "noshow"))    noshow++;
            if (myStrEqual(a->getStatus(), "cancelled")) cancelled++;
        }

        for (int i = 0; i < bills.size(); i++) {
            Bill* b = bills.getAt(i);
            if (myStrEqual(b->getStatus(), "paid") && myStrEqual(b->getDate(), today)) {
                revenue += b->getAmount();
            }
        }

        char buf[200];
        outBuf[0] = '\0';
        myStrCat(outBuf, "=== Daily Report: ");
        myStrCat(outBuf, today);
        myStrCat(outBuf, " ===\n");

        myStrCat(outBuf, "Total appointments: ");
        myIntToStr(total, buf); myStrCat(outBuf, buf);
        myStrCat(outBuf, "\n  Pending: ");
        myIntToStr(pending, buf); myStrCat(outBuf, buf);
        myStrCat(outBuf, "  Completed: ");
        myIntToStr(completed, buf); myStrCat(outBuf, buf);
        myStrCat(outBuf, "  No-show: ");
        myIntToStr(noshow, buf); myStrCat(outBuf, buf);
        myStrCat(outBuf, "  Cancelled: ");
        myIntToStr(cancelled, buf); myStrCat(outBuf, buf);
        myStrCat(outBuf, "\n");

        myStrCat(outBuf, "Revenue collected (paid bills): PKR ");
        myFloatToStr(revenue, buf); myStrCat(outBuf, buf);
        myStrCat(outBuf, "\n");
    }

    // Get security log content
    void getSecurityLog(char* outBuf, int maxLen) {
        FileHandler::readSecurityLog(outBuf, maxLen);
    }

    // Log a security event
    void logSecurity(const char* role, const char* enteredId, const char* result) {
        FileHandler::logSecurityEvent(role, enteredId, result);
    }

    // Get doctor name by ID (for display)
    void getDoctorName(int id, char* buf) {
        Doctor* d = findDoctor(id);
        if (d) myStrCopy(buf, d->getName());
        else myStrCopy(buf, "Unknown");
    }

    // Get patient name by ID (for display)
    void getPatientName(int id, char* buf) {
        Patient* p = findPatient(id);
        if (p) myStrCopy(buf, p->getName());
        else myStrCopy(buf, "Unknown");
    }

    // Get doctor specialization
    void getDoctorSpec(int id, char* buf) {
        Doctor* d = findDoctor(id);
        if (d) myStrCopy(buf, d->getSpecialization());
        else myStrCopy(buf, "Unknown");
    }
};
