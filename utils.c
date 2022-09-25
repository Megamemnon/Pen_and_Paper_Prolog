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

#include "utils.h"

char tib[B_TIB_LENGTH];
int tibIndex;

int strcomp(char *s1, char * s2){
  for(int i=0; i<B_MAX_STRING_LENGTH; i++){
    if(s1[i]!=s2[i]) return i+1;
    if(s1[i]==0) return 0;
  }
  return 0;
}

void strcopy(char *from, char *to){
  if(!from) return;
  for(int i=0; i<B_MAX_STRING_LENGTH; i++){
    if(from[i]!=0){
      to[i]=from[i];
    }
    else{
      to[i]=0;
      break;
    }
  }
}

int strlength(char *s){
  if(!s) return 0;
  int c = 0;
  while(s[c]!=0){
    c++;
  }
  return c;
}

char *copyString(char *str){
  char *newstr = malloc(strlength(str)+1);
  strcopy(str, newstr);
  return newstr;
}

int charInStr(char *str, char search){
  int length = strlength(str);
  for(int i = 0; i<length; i++){
    if(str[i] == search) return i+1;
  }
  return 0;
}

int strInStr(char *str, char *search){
  if(!str) return 0;
  int length = strlength(str);
  int searchlen = strlength(search);
  for(int i = 0; i<length; i++){
    if(str[i] == search[0]){
      int x = i+1;
      for(int j = 1; j<searchlen; j++){
        if(str[x++] != search[j]) break;
        if(j == searchlen - 1) return i +1;
      }
    }
  }
  return 0;
}

char *concat(char *str1, char *str2){
  char buf[B_MAX_STRING_LENGTH];
  strcopy(str1, buf);
  strcopy(str2, buf + strlength(str1));
  char *newstr = malloc(strlength(buf)+1);
  strcopy(buf, newstr);
  return newstr;
}

int atoi(const char* s){
    int num = 0;
    int i = 0;
    int negative = 0;
    if(s[i] == '-'){
        negative = 1;
        i++;
    }
    while (s[i] && (s[i] >= '0' && s[i] <= '9')){
        num = num * 10 + (s[i] - '0');
        i++;
    }
    if(negative) num = -1 * num;
    return num;
}

void resetTib(){
  tibIndex=0;
  tib[tibIndex]=0;
}

void output(char *s){
  fputs(s, stdout);
  fflush(stdout);
}

void input(void){
  int i;
	resetTib();
	fgets(tib, B_TIB_LENGTH, stdin);
	for(i=0;i<B_TIB_LENGTH; i++){
		if(tib[i]=='\n')
			tib[i]=0;
	}
}

long getFileSize(const char *pathname){
  long sz;
  FILE *f;

  f = fopen(pathname, "r");
  if(f == NULL) return 0;
  fseek(f, 0L, SEEK_END);
  sz = ftell(f);
  fclose(f);
  return sz;
}

char *loadMemFile(const char *pathname){
  char c;
  long sz = getFileSize(pathname);
  long i = 0;
  char *memfile = malloc(sz+1);
  if(memfile == NULL) return NULL;
  FILE *f = fopen(pathname, "r");
  if(f == NULL) return NULL;
  do
  {
    c = fgetc(f);
    memfile[i++] = c;
  } while (c != EOF);
  
  fclose(f);
  return memfile;
}

char *loadMemFileWithSize(const char *pathname, long size){
  char c;
  long i = 0;
  char *memfile = malloc(size+1);
  if(memfile == NULL) return NULL;
  FILE *f = fopen(pathname, "r");
  if(f == NULL) return NULL;
  do
  {
    c = fgetc(f);
    memfile[i++] = c;
  } while (c != EOF);
  
  fclose(f);
  return memfile;
}

FILE *openFile(char *pathname, char *mode){
  FILE *f = fopen(pathname, mode);
  return f;
}

void outputFile(FILE *f, char *s){
  fprintf(f, "%s", s);
}

int closeFile(FILE *f){
  return fclose(f);
}