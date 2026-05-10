#pragma once

// Generic storage class using static array
// No vectors allowed as per project rules
template <typename T>
class Storage {
private:
    T data[100];
    int count;

public:
    Storage() {
        count = 0;
    }

    // Add an element
    bool add(const T& item) {
        if (count >= 100) return false;
        data[count] = item;
        count++;
        return true;
    }

    // Remove by index
    bool removeAt(int index) {
        if (index < 0 || index >= count) return false;
        // shift elements left
        for (int i = index; i < count - 1; i++) {
            data[i] = data[i + 1];
        }
        count--;
        return true;
    }

    // Get element by index
    T* getAt(int index) {
        if (index < 0 || index >= count) return nullptr;
        return &data[index];
    }

    // Get count of elements
    int size() const {
        return count;
    }

    // Clear all
    void clear() {
        count = 0;
    }

    // Get pointer to raw data for iteration
    T* getData() {
        return data;
    }
};
