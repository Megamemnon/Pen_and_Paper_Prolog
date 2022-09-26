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

/**
 * Pen & Paper Prolog
 * Copyright (c) 2022 Brian O'Dell
 * 
 * Version 0.1 
 *    - Unifier and Resolution algorithms implemented on Strings (no ASTs)
 *    - local buffers max size 4K
 *    - loop for user-entered query; "quit." to exit
 *    - tested with Mary Likes Wine, Implication, and Ackermann algorithm
 *    - Manual Garbage Collection fully implemented; verified with Valgrind
 */


#include <stdio.h>
#include <ctype.h>

#include "utils.h"

#define DEBUG

typedef enum 
{
  TTVARIABLE, TTATOM, TTFUNCTOR, TTCONJUNCTION, TTCLAUSE, TTCONTROLCHAR
}TermType;

typedef struct STRING_LIST{
  char *entry;
  struct STRING_LIST *next;
} StringList;

StringList *KnowledgeBase;
char *Query;
char *Unifiers;
StringList *Proof;

int isControlChar(char c){
  return (c == '(' || c == ')' || c == ',' || c == ':' || 
    c == '.' || c == '|' || c == '{' || c == '}');
}

void freeChar(char **charptr){
  if(!(* charptr)) return;
  free((* charptr));
  (* charptr) = NULL;
}

void freeStringList(StringList **list){
  StringList *next = NULL;
  while(* list){
    next = (* list)->next;
    freeChar(&(* list)->entry);
    free(* list);
    (* list) = next;
  }
  (* list) = NULL;
}

StringList *splitByControlChars(char *str){
  if(!str) return NULL;
  StringList *t1 = NULL;
  StringList *tn = NULL;
  StringList *tp = NULL;
  char buf[B_MAX_STRING_LENGTH];
  buf[0] = '\0'; 
  buf[1] = '\0'; 
  int bufi = 0;
  int length = strlength(str);
 for(int i = 0; i<length; i++){
    char c = str[i];
    buf[bufi++] = c;
    if(isControlChar(c)){
      if(buf[1] == '\0' && bufi == 1){
        // add this control character to string list
        if(c == ':' && str[i+1] == '-'){
          i++;
          buf[bufi++] = '-';
        } 
        buf[bufi] = '\0';
      } else {
        // add a variable, atom, or functor identifier to string list
        i--;
        buf[bufi - 1] = '\0';
      }
      if(!t1){
        t1 = malloc(sizeof(StringList));
        t1->entry = NULL;
        t1->next = NULL;
        tn = t1;
      }
      tn->entry = malloc(strlength(buf)+1);
      strcopy(buf, tn->entry);
      tp = tn;
      tn = malloc(sizeof(StringList));
      tn->entry = NULL;
      tn->next = NULL;
      tp->next = tn;
      bufi = 0;  
      buf[0] = '\0'; 
      buf[1] = '\0'; 
    }
  }
  tp->next = NULL;
  freeStringList(&tn);
  return t1;
}

char *joinStringList(StringList *list){
  char buf[B_MAX_STRING_LENGTH];
  int bufi = 0;
  int length = 0;
  while(list){
    length = strlength(list->entry);
    if(bufi + length >= B_MAX_STRING_LENGTH) return NULL;
    strcopy(list->entry, buf + bufi);
    bufi += length;
    list = list->next;
  }
  char *str = malloc(bufi+1);
  strcopy(buf, str);
  return str;
}

void printStringlist(StringList *list){
  while(list){
    if(list->entry) printf("%s\n", list->entry);
    list = list->next;
  }
}

StringList *splitUnifier(char *unifier){
  if(!unifier) return NULL;
  StringList *t1 = NULL;
  StringList *tn = NULL;
  StringList *tp = NULL;
  char buf[B_MAX_STRING_LENGTH];
  int bufi = 0;
  char c = unifier[0];
  if(c != '{') return NULL;
  int length = strlength(unifier);
 for(int i = 0; i<length; i++){
    c = unifier[i];
    buf[bufi++] = c;
    if(c == '}'){
      if(!t1){
        t1 = malloc(sizeof(StringList));
        t1->entry = NULL;
        t1->next = NULL;
        tn = t1;
      }
      buf[bufi] = '\0';
      tn->entry = malloc(strlength(buf)+1);
      strcopy(buf, tn->entry);
      tp = tn;
      tn = malloc(sizeof(StringList));
      tn->entry = NULL;
      tn->next = NULL;
      tp->next = tn;
      bufi = 0;   
    }
  }
  freeStringList(&tp->next);
  tp->next = NULL;
  return t1;
}

char *firstTerm(char *term){
  int length = strlength(term)+1;
  char *first = malloc(length+1);
  strcopy(term, first);
  int paren = 0;
  for(int index = 1; index<length; index++){
    char c = first[index];
    if(c == '(') paren++;
    if(c == ')') paren--;
    if(!paren){
      if(c == ',' || c == '.'){
        first[index] = '\0';
        return first;
      }
    }
  }
  return first;
}

// 
char *restTerm(char *term, char *firstTerm){
  int length = strlength(term) + 1;
  int lengthfirst = strlength(firstTerm);
  // if(term[length - 2] == '.') return NULL;
  if(length - lengthfirst == 0) return NULL;
  char *rest = malloc(length - lengthfirst+1);
  strcopy(term+lengthfirst+1, rest);
  if(rest[0] == '\0'){
    freeChar(&rest); 
    return NULL;
  }
  return rest;
}

/* head - returns head of clause or full clause if no head;
 * no :- permitted in head of clause */
char *head(char *clause){
  char *head = clause;
  int index = 0;
  int length = strlength(clause) + 1;
  while(index<length){
    if(clause[index] == ':' && clause[index + 1] == '-'){
      head = malloc(index+1);
      for(int i = 0; i<=index; i++){
        head[i] = clause[i];
      }
      head[index] = '\0';
      return head;
    }
    index++;
  }
  head = malloc(length +1);
  strcopy(clause, head);
  return head;
}

/* body - returns body of clause or NULL;
 * no :- permitted in head of clause */
char *body(char *clause){
  char *body = NULL;
  int index = 0;
  int length = strlength(clause) + 1;
  while(index<length){
    if(clause[index] == ':' && clause[index + 1] == '-'){
      body = malloc(length - (index) +1);//(index + 2)
      for(int i = 0; i<length - (index + 2); i++){//(index + 2)
        body[i] = clause[i + index + 2];//(index + 2)//TODO clause index overrun
      }
      break;
    }
    index++;
  }
  return body;
}

/* sff - returns 1 if clause is well formed formula; 0 otherwise */
char *wff(char *clause){
  int length = strlength(clause) + 1;
  char *newClause = malloc(length+1);
  int index = 0;
  int newIndex = 0;
  while(index<length){
    char c = clause[index++];
    if(!isspace(c)){
      newClause[newIndex++] = c;
    }
  }
  newClause[newIndex] = '\0';
  return newClause;
}

int arity(char *term){
  if(!term) return 0;
  int length = strlength(term) + 1;
  int paren = 0;
  int arity = 0;
  int index = 0;
  char c = term[index++];
  while(!isControlChar(c)){
    c = term[index++];
  }
  if(c != '(') return 0;
  arity++;
  paren = 1;
  while(paren != 0){
    c = term[index++];
    if(c == '(') paren++;
    if(c == ')') paren--;
    if(paren == 1 && c == ',') arity++;
  }
  return arity;
}

char *getOp(char *term){
  char buf[B_MAX_STRING_LENGTH];
  int index = 0;
  char c = term[index];
  while(!isControlChar(c)){
    buf[index++] = c;
    c = term[index];
  }
  buf[index] = '\0';
  char *op = malloc(strlength(buf)+1);
  strcopy(buf, op);
  return op;
}

char *getArgs(char *term){
  char buf[B_MAX_STRING_LENGTH];
  int bufindex = 0;
  buf[bufindex]='\0';
  int index = 0;
  int paren = 1;
  char c = term[index++];
  while(c != '('){
    c = term[index++];
  }
  while(paren != 0){
    c = term[index++];
    if(c == '(') paren++;
    if(c == ')') paren--;
    buf[bufindex++] = c;
  }
  buf[bufindex - 1] = '\0';
  char *args = malloc(strlength(buf)+1);
  strcopy(buf, args);
  return args;
}

TermType type(char *term){
  int paren = 0;
  int conjunctions = 0;
  int foundparen = 0;
  int functor = 0;
  int length = strlength(term);
  for(int index = 0; index<length; index++){
    char c = term[index];
    if(c == '('){
      paren++;
      foundparen++;
    } 
    if(c == ')') paren--;
    if(conjunctions == 0 && paren == 0 && c == ':' && term[index + 1] == '-') return TTCLAUSE;
    if(paren == 0 && c == ',') return TTCONJUNCTION;
  }
  if(foundparen) return TTFUNCTOR;
  if(isupper(term[0])) return TTVARIABLE;
  return TTATOM;
}

TermType typeStringListEntry(StringList *list){
  if(isControlChar(list->entry[0])) return TTCONTROLCHAR;
  return type(list->entry);
}

