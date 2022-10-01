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


#include "ppp.h"
#include "utils.h"

#define DEBUG


int continueprompt(){
  printf("\nContinue? (y/N) ");
  char buf[10];
  fgets(buf, 9, stdin);
  if(buf[0] == 'Y' || buf[0] == 'y') return 1;
  return 0;
}

int main(int argc, char const *argv[])
{
  char buf[B_MAX_STRING_LENGTH];
  int bufi = 0;

  // Initialize Globals
  KnowledgeBase = NULL;
  Query = NULL;
  Unifiers = NULL;
  Proof = NULL;

  printf("Pen & Paper Prolog\nCopyright (c) 2022 Brian O'Dell\n");

  int load;
#ifdef DEBUG
  load = loadKB("/home/brian/dev/ppp/testkb");
#else
  if(argc == 0){
    printf("usage: ppp knowledgebasefile\n");
    return 1;
  }
  load = loadKB(argv[1]);
#endif
  if(!load){
    printf("\nFile Not Found\n");
    return 1;
  }

  printf("\nKnowledge Base Loaded:\n");
  printStringlist(KnowledgeBase, 0, 100);
  printf("\n");

  while(1){
    printf("]");
    fgets(buf, B_MAX_STRING_LENGTH-1, stdin);
    bufi = strlength(buf);
    if(bufi > 0) buf[bufi-1] = '\0';

    //Quit
    if(!strcomp(buf, "quit.")) break;

    //Query
    if(buf[0] == '?' && buf[1] == '-'){
      Query = wff(buf+2);
      if(Query){
        AbortResolution = 0;
        WorkingKB = copyStringList(KnowledgeBase);
        char *unifier = resolve(Query, "{ | }",1);
        if(Unifiers){
          printf("Unifiers:\n");
          printf("Θ = %s\n", Unifiers);
          // printf("Proof:\n");
          // printStringlist(Proof);
        }
        freeChar(&Query);
        freeStringList(&Proof);
        freeChar(&unifier);
        printStringlist(WorkingKB, 0, 100);
        freeStringList(&WorkingKB);
      }
    }

    char *w = wff(buf);
    if(!w){
      printf("syntax error.\n");
    } else {
      StringList *slist = splitByControlChars(w);
      freeChar(&w);
      StringList *s = slist;

      //List
      if(!strcomp(s->entry, "list")){
        s = slist->next->next;
        if(s->entry[0]==')'){
          printStringlist(KnowledgeBase, 0, 100);
        } else {
          int start = atoint(s->entry);
          s = s->next->next;
          int count = atoint(s->entry);
          freeStringList(&slist);
          printStringlist(KnowledgeBase, start, count);
        }
      }

      //Edit
      if(!strcomp(s->entry, "edit")){ 
        s = s->next->next;
        if(s->entry[0] != ')'){
          int index = atoint(s->entry);
          printf("Enter statement to replace statement %d:\n", index);
          printStringlist(KnowledgeBase, index, 1);
          printf("\n>");
          fgets(buf, B_MAX_STRING_LENGTH-1, stdin);
          w = wff(buf);
          if(!w){
            printf("syntax error.\n");
          } else {
            if(continueprompt()){
              replaceStatement(KnowledgeBase, index, w);
            }
            putchar('\n');
            freeChar(&w);
          }
        }
      }

      //Insert
      if(!strcomp(s->entry, "insert")){ 
        s = s->next->next;
        if(s->entry[0] != ')'){
          int index = atoint(s->entry);
          printf("Enter statement to insert prior to statement %d:\n", index);
          printStringlist(KnowledgeBase, index, 1);
          printf("\n>");
          fgets(buf, B_MAX_STRING_LENGTH-1, stdin);
          w = wff(buf);
          if(!w){
            printf("syntax error.\n");
          } else {
            if(continueprompt()){
              insertStatement(&KnowledgeBase, index, w);
            }
            putchar('\n');
            freeChar(&w);
          }
        }
      }

      //Append
      if(!strcomp(s->entry, "append")){ 
        printf("Enter statement to append to KnowledgeBase:\n>");
        fgets(buf, B_MAX_STRING_LENGTH-1, stdin);
        w = wff(buf);
        if(!w){
          printf("syntax error.\n");
        } else {
          appendStatement(KnowledgeBase, w);
          freeChar(&w);
        }
      }

      //Delete
      if(!strcomp(s->entry, "delete")){
        s = s->next->next;
        if(s->entry[0] != ')'){
          int index = atoint(s->entry);
          printf("Delete: ");
          printStringlist(KnowledgeBase, index, 1);
          if(continueprompt()){
            deleteStatement(&KnowledgeBase, index);
          }
          putchar('\n');
        }
      }

    }

  }
  freeStringList(&KnowledgeBase);
  return 0;
}
