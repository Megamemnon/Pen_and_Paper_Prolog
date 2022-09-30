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
  printStringlist(KnowledgeBase);
  printf("\n");

  while(1){
    printf("]");
    fgets(buf, B_MAX_STRING_LENGTH-1, stdin);
    bufi = strlength(buf);
    if(bufi > 0) buf[bufi-1] = '\0';

    if(!strcomp(buf, "quit.")) break;

    if(!strcomp(buf, "query.")){
      while(1){
        printf("?-");
        fgets(buf, B_MAX_STRING_LENGTH-1, stdin);
        if(buf[0] != '\0' && buf[0] != '\n'){
          buf[strlength(buf)-1] = '\0';
          if(!strcomp(buf, "quit.")) break;
          Query = wff(buf);
          if(Query){
            char *unifier = resolve(Query, "{ | }",1);
            // if(!unifier){
            //   printf("No.\n");
            // }
            // if(Unifiers){
            //   printf("Unifiers:\n");
            //   printf("Θ = %s\n", Unifiers);
            //   printf("Proof:\n");
            //   printStringlist(Proof);
            //   freeChar(&Unifiers);
            // } else {
            //   printf("No Resolution found.\nΘ = {}\n");
            // }
            freeChar(&Query);
            freeStringList(&Proof);
            freeChar(&unifier);
          }
        }
      }
    }

  }
  freeStringList(&KnowledgeBase);
  return 0;
}