/* returns term bound to variable var */
char * getBound(char *var, char *unifier){
  if(!var || !unifier) return NULL;
  StringList *list = splitByControlChars(unifier);
  StringList *l1 = list;
  char buf[B_MAX_STRING_LENGTH];
  int bufi = 0;
  int priorcurly = 0;
  while(l1){
    TermType typ = typeStringListEntry(l1);
    if(typ == TTVARIABLE && priorcurly){
      if(!strcomp(var, l1->entry)){
        l1 = l1->next->next;
        while(l1){
          if(l1->entry[0] == '}'){
            char *bound = malloc(strlength(buf)+1);
            strcopy(buf, bound);
            freeStringList(&list);
            return bound;
          } else {
            strcopy(l1->entry, buf + bufi);
            bufi += strlength(l1->entry);
            l1 = l1->next;
          }
        }
      }
    }
    priorcurly = l1->entry[0] =='{' ? 1 : 0;
    l1 = l1->next;
  }
  freeStringList(&list);
  return NULL;
}

char *compose(char *origunifier, char *newunifier){
  if(!origunifier || !newunifier) {
    if(origunifier) return copyString(origunifier);
    return copyString(newunifier);
  }
  if(!strcomp(origunifier, newunifier)) return copyString(origunifier);
  if(!strcomp(origunifier, "{ | }")) return copyString(newunifier);
  if(!strcomp(newunifier, "{ | }")) return copyString(origunifier);
  StringList *list = splitByControlChars(origunifier);
  StringList *l1 = list;
  StringList *lp = NULL;
  char buf[B_MAX_STRING_LENGTH];
  int bufi = 0;
  int priorcurly = 0;
  // replace any origunifier variables matched 
  // in newunifier with their bound value
  while(l1){
    TermType typ = typeStringListEntry(l1);
    if(typ == TTVARIABLE && !priorcurly){
      char *bound = getBound(l1->entry, newunifier);
      if(bound){
        freeChar(&l1->entry);
        l1->entry = malloc(strlength(bound)+1);
        strcopy(bound, l1->entry);
        freeChar(&bound);
      }
    }
    priorcurly = l1->entry[0] =='{' ? 1 : 0;
    lp = l1;
    l1 = l1->next;
  }
  // append everything in newunifier to the end
  // of origunifier (except blank "{ | }")
  StringList *nu = splitUnifier(newunifier);
  StringList *nu2 = nu;
  while(nu2){
    if(strcomp(nu2->entry,"{ | }")){
      lp->next = malloc(sizeof(StringList));
      lp = lp->next;
      lp->entry = NULL;
      lp->next = NULL;
      lp->entry = malloc(strlength(nu2->entry)+1);
      strcopy(nu2->entry, lp->entry);
    } 
    nu2 = nu2->next;
  }
  char *compos = joinStringList(list);
  freeStringList(&list);
  freeStringList(&nu);  
  // remove any duplicate entries and remove 
  // all blanks "{ | }"
  list = splitUnifier(compos);
  freeChar(&compos);
  lp = list;
  nu = lp;
  while(lp){
    if(!strcomp(lp->entry, "{ | }") && lp->next){
      l1 = lp->next;
      lp->next = NULL;
      freeStringList(&lp);
      lp = l1;
      if(nu == list){
        list = lp;
        nu = lp;
      } else {
        nu->next = lp;
      }
    }
    l1 = lp->next;
    StringList *l1p = lp;
    while(l1){
      if(!strcomp(lp->entry, l1->entry)){
        l1p->next = l1->next;
        l1->next = NULL;
        freeStringList(&l1);
        l1 = l1p->next;
      } else {
        l1p = l1;
        l1 = l1->next;
      }
    }
    nu = lp;
    lp = lp->next;
  }
  // verify there's at least one entry or 
  // return the emtpy unifier
  int x = 0;
  if(!list->next){
    if(!list->entry){
      x = 1;
    } else {
      if(!strcomp(list->entry, "{ | }")){
        x = 1;
      }
    }
  }
  if(x){
    compos = malloc(6);
    strcopy("{ | }", compos);
    freeStringList(&list);
    return compos;
  }
  compos = joinStringList(list);
  freeStringList(&list);
  return compos;
}

char *unifyVariable(char *term1, char *term2, char *unifier);

/* unify - returns unification of provided terms in format {X|a}{Y|b}... */
char *unify(char *term1, char *term2, char *unifier){
  if(!unifier) return NULL;
  if(!term1 || !term2) return copyString(unifier);
  if(!strcomp(term1, term2)) return copyString(unifier);
  TermType tt1 = type(term1);
  TermType tt2 = type(term2);
  if(tt1 == TTVARIABLE){
    return unifyVariable(term1, term2, unifier);
  }
  if(tt2 == TTVARIABLE){ 
    return unifyVariable(term2, term1, unifier);
  }
  if(tt1 == TTFUNCTOR && tt2 == TTFUNCTOR){
    char *t1a = getArgs(term1);
    char *t2a = getArgs(term2);
    char *t1o = getOp(term1);
    char *t2o = getOp(term2);
    char *ufunc = unify(t1o, t2o, unifier);
    if(!ufunc){
      freeChar(&t1o);
      freeChar(&t2o);
      freeChar(&t1a);
      freeChar(&t2a);
      return NULL;
    }
    char *ufunc2 = unify(t1a, t2a, ufunc);
    freeChar(&t1o);
    freeChar(&t2o);
    freeChar(&t1a);
    freeChar(&t2a);
    freeChar(&ufunc);    
    return ufunc2;
  }
  if(tt1 == TTCONJUNCTION && tt2 == TTCONJUNCTION){
    char *ft1 = firstTerm(term1);
    char *ft2 = firstTerm(term2);
    char *rt1 = restTerm(term1, ft1);
    char *rt2 = restTerm(term2, ft2);
    char *uconj = unify(ft1, ft2, unifier);
    if(!uconj){
      freeChar(&ft1);
      freeChar(&ft2);
      freeChar(&rt1);
      freeChar(&rt2);
      return NULL;
    }
    char *uconj2 = unify(rt1, rt2, uconj);
    freeChar(&ft1);
    freeChar(&ft2);
    freeChar(&rt1);
    freeChar(&rt2);
    freeChar(&uconj);
    return uconj2;
  }
  return NULL;
}

char *unifyVariable(char *var, char *term, char *unifier){
  if(strInStr(unifier, var)){
    char *bound = getBound(var, unifier);
    if(bound){ 
      char *ub = unify(bound, term, unifier);
      freeChar(&bound);
      return ub;
    }
  }
  if(type(term) == TTVARIABLE){
    if(strInStr(unifier, term)){
      char *tbound = getBound(term, unifier);
      char *utv = unify(var, tbound, unifier);
      freeChar(&tbound);
      return utv;
    }
  }
  char buf[B_MAX_STRING_LENGTH];
  int bufindex = 0;
  if(strcomp(unifier, "{ | }")){
    strcopy(unifier, buf);
    bufindex = strlength(unifier);
  }
  buf[bufindex++] = '{';
  strcopy(var, buf + bufindex);
  bufindex += strlength(var);
  buf[bufindex++] = '|';
  strcopy(term, buf + bufindex);
  bufindex += strlength(term);
  buf[bufindex++] = '}';
  buf[bufindex] = '\0';
  char *u = malloc(strlength(buf)+1);
  strcopy(buf, u);
  return u;
}

char *substitute(char *term, char *unifier){
  if(!term) return NULL;
  StringList *list = splitByControlChars(term);
  StringList *l1 = list;
  while(l1){
    TermType typ = typeStringListEntry(l1);
    if(typ == TTVARIABLE){

      char *bound = getBound(l1->entry, unifier);
      if(bound){
        freeChar(&l1->entry);
        l1->entry = bound;
      }
    }
    l1 = l1->next;
  }
  char *newterm = joinStringList(list);
  freeStringList(&list);
  return newterm;
}

char *indexVariables(char *term){
  static int i;
  char buf[5];
  sprintf(buf, "%d", i);
  StringList *t = splitByControlChars(term);
  StringList *t1 = t;
  while(t1){
    TermType typ = typeStringListEntry(t1);
    if(typ == TTVARIABLE){
      char *newentry = concat(t1->entry, buf);
      freeChar(&t1->entry);
      t1->entry = newentry;
    }
    t1 = t1->next;
  }
  char *newterm = joinStringList(t);
  freeStringList(&t);
  i++;
  return newterm;
}

void appendResolution(char *unifier){
  char *r = Unifiers;
  if(!r){
    r = malloc(strlength(unifier)+1);
    strcopy(unifier, r);
    Unifiers = r;
    return;
  }
  freeChar(&Unifiers);
  Unifiers = concat(Unifiers, unifier);
}

