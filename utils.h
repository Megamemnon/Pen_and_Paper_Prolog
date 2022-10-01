/**
 * MIT License
 *
 * Copyright (c) 2022 Brian O'Dell
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef B_UTILS
#define B_UTILS

#include <stdio.h>
#include <stdlib.h>

#define B_TIB_LENGTH 256
#define B_MAX_STRING_LENGTH 4096

/* tibIndex - an index for The Input Buffer (tib) */
extern int tibIndex;
/* tib - The Input Buffer, B_TIB_LENGTH bytes long */
extern char tib[B_TIB_LENGTH];

/* strcomp - Compare 2 strings; Returns 0 if identical otherwise first different char */
int strcomp(char *s1, char * s2); 
/* strcopy - copies chars from 'from' to 'to' until a 0 value is encountered. */
void strcopy(char *from, char *to);
/* strlength - returns number of bytes before a 0 value is encountered */
int strlength(char *s);
/* copyString - returns a copy of str */
char *copyString(char *str);
/* returns location of character c in s; 0 if not found */
int charInStr(char *str, char search);
/* strInStr - returns the first occurence of search in str */
int strInStr(char *str, char *search);
/* concat - returns a new (char *) pointint to beginning of str1 & str2 */
char *concat(char *str1, char *str2);
/* convert string to int; will return a number by ignoring all non digits in string */
int atoint(const char* s);
/* resetTib -  clears first byte of tib and sets tibIndex to 0 */
void resetTib();
/* output - hardware independent print */
void output(char *s);
/* input - hardware independent console input */
void input(void);
/* getFileSize - hardware independent file size (bytes) */
long getFileSize(const char *pathname);
/* loadMemFile - hardware independent; loads entire file into memory, 
returning a pointer to first byte; last byte is EOF */
char *loadMemFile(const char *pathname);
/* loadMemFile - hardware independent; loads entire file into memory, 
returning a pointer to first byte; last byte is EOF; 
avoids call to getFileSize */
char *loadMemFileWithSize(const char *pathname, long size);

FILE *openFile(char *pathname, char *mode);

int closeFile(FILE *f);

void outputFile(FILE *f, char *s);

#endif