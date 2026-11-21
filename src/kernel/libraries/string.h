#ifndef string_h
#define string_h

#include <stdint.h>
typedef char *string;


int strlen(string str);
string hextostr(uint8_t hex);
int intlen(int num);
string join(string str1, string str2, string newstr);
int findinstr(string str, char c);
char upper(char c);
bool iscntrl(char c);

#endif