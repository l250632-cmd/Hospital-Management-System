#pragma once
#include "MyString.h"

class Prescription {
private:
    int prescriptionId;
    int appointmentId;
    int patientId;
    int doctorId;
    char date[20];
    char medicines[500];
    char notes[300];

public:
    Prescription() {
        prescriptionId = 0;
        appointmentId = 0;
        patientId = 0;
        doctorId = 0;
        date[0] = '\0';
        medicines[0] = '\0';
        notes[0] = '\0';
    }

    Prescription(int pid, int aid, int patid, int did,
                 const char* dt, const char* med, const char* nt) {
        prescriptionId = pid;
        appointmentId = aid;
        patientId = patid;
        doctorId = did;
        myStrCopy(date, dt);
        myStrCopy(medicines, med);
        myStrCopy(notes, nt);
    }

    // Getters
    int getPrescriptionId() const { return prescriptionId; }
    int getAppointmentId() const { return appointmentId; }
    int getPatientId() const { return patientId; }
    int getDoctorId() const { return doctorId; }
    const char* getDate() const { return date; }
    const char* getMedicines() const { return medicines; }
    const char* getNotes() const { return notes; }

    // Setters
    void setPrescriptionId(int id) { prescriptionId = id; }
    void setAppointmentId(int id) { appointmentId = id; }
    void setPatientId(int id) { patientId = id; }
    void setDoctorId(int id) { doctorId = id; }
    void setDate(const char* d) { myStrCopy(date, d); }
    void setMedicines(const char* m) { myStrCopy(medicines, m); }
    void setNotes(const char* n) { myStrCopy(notes, n); }
};
