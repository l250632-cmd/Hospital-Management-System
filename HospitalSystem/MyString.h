#pragma once

// Custom string helper functions since std::string is not allowed
// and strcmp, strtok etc are also banned

// copy src into dest
void myStrCopy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// return length of string
int myStrLen(const char* s) {
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

// strip trailing \r and \n from a string in place
void myStrStrip(char* s) {
    int len = myStrLen(s);
    while (len > 0 && (s[len-1] == '\r' || s[len-1] == '\n' || s[len-1] == ' ')) {
        s[len-1] = '\0';
        len--;
    }
}

// compare two strings, return true if equal
bool myStrEqual(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        i++;
    }
    return (a[i] == '\0' && b[i] == '\0');
}

// case-insensitive compare using tolower manually
bool myStrEqualCI(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        char ca = a[i];
        char cb = b[i];
        // manual tolower
        if (ca >= 'A' && ca <= 'Z') ca = ca + 32;
        if (cb >= 'A' && cb <= 'Z') cb = cb + 32;
        if (ca != cb) return false;
        i++;
    }
    return (a[i] == '\0' && b[i] == '\0');
}

// append src to dest (dest must have enough space)
void myStrCat(char* dest, const char* src) {
    int di = myStrLen(dest);
    int si = 0;
    while (src[si] != '\0') {
        dest[di] = src[si];
        di++;
        si++;
    }
    dest[di] = '\0';
}

// convert integer to string, store in buf
void myIntToStr(int num, char* buf) {
    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    bool neg = false;
    if (num < 0) {
        neg = true;
        num = -num;
    }
    char temp[30];
    int i = 0;
    while (num > 0) {
        temp[i++] = '0' + (num % 10);
        num /= 10;
    }
    int start = 0;
    if (neg) buf[start++] = '-';
    for (int j = i - 1; j >= 0; j--) {
        buf[start++] = temp[j];
    }
    buf[start] = '\0';
}

// convert float to string with 2 decimal places
void myFloatToStr(float num, char* buf) {
    if (num < 0) {
        buf[0] = '-';
        myFloatToStr(-num, buf + 1);
        return;
    }
    int intPart = (int)num;
    int fracPart = (int)((num - intPart) * 100 + 0.5f);
    char intBuf[20];
    myIntToStr(intPart, intBuf);
    myStrCopy(buf, intBuf);
    myStrCat(buf, ".");
    // add leading zero for fractions like .05
    if (fracPart < 10) {
        myStrCat(buf, "0");
    }
    char fracBuf[10];
    myIntToStr(fracPart, fracBuf);
    myStrCat(buf, fracBuf);
}

// convert string to integer
int myStrToInt(const char* s) {
    int result = 0;
    int i = 0;
    bool neg = false;
    if (s[0] == '-') { neg = true; i = 1; }
    while (s[i] != '\0') {
        if (s[i] >= '0' && s[i] <= '9') {
            result = result * 10 + (s[i] - '0');
        }
        i++;
    }
    return neg ? -result : result;
}

// convert string to float
float myStrToFloat(const char* s) {
    float result = 0.0f;
    float frac = 0.0f;
    bool decimalFound = false;
    float divisor = 10.0f;
    bool neg = false;
    int i = 0;
    if (s[0] == '-') { neg = true; i = 1; }
    while (s[i] != '\0') {
        if (s[i] == '.') {
            decimalFound = true;
        } else if (s[i] >= '0' && s[i] <= '9') {
            if (!decimalFound) {
                result = result * 10 + (s[i] - '0');
            } else {
                frac += (s[i] - '0') / divisor;
                divisor *= 10.0f;
            }
        }
        i++;
    }
    result += frac;
    return neg ? -result : result;
}

// check if string contains only digits
bool myIsAllDigits(const char* s) {
    if (s[0] == '\0') return false;
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') return false;
    }
    return true;
}

// split a CSV line by comma, store tokens in parts array
// returns number of parts found
int splitCSV(const char* line, char parts[][300], int maxParts) {
    int count = 0;
    int pi = 0; // index in current part
    int li = 0;
    while (line[li] != '\0' && count < maxParts) {
        if (line[li] == ',') {
            // strip trailing \r from this part before moving on
            while (pi > 0 && (parts[count][pi-1] == '\r' || parts[count][pi-1] == '\n'))
                pi--;
            parts[count][pi] = '\0';
            count++;
            pi = 0;
        } else if (line[li] == '\n' || line[li] == '\r') {
            // skip newlines and carriage returns (Windows line endings)
        } else {
            parts[count][pi++] = line[li];
        }
        li++;
    }
    // strip trailing \r\n from the last field too
    while (pi > 0 && (parts[count][pi-1] == '\r' || parts[count][pi-1] == '\n'))
        pi--;
    parts[count][pi] = '\0';
    count++;
    return count;
}
