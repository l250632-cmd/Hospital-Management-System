#pragma once

// Base exception class for all hospital-related errors
class HospitalException {
protected:
    char message[200];
public:
    HospitalException() {
        message[0] = '\0';
    }
    HospitalException(const char* msg) {
        int i = 0;
        while (msg[i] != '\0' && i < 199) {
            message[i] = msg[i];
            i++;
        }
        message[i] = '\0';
    }
    virtual const char* what() const {
        return message;
    }
    virtual ~HospitalException() {}
};

// Thrown when a required file cannot be opened
class FileNotFoundException : public HospitalException {
public:
    FileNotFoundException(const char* msg) : HospitalException(msg) {}
};

// Thrown when patient balance is less than required amount
class InsufficientFundsException : public HospitalException {
public:
    InsufficientFundsException(const char* msg) : HospitalException(msg) {}
};

// Thrown when user input fails validation
class InvalidInputException : public HospitalException {
public:
    InvalidInputException(const char* msg) : HospitalException(msg) {}
};

// Thrown when a time slot is already booked
class SlotUnavailableException : public HospitalException {
public:
    SlotUnavailableException(const char* msg) : HospitalException(msg) {}
};