int appendProof(char *term){
  if(!term) return 0;
  StringList *p = Proof;
  if(!p){
    p = malloc(sizeof(StringList));
    p->entry = malloc(strlength(term)+1);
    strcopy(term, p->entry);
    p->next = NULL;
    Proof = p;
  } else {
    while(p->next){
      p = p->next;
    }
    p->next = malloc(sizeof(StringList));
    p = p->next;
    p->entry = malloc(strlength(term)+1);
    strcopy(term, p->entry);
    p->next = NULL;
  }
  return 1;
}

int midresolveprompt(char *unifier){
  if(unifier){
    printf("Yes.\n");
    // printf("Θ = %s\n", unifier);
    // printf("q = %s\n", Query);
    char *t = Query;
    char *tp = NULL;
    int done = 0;
    while(!done){
      t = substitute(t, unifier);
      if(!tp){
        tp = t;
      } else {
        if(!strcomp(t, tp)){
          done = 1;
          freeChar(&tp);
        } else {
          freeChar(&tp);
          tp = t;
        }
      }
    }
    printf("Θq = %s\n", t);
    freeChar(&t);
  } else {
    printf("No.\n");
  }
  printf("More? (y/N)? ");
  char buf[10];
  fgets(buf, 9, stdin);
  if(buf[0] == 'Y' || buf[0] == 'y') return 0;
  return 1;
}

char *resolve(char *goal, char *unifier, int level){
  if(!goal) return copyString(unifier);
  StringList *kb = KnowledgeBase;
  char *ans = NULL;
  while(kb){
    char *kbentry = indexVariables(kb->entry);
    char *hed = head(kbentry);
    ans = unify(goal, hed, unifier);
    freeChar(&hed);
    if(!ans){
      freeChar(&kbentry);
    } else {
      // appendProof(kbentry);
      char *a1 = compose(unifier, ans);
      freeChar(&ans);
      ans = a1;
      char *bdy = body(kbentry);
      freeChar(&kbentry);
      char *bdyclause = NULL;
      char *restbdy = NULL;
      if(bdy){ 
        bdyclause = firstTerm(bdy);
        restbdy = restTerm(bdy, bdyclause);
        freeChar(&bdy);
      }
      while(bdyclause){
        char *ans2 = resolve(bdyclause, ans, level + 1);
        if(!ans2){
          freeChar(&ans);
          freeChar(&bdyclause);
          freeChar(&restbdy);
          freeStringList(&Proof);
          return NULL;
        }
        // appendProof(bdyclause);
        freeChar(&bdyclause);
        char *a2 = compose(ans, ans2);
        freeChar(&ans);
        freeChar(&ans2);
        ans = a2;
        if(!restbdy) break;
        bdyclause = firstTerm(restbdy);
        char *rb = restTerm(restbdy, bdyclause);
        freeChar(&restbdy);
        restbdy = rb;
      }
      if(level == 1){
        // appendResolution(ans);
        int r = midresolveprompt(ans);
        if(r) return ans;
      } else {
        return ans;
      }
    }
    freeChar(&ans);
    kb = kb->next;
  }
  return ans;
}

int loadKB(char *pathname){
  char buf[B_MAX_STRING_LENGTH];
  StringList *kb = NULL;
  StringList *kb1 = NULL;
  StringList *kb2 = NULL;
  FILE *f = fopen(pathname, "r");
  if(!f){ 
    return 0;
  }
  while(fgets(buf, B_MAX_STRING_LENGTH-1, f)){
    if(!kb){
      kb = malloc(sizeof(StringList));
      kb->entry = NULL;
      kb->next = NULL;
      kb1 = kb;
    }
    int length = strlength(buf);
    if(buf[length - 1] =='\n') {
      buf[length - 1] = '\0';
      length -= 1;
    }
    char *wffterm = wff(buf);
    length = strlength(wffterm);
    kb1->entry = wffterm;
    kb1->next = malloc(sizeof(StringList));
    kb2 = kb1;
    kb1 = kb1->next;
    kb1->entry = NULL;
    kb1->next = NULL;
  }
  fclose(f);
  freeStringList(&kb2->next);
  kb2->next = NULL;
  KnowledgeBase = kb;
  return 1;
}

int main(int argc, char const *argv[])
{
 
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

  char buf[B_MAX_STRING_LENGTH];
  char *emptyu = malloc(6);
  strcopy("{ | }", emptyu);
  while(1){
    printf("?-");
    fgets(buf, B_MAX_STRING_LENGTH-1, stdin);
    if(buf[0] != '\0' && buf[0] != '\n'){
      buf[strlength(buf)-1] = '\0';
      if(!strcomp(buf, "quit.")) break;
      Query = wff(buf);
      char *unifier = resolve(Query, "{ | }",1);
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
  freeChar(&emptyu);
  freeStringList(&KnowledgeBase);
  return 0;
}
