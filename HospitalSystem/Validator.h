#pragma once
#include "MyString.h"

// Only class allowed to do input validation
class Validator {
public:
    // Validate date DD-MM-YYYY format
    // Day 01-31, month 01-12, year >= current year
    static bool isValidDate(const char* date, int currentYear) {
        // Must be exactly 10 chars: DD-MM-YYYY
        if (myStrLen(date) != 10) return false;
        if (date[2] != '-' || date[5] != '-') return false;

        // Extract parts manually
        char dayBuf[3], monBuf[3], yrBuf[5];
        dayBuf[0] = date[0]; dayBuf[1] = date[1]; dayBuf[2] = '\0';
        monBuf[0] = date[3]; monBuf[1] = date[4]; monBuf[2] = '\0';
        yrBuf[0] = date[6]; yrBuf[1] = date[7];
        yrBuf[2] = date[8]; yrBuf[3] = date[9]; yrBuf[4] = '\0';

        if (!myIsAllDigits(dayBuf)) return false;
        if (!myIsAllDigits(monBuf)) return false;
        if (!myIsAllDigits(yrBuf)) return false;

        int day = myStrToInt(dayBuf);
        int mon = myStrToInt(monBuf);
        int yr = myStrToInt(yrBuf);

        if (day < 1 || day > 31) return false;
        if (mon < 1 || mon > 12) return false;
        if (yr < currentYear) return false;

        return true;
    }

    // Validate time slot - must be one of 8 fixed slots
    static bool isValidTimeSlot(const char* slot) {
        const char* validSlots[8] = {
            "09:00", "10:00", "11:00", "12:00",
            "13:00", "14:00", "15:00", "16:00"
        };
        for (int i = 0; i < 8; i++) {
            if (myStrEqual(slot, validSlots[i])) return true;
        }
        return false;
    }

    // Validate contact - must be exactly 11 digits
    static bool isValidContact(const char* contact) {
        if (myStrLen(contact) != 11) return false;
        return myIsAllDigits(contact);
    }

    // Validate password - min 6 characters
    static bool isValidPassword(const char* pass) {
        return myStrLen(pass) >= 6;
    }

    // Validate positive float > 0
    static bool isPositiveFloat(const char* s) {
        if (s[0] == '\0') return false;
        bool hasDecimal = false;
        int i = 0;
        if (s[0] == '-') return false; // no negatives
        while (s[i] != '\0') {
            if (s[i] == '.') {
                if (hasDecimal) return false;
                hasDecimal = true;
            } else if (s[i] < '0' || s[i] > '9') {
                return false;
            }
            i++;
        }
        float val = myStrToFloat(s);
        return val > 0.0f;
    }

    // Validate menu choice in range 1..max
    static bool isValidMenuChoice(int choice, int max) {
        return choice >= 1 && choice <= max;
    }

    // Validate positive integer
    static bool isPositiveInt(const char* s) {
        if (!myIsAllDigits(s)) return false;
        return myStrToInt(s) > 0;
    }
};
