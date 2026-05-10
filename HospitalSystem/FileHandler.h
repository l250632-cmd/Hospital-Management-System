#pragma once
// Disable MSVC unsafe function warnings (fopen, localtime, etc.)
#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif
#include <cstdio>
#include <ctime>
#include "MyString.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "Storage.h"
#include "HospitalException.h"

// Only class permitted to do file I/O
class FileHandler {
public:
    // ---- Loading functions ----

    static void loadPatients(Storage<Patient>& store) {
        FILE* f = fopen("patients.txt", "r");
        if (!f) {
            // file might not exist yet - that's okay
            return;
        }
        char line[500];
        while (fgets(line, 500, f)) {
            if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;
            char parts[10][300];
            int n = splitCSV(line, parts, 10);
            if (n < 7) continue;
            for (int pi = 0; pi < n; pi++) myStrStrip(parts[pi]);
            int id = myStrToInt(parts[0]);
            int age = myStrToInt(parts[2]);
            float bal = myStrToFloat(parts[6]);
            Patient p(id, parts[1], age, parts[3], parts[4], parts[5], bal);
            store.add(p);
        }
        fclose(f);
    }

    static void loadDoctors(Storage<Doctor>& store) {
        FILE* f = fopen("doctors.txt", "r");
        if (!f) return;
        char line[500];
        while (fgets(line, 500, f)) {
            if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;
            char parts[10][300];
            int n = splitCSV(line, parts, 10);
            if (n < 6) continue;
            for (int pi = 0; pi < n; pi++) myStrStrip(parts[pi]);
            int id = myStrToInt(parts[0]);
            float fee = myStrToFloat(parts[5]);
            Doctor d(id, parts[1], parts[2], parts[3], parts[4], fee);
            store.add(d);
        }
        fclose(f);
    }

    static void loadAdmin(Admin& admin) {
        FILE* f = fopen("admin.txt", "r");
        if (!f) {
            // create default admin if file missing
            admin = Admin(1, "Admin", "admin123");
            saveAdmin(admin);
            return;
        }
        char line[300];
        if (fgets(line, 300, f)) {
            char parts[5][300];
            int n = splitCSV(line, parts, 5);
            if (n >= 3) {
                for (int pi = 0; pi < n; pi++) myStrStrip(parts[pi]);
                admin = Admin(myStrToInt(parts[0]), parts[1], parts[2]);
            }
        }
        fclose(f);
    }

    static void loadAppointments(Storage<Appointment>& store) {
        FILE* f = fopen("appointments.txt", "r");
        if (!f) return;
        char line[500];
        while (fgets(line, 500, f)) {
            if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;
            char parts[10][300];
            int n = splitCSV(line, parts, 10);
            if (n < 6) continue;
            int aid = myStrToInt(parts[0]);
            int pid = myStrToInt(parts[1]);
            int did = myStrToInt(parts[2]);
            Appointment a(aid, pid, did, parts[3], parts[4], parts[5]);
            store.add(a);
        }
        fclose(f);
    }

    static void loadBills(Storage<Bill>& store) {
        FILE* f = fopen("bills.txt", "r");
        if (!f) return;
        char line[500];
        while (fgets(line, 500, f)) {
            if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;
            char parts[10][300];
            int n = splitCSV(line, parts, 10);
            if (n < 6) continue;
            int bid = myStrToInt(parts[0]);
            int pid = myStrToInt(parts[1]);
            int aid = myStrToInt(parts[2]);
            float amt = myStrToFloat(parts[3]);
            Bill b(bid, pid, aid, amt, parts[4], parts[5]);
            store.add(b);
        }
        fclose(f);
    }

    static void loadPrescriptions(Storage<Prescription>& store) {
        FILE* f = fopen("prescriptions.txt", "r");
        if (!f) return;
        char line[1000];
        while (fgets(line, 1000, f)) {
            if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;
            char parts[10][300];
            int n = splitCSV(line, parts, 10);
            if (n < 7) continue;
            int pid = myStrToInt(parts[0]);
            int aid = myStrToInt(parts[1]);
            int patid = myStrToInt(parts[2]);
            int did = myStrToInt(parts[3]);
            Prescription pr(pid, aid, patid, did, parts[4], parts[5], parts[6]);
            store.add(pr);
        }
        fclose(f);
    }

    // ---- Save entire storage to file (rewrite) ----

    static void saveAllPatients(Storage<Patient>& store) {
        FILE* f = fopen("patients.txt", "w");
        if (!f) return;
        for (int i = 0; i < store.size(); i++) {
            Patient* p = store.getAt(i);
            char balBuf[30];
            myFloatToStr(p->getBalance(), balBuf);
            fprintf(f, "%d,%s,%d,%s,%s,%s,%s\n",
                p->getId(), p->getName(), p->getAge(),
                p->getGender(), p->getContact(),
                p->getPassword(), balBuf);
        }
        fclose(f);
    }

    static void saveAllDoctors(Storage<Doctor>& store) {
        FILE* f = fopen("doctors.txt", "w");
        if (!f) return;
        for (int i = 0; i < store.size(); i++) {
            Doctor* d = store.getAt(i);
            char feeBuf[30];
            myFloatToStr(d->getFee(), feeBuf);
            fprintf(f, "%d,%s,%s,%s,%s,%s\n",
                d->getId(), d->getName(),
                d->getSpecialization(), d->getContact(),
                d->getPassword(), feeBuf);
        }
        fclose(f);
    }

    static void saveAdmin(Admin& admin) {
        FILE* f = fopen("admin.txt", "w");
        if (!f) return;
        fprintf(f, "%d,%s,%s\n", admin.getId(), admin.getName(), admin.getPassword());
        fclose(f);
    }

    static void saveAllAppointments(Storage<Appointment>& store) {
        FILE* f = fopen("appointments.txt", "w");
        if (!f) return;
        for (int i = 0; i < store.size(); i++) {
            Appointment* a = store.getAt(i);
            fprintf(f, "%d,%d,%d,%s,%s,%s\n",
                a->getAppointmentId(), a->getPatientId(),
                a->getDoctorId(), a->getDate(),
                a->getTimeSlot(), a->getStatus());
        }
        fclose(f);
    }

    static void saveAllBills(Storage<Bill>& store) {
        FILE* f = fopen("bills.txt", "w");
        if (!f) return;
        for (int i = 0; i < store.size(); i++) {
            Bill* b = store.getAt(i);
            char amtBuf[30];
            myFloatToStr(b->getAmount(), amtBuf);
            fprintf(f, "%d,%d,%d,%s,%s,%s\n",
                b->getBillId(), b->getPatientId(),
                b->getAppointmentId(), amtBuf,
                b->getStatus(), b->getDate());
        }
        fclose(f);
    }

    static void saveAllPrescriptions(Storage<Prescription>& store) {
        FILE* f = fopen("prescriptions.txt", "w");
        if (!f) return;
        for (int i = 0; i < store.size(); i++) {
            Prescription* pr = store.getAt(i);
            fprintf(f, "%d,%d,%d,%d,%s,%s,%s\n",
                pr->getPrescriptionId(), pr->getAppointmentId(),
                pr->getPatientId(), pr->getDoctorId(),
                pr->getDate(), pr->getMedicines(), pr->getNotes());
        }
        fclose(f);
    }

    // ---- Append a single record ----

    static void appendPatient(const Patient& p) {
        FILE* f = fopen("patients.txt", "a");
        if (!f) return;
        char balBuf[30];
        myFloatToStr(p.getBalance(), balBuf);
        fprintf(f, "%d,%s,%d,%s,%s,%s,%s\n",
            p.getId(), p.getName(), p.getAge(),
            p.getGender(), p.getContact(), p.getPassword(), balBuf);
        fclose(f);
    }

    static void appendDoctor(const Doctor& d) {
        FILE* f = fopen("doctors.txt", "a");
        if (!f) return;
        char feeBuf[30];
        myFloatToStr(d.getFee(), feeBuf);
        fprintf(f, "%d,%s,%s,%s,%s,%s\n",
            d.getId(), d.getName(), d.getSpecialization(),
            d.getContact(), d.getPassword(), feeBuf);
        fclose(f);
    }

    static void appendAppointment(const Appointment& a) {
        FILE* f = fopen("appointments.txt", "a");
        if (!f) return;
        fprintf(f, "%d,%d,%d,%s,%s,%s\n",
            a.getAppointmentId(), a.getPatientId(),
            a.getDoctorId(), a.getDate(),
            a.getTimeSlot(), a.getStatus());
        fclose(f);
    }

    static void appendBill(const Bill& b) {
        FILE* f = fopen("bills.txt", "a");
        if (!f) return;
        char amtBuf[30];
        myFloatToStr(b.getAmount(), amtBuf);
        fprintf(f, "%d,%d,%d,%s,%s,%s\n",
            b.getBillId(), b.getPatientId(),
            b.getAppointmentId(), amtBuf,
            b.getStatus(), b.getDate());
        fclose(f);
    }

    static void appendPrescription(const Prescription& pr) {
        FILE* f = fopen("prescriptions.txt", "a");
        if (!f) return;
        fprintf(f, "%d,%d,%d,%d,%s,%s,%s\n",
            pr.getPrescriptionId(), pr.getAppointmentId(),
            pr.getPatientId(), pr.getDoctorId(),
            pr.getDate(), pr.getMedicines(), pr.getNotes());
        fclose(f);
    }

    // ---- Security log ----
    static void logSecurityEvent(const char* role, const char* enteredId, const char* result) {
        FILE* f = fopen("security_log.txt", "a");
        if (!f) return;
        // get timestamp
        time_t now = time(nullptr);
        char timeBuf[50];
        struct tm* t = localtime(&now);
        strftime(timeBuf, 50, "%d-%m-%Y %H:%M:%S", t);
        fprintf(f, "%s,%s,%s,%s\n", timeBuf, role, enteredId, result);
        fclose(f);
    }

    // ---- Discharge patient (copy to discharged.txt and remove) ----
    static void archivePatient(int patientId,
                                Storage<Patient>& patients,
                                Storage<Appointment>& appointments,
                                Storage<Bill>& bills,
                                Storage<Prescription>& prescriptions) {
        FILE* f = fopen("discharged.txt", "a");
        if (!f) return;

        // write patient record
        for (int i = 0; i < patients.size(); i++) {
            Patient* p = patients.getAt(i);
            if (p->getId() == patientId) {
                char balBuf[30];
                myFloatToStr(p->getBalance(), balBuf);
                fprintf(f, "%d,%s,%d,%s,%s,%s,%s\n",
                    p->getId(), p->getName(), p->getAge(),
                    p->getGender(), p->getContact(),
                    p->getPassword(), balBuf);
                break;
            }
        }

        // write their appointments
        for (int i = 0; i < appointments.size(); i++) {
            Appointment* a = appointments.getAt(i);
            if (a->getPatientId() == patientId) {
                fprintf(f, "APPT,%d,%d,%d,%s,%s,%s\n",
                    a->getAppointmentId(), a->getPatientId(),
                    a->getDoctorId(), a->getDate(),
                    a->getTimeSlot(), a->getStatus());
            }
        }

        // write their bills
        for (int i = 0; i < bills.size(); i++) {
            Bill* b = bills.getAt(i);
            if (b->getPatientId() == patientId) {
                char amtBuf[30];
                myFloatToStr(b->getAmount(), amtBuf);
                fprintf(f, "BILL,%d,%d,%d,%s,%s,%s\n",
                    b->getBillId(), b->getPatientId(),
                    b->getAppointmentId(), amtBuf,
                    b->getStatus(), b->getDate());
            }
        }

        // write their prescriptions
        for (int i = 0; i < prescriptions.size(); i++) {
            Prescription* pr = prescriptions.getAt(i);
            if (pr->getPatientId() == patientId) {
                fprintf(f, "PRESC,%d,%d,%d,%d,%s,%s,%s\n",
                    pr->getPrescriptionId(), pr->getAppointmentId(),
                    pr->getPatientId(), pr->getDoctorId(),
                    pr->getDate(), pr->getMedicines(), pr->getNotes());
            }
        }

        fclose(f);
    }

    // Read security log and return contents in a buffer
    static void readSecurityLog(char* outBuf, int maxLen) {
        FILE* f = fopen("security_log.txt", "r");
        if (!f) {
            myStrCopy(outBuf, "No security events logged.");
            return;
        }
        int pos = 0;
        int c;
        while ((c = fgetc(f)) != EOF && pos < maxLen - 1) {
            outBuf[pos++] = (char)c;
        }
        outBuf[pos] = '\0';
        fclose(f);
        if (pos == 0) myStrCopy(outBuf, "No security events logged.");
    }
};
