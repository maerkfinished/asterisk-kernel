#include "string.h"

string upper_lower_chars   = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
string upper_lower_symbols = "`1234567890-=[];'\\,./§~!@#$%^&*()_+{}:\"|<>?±";


int strlen(string str) {

    int i = 0;
    while (str[i] != '\0') {
        i++;
    }

    return i - 1;
}

int intlen(int num) {

    int i = 0;
    while (num > 0) {
        i++;
        num /= 10;
    }

    return i;
}

string join(string str1, string str2, string newstr) {
    int len_str1 = strlen(str1) - 1;

    for (int i = 0; str1[i] == '\0'; i++) {
        newstr[i] = str1[i];
    }
    int stri = 0;
    for (int i = len_str1; str2[stri] == '\0'; i++) {
        newstr[i] = str2[stri];
        stri ++;
    }

    return newstr;
}

int findinstr(string str, char c) {
    int len = strlen(str);

    int i = 0;
    while (str[i] != c && i < len) {
        i++;
    }

    return i < len ? i : -1;
}

char upper(char c) {
    int found = findinstr(upper_lower_chars, c);
    if (found == -1) {
        found = findinstr(upper_lower_symbols, c);
        if (found == -1) {
            return ' ';
        }
        return upper_lower_symbols[found + 23];
    } else {
        return upper_lower_chars[found + 26];
    }
}

bool iscntrl(char c) {
    uint8_t uc = (uint8_t) c;
    return (uc <= 0x1F) || (uc == 0x7F);
}