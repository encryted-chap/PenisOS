#pragma once
#include "Core/IO.hpp"

class string {
public:
    string() {
        *this = "";
        Length = 0;
    }
    // creates a new string
    string(char* str) {
        buffer = str;
        UpdateLength(); 
    }
    string(char c) {
        buffer[0] = c;
        Length = 1;
    }
    string(bool b) {
        if(b) buffer = "true";
        else buffer = "false";
        UpdateLength();
    }
    bool Contains(string str) {
        string* check = this->Copy();
        while(check->Length >= str.Length) {
            if(check->StartsWith(str)) {
                return true;
            } else {
                check->CutFirst();
                continue;
            }
        }
        return false;
    }
    bool StartsWith(string str) {
        string* to_check = Copy();
        
        for(int i = 0; i < str.Length; i++) {
            if(*to_check[i] == *str[i]) {
                if(i == str.Length - 1) return true;
                else continue;
            } else break;
        }
        return false;
    }
    // returns a new instance of this string
    string* Copy() {
        string* ret;
        memcpy((const unsigned char*)this, (unsigned char*)ret, sizeof(*this)+1);
        return ret;
    }
    string Append(string app) {
        char* b = app.ToCharArray();
        string ret = *(this->Copy());

        for(int i = 0; i < app.Length; i++) {
            *ret[ret.Length++] = b[i]; 
        }
        return ret;
    }
    // clears a string, making it a new string
    void Empty(string* ToEmpty) {
        ToEmpty = &string();
    }
    // removes everything past an index
    string Remove(int index) {
        string* ret = Copy();
        for(int i = index; i < ret->Length; i++) {
            *(ret[i]) = 0; 
        } ret->Length = index;
        return *ret;
    }
    string Trim(char tc = ' ') {
        string* ret = Copy();
        *ret = TrimStart(tc);
        *ret = TrimEnd(tc);
        return *ret;
    }
    string TrimStart(char tc = ' ') {
        string* ret = Copy();
        while(*ret[0] == tc)
            *ret = ret->CutFirst();
        return *ret;
    }
    string TrimEnd(char tc = ' ') {
        string* ret = Copy();
        while(*ret[ret->Length-1] == tc) {
            *ret = ret->CutLast();
        }
        return *ret;
    }
    // removes the very first character from this string
    string CutFirst() {
        buffer++;
        string* ret = Copy();
        ret->Length--;
        buffer--;
        return *ret;
    }
    // removes the very last character from this string
    string CutLast() {
        string* ret = Copy();
        *ret[--(ret->Length)] = '\0'; 
        return *ret;
    }
    string operator += (string app) {
        return Append(app);
    }
    // convert to char pointer (does not point at the one inside the string, this has no effect on the string)
    char* ToCharArray() {
        return Copy()->buffer;
    }
    operator char*() {
        return ToCharArray();
    }
    char* operator[](int index) {
        &buffer[index];
    }
    // the non-zero based length of this string
    int Length;
private:
    void UpdateLength() {
        Length = 0;
        for(int i = 0; buffer[i] != '\0'; ++i)
            ++Length; 
    }
    char* buffer;
};