#pragma once

typedef unsigned char byte;
typedef unsigned long word;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long long size_t;

template<class T> class List {
public:
    bool overflow = false;
    List() {
        index = 0; // start at 0 obviously
        max = 0; // size limitation is infinite
        buffer[0] = (T)'\0'; // make sure this is set to 0 so it aint confusing
    }
    // adds a single item to the end of this list
    void Add(T item) {
        if(max != 0 && ++index > max) {
            --index;
            overflow = true;
            return;
        } else
            buffer[index] = item;
        overflow = false;
    }
    // adds multiple to this list
    void AddRange(T* values, int length) {
        for(int i = 0; i < length; i++) {
            Add(values[i]); // add the next value
            if(overflow) break; // overflows aren't pog
        }
    }
    // appends another list to this one
    void Append(List<T> values) {
        for(int i = 0; i < values.GetLength(); i++) {
            Add(*values[i]);
            if(overflow) break;
        }
    }

    // removes any matching variables, returns the amount removed
    int RemoveSpecific(T value) {
        if(!Contains(value)) return 0;
        int ret = 0;
        while(GetFirst(value) != -1) {
            RemoveAt(GetFirst(value));
            ++ret;
            if(!GetLength()) break;
        }
        return ret;
    }
    // gets the first match for this value, returns the index. returns -1 if it doesn't contain this value
    int GetFirst(T value) {
        if(!Contains(value)) return -1;
        else {
            for(int i = 0; i < GetLength(); i++) {
                if(buffer[i] == value)
                    return i;
            }
            return -1; // if it somehow doesn't contain this, return -1
        }
    }
    // empties this list, resetting its index to 0 and clearing its entries
    void Empty() {
        while(GetLength()) {
            RemoveLast(); // remove a single entry
        }
    }
    // removes the topmost value in the list
    void RemoveLast() {
        buffer[--index] = (T)'\0'; // remove last and update index
    }
    // use like: *listvar[0] = value
    T* operator [] (int index) {
        return &buffer[index];
    }
    // removes a single item from this list
    void RemoveAt(int index) {
        if(max != 0 && index > max) {
            overflow = true;
            return; // overflows aren't pog
        }
            
        buffer[index] = (T)'\0'; // removes the item
        // make all the current items move down one to replace the removed one
        for(; index < this->index; index++) {
            buffer[index] = buffer[index+1];
        } 
        buffer[this->index--] = (T)'\0';
    }
    // removes a specified amount from an index
    void RemoveRange(int index, int count) {
        for(int i = 0; i < count; i++) {
            RemoveAt(index);
            if(overflow) break; // overflows aren't pog
        }
    }
    // returns true if the list contains this item
    bool Contains(T item) {
        for(int i = 0; i < GetLength(); i++) {
            if(item == buffer[i]) return true;
        }
        return false;
    }
    // turns this list into a X pointer with X being the type specified for this list
    T* ToArray() {
        T* newbuffer;
        for(int i = 0; i < GetLength(); i++) {
            newbuffer[i] = buffer[i];
        }
        return newbuffer;
    }
    // returns the non-zero based size of this list
    int GetLength() {return this->index;}
    // sets the maximum value for this array (0 = infinite)
    void SetMax(int maximum) {
        max = maximum;
        if(index > max) {
            // make sure to cut back to avoid data overflow
            for(; index != max; --index) {
                buffer[index] = (T)'\0'; // clear spot
            }
        } else return;
    }
    
private:
    int index;
    int max;
    T* buffer;
};
template<class T> class DynamicBitArray {
public:
    DynamicBitArray(T val) {
        DynVal = val;
        maximum = sizeof(T)*8; // sizeof = byte size, 8 bits in a byte
        UpdateBits(); // make sure to update
    }
    // updates the dynamic value based on the bits inside the byte
    void UpdateVal() {
        DynVal = (T)0;
        for(int i = 0; i < maximum; i++) {
            if(b[i])
                DynVal |= (1 << i);
            else continue;
        }
    }
    operator T() {
        UpdateVal();
        return DynVal;
    }
    // updates all the bits
    void UpdateBits() {
        for(int i = 0; i < maximum; i++) {
            b[i] = (DynVal & (1 << i)) != 0;
        }
    }
    bool* operator [] (int index) {
        return &b[index];
    }
    T DynVal;
private:
    int maximum;
    bool b[sizeof(T)*8];
};
class BitArray {
public:
    BitArray(byte val) {
        ByteValue = val;
    }
    operator byte() {
        // constructs a byte from 8 bits
        UpdateByte();
        return ByteValue;
    }
    bool* operator[] (int index) {
        return &b[index];
    }
    byte GetByte() {
        return (byte)*this;
    }
    byte operator=(byte other) {
        this->ByteValue = other;
        UpdateBits();
    }
    void UpdateBits() {
        for (int i=0; i < 8; ++i)
            b[i] = (ByteValue & (1<<i)) != 0;
        
    }
    byte ByteValue;

    void UpdateByte() {
        ByteValue = 0;
        for(int i = 0; i < 8; i++) {
            // if this = 1, or it accordingly
            if(b[i]) {
                ByteValue |= (1 << i); // shift to the correct position then OR
            }
        }
    }

private:
    bool b[8];
};
