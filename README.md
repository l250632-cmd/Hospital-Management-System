# 🏥 MediCore Hospital Management System

<div align="center">

![C++](https://img.shields.io/badge/Language-C++-blue?style=for-the-badge&logo=cplusplus)
![SFML](https://img.shields.io/badge/GUI-SFML%202.6-green?style=for-the-badge)
![OOP](https://img.shields.io/badge/Paradigm-OOP-gold?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Complete-brightgreen?style=for-the-badge)

**A fully functional Hospital Management System engineered in C++ with a custom SFML GUI.**  
*Three user roles · File-based persistence · Custom string engine · Complete exception handling*

[Features](#-features) · [Getting Started](#-getting-started) · [Architecture](#-architecture) · [OOP Concepts](#-oop-concepts-used)

</div>

---

## 📌 Overview

MediCore is a complete, multi-role hospital management system built entirely from scratch in C++ with an SFML graphical interface.

The backend is architected using core OOP principles: abstract base classes, polymorphism, operator overloading, dynamic memory management, template classes, and file-based persistence. All string operations are handled through a custom `MyString.h` engine — `std::string` is not used anywhere in the project.

Built as an individual project for the Object Oriented Programming course at FAST NUCES, Spring 2026, Section BCS-2G.

---

## ✨ Features

### 🧑‍⚕️ Patient Portal
| Feature | Description |
|---|---|
| Book Appointment | Search doctors by specialization, pick a date and time slot, balance is deducted automatically |
| Cancel Appointment | Cancel any pending appointment and receive a full refund |
| View Appointments | All appointments sorted by date with doctor and status details |
| View Medical Records | Prescriptions grouped by appointment, sorted most recent first |
| View Bills | All bills with outstanding total displayed at the bottom |
| Pay Bill | Pay any unpaid bill directly from wallet balance |
| Top Up Balance | Add funds to your patient wallet |

### 👨‍⚕️ Doctor Portal
| Feature | Description |
|---|---|
| Today's Appointments | View all appointments scheduled for today sorted by time slot |
| Mark Complete | Mark a pending appointment as completed |
| Mark No-Show | Mark a patient as no-show; bill is cancelled, no refund issued |
| Write Prescription | Add medicines and notes for any completed appointment |
| Patient History | View full prescription history for your own patients |

### 🔐 Admin Portal
| Feature | Description |
|---|---|
| Add Doctor | Register a new doctor with validated credentials and consultation fee |
| Remove Doctor | Remove a doctor only if they have no pending appointments |
| View All Patients | Full patient list with unpaid bill counts |
| View All Doctors | Complete doctor directory |
| View All Appointments | All appointments sorted by date descending |
| View Unpaid Bills | All outstanding bills system-wide with overdue flagging |
| Discharge Patient | Archive and remove a patient with all their records |
| Security Log | View all failed login attempts |
| Daily Report | Live report — appointments, revenue, outstanding bills, doctor summaries |

---

## 🏗 Architecture

```
HospitalSystem/
├── main.cpp                  ← SFML GUI entry point
├── HospitalSystem.h          ← Core logic and menu orchestration
├── Person.h                  ← Abstract base class (pure virtual)
├── Patient.h                 ← Patient class (+=, -=, ==, << overloaded)
├── Doctor.h                  ← Doctor class (==, << overloaded)
├── Admin.h                   ← Admin class
├── Appointment.h             ← Appointment class (== conflict, << overloaded)
├── Bill.h                    ← Bill class
├── Prescription.h            ← Prescription class
├── Storage.h                 ← Generic template storage class T data[100]
├── FileHandler.h             ← All file I/O (load, append, update, delete)
├── Validator.h               ← All input validation logic
├── HospitalException.h       ← Custom exception hierarchy
├── MyString.h                ← Custom string utilities (no std::string)
├── Makefile                  ← Linux/Mac build
├── CMakeLists.txt            ← CMake build
└── *.txt                     ← Data files (patients, doctors, appointments, etc.)
```

### Class Hierarchy
```
Person  (abstract)
├── Patient    →  +=, -=, ==, <<
├── Doctor     →  ==, <<
└── Admin

HospitalException  (abstract)
├── FileNotFoundException
├── InsufficientFundsException
├── InvalidInputException
└── SlotUnavailableException

Storage<T>     →  owns  →  T data[100]
FileHandler    →  persists  →  everything
Validator      →  validates  →  all user input
```

---

## 🚀 Getting Started

### Prerequisites
- C++17 compiler (g++ or MSVC)
- SFML 2.6.2 — [Download here](https://www.sfml-dev.org/download.php)

### Linux / Ubuntu
1. Install SFML:
```bash
sudo apt update
sudo apt install libsfml-dev
```
2. Compile using the Makefile:
```bash
make
./MediCore
```
Or manually:
```bash
g++ -std=c++17 -o MediCore main.cpp -lsfml-graphics -lsfml-window -lsfml-system
./MediCore
```

### Windows (MinGW + SFML 2.6.2)
1. Download SFML 2.6.2 for MinGW from https://www.sfml-dev.org
2. Extract to `C:\SFML-2.6.2`
3. Compile:
```bash
g++ -std=c++17 -IC:\SFML-2.6.2\include -o MediCore.exe main.cpp -LC:\SFML-2.6.2\lib -lsfml-graphics -lsfml-window -lsfml-system -mwindows
```
4. Copy all `.dll` files from `C:\SFML-2.6.2\bin\` to the same folder as `MediCore.exe`

### Using CMake
```bash
mkdir build && cd build
cmake .. -DSFML_DIR=/path/to/SFML-2.6.2/lib/cmake/SFML
make
```

### Data Files
Place these files in the same directory as the executable:

| File | Purpose |
|---|---|
| `patients.txt` | Patient records |
| `doctors.txt` | Doctor records |
| `admin.txt` | Admin credentials |
| `appointments.txt` | Appointment records |
| `bills.txt` | Billing records |
| `prescriptions.txt` | Prescription records |
| `security_log.txt` | Failed login attempts |
| `discharged.txt` | Discharged patient archive |

### Default Login Credentials
```
Role      ID    Password
Patient    1    pass123
Patient    2    pass456
Doctor     1    doc456
Doctor     2    doc789
Admin      1    admin123
```

---

## 🔑 OOP Concepts Used

| Concept | Where Applied |
|---|---|
| Abstract Class | `Person` — pure virtual `displayMenu()` and `display()` |
| Inheritance | `Patient`, `Doctor`, `Admin` extend `Person` |
| Polymorphism | `Person*` dispatches correct menu at runtime |
| Operator Overloading | `+=`, `-=`, `==`, `<<` on `Patient`, `Doctor`, `Appointment` |
| Templates | `Storage<T>` — generic container for all entity types |
| Exception Handling | Full custom exception hierarchy from `HospitalException` |
| Encapsulation | All private members accessed through methods |
| Dynamic Memory | All allocations via `new`/`delete` — no static arrays |
| File I/O | `FileHandler` — sole class responsible for all persistence |
| Input Validation | `Validator` — sole class responsible for all validation logic |
| Custom Strings | `MyString.h` — no `std::string`, `strcmp`, or `strtok` anywhere |

---

## 📄 License

This project was built for academic purposes as part of the Object Oriented Programming course at FAST NUCES, Spring 2026, Section BCS-2A.

---

<div align="center">
<b>MediCore Hospital Management System</b><br>
FAST NUCES · BCS-2A · OOP · Spring 2026
</div>
