#pragma once
#include "Core/IO.hpp"

class string {
public:
    // creates a new string
    string(char* str) {
        Length = 0;
        buffer = str;
        for(int i = 0; str[i] != '\0'; i++)
            Length++;
    }
    // appends a string to this one
    string operator += (char* other) {
        for(int i = 0; other[i] != '\0'; i++) {
            buffer[Length++] = other[i];
        }
        return *this;
    }
    // returns a new instance of this string
    string Copy() {
        // generate a buffer made out of the raw bytes of this class instance
        char* Data = (char*)*this;
        char ret[sizeof(*this)]; // the buffer to return 
        for(int i = 0; i < sizeof(*this); i++) {
            ret[i] = Data[i];
        }
        return (string)ret; // returns the raw data cast to a new string
    }
    // returns a new instance of this string with everything removed after
    // zero-based index
    string Remove(int index) {
        string ToAlter = Copy(); // create new instance
        for(int i = index; i < Length; i++) {
            *ToAlter[i] = '\0';
            --Length;
        }
        return ToAlter;
    }
    // Returns true if this string contains another string's value
    bool Contains(string otherval) {
        int s_index = 0;
        for(int i = 0; i < Length; i++) {
            if(*otherval[s_index] == *this[i]) {
                int x = i+1;
                for(; s_index < otherval.Length; ++s_index) {
                    if(*otherval[s_index] != *this[i]) {
                        i = x;
                        break;
                    } else if(s_index == otherval.Length-1) return true; 
                }
                i = x;
                continue;
            }
        }
        return false;
    }
    // returns true if this string contains this char
    bool Contains(char val) {
        for(int i = 0; i < Length; i++) {
            if(*this[i] == val) 
                return true;
            else continue;
        }
        return false;
    }
    // returns the zero based index of an instance of a specified char
    int IndexOf(char val) {
        for(int i = 0; i < Length; i++) {
            if(*this[i] == val) 
                return i;
            else continue;
        }
        return -1;
    }
    int IndexOf(string val) {

    }
    // returns a new instance of this string's buffer
    operator char*() {
        char nbuff[Length]; // define new buffer with specified length
        for(int i = 0; i < Length; i++) 
            nbuff[i] = buffer[i];
        return nbuff;
    }
    int Length;
    char* operator[](int index) {
        if(index >= Length) return "";
        else return &buffer[index];
    }
private:
    char* buffer;
};