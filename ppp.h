#ifndef PPP_H
#define PPP_H


typedef struct STRING_LIST{
  char *entry;
  struct STRING_LIST *next;
} StringList;

extern StringList *KnowledgeBase;
extern StringList *WorkingKB;
extern char *Query;
extern char *Unifiers;
extern StringList *Proof;
extern int AbortResolution;

void freeChar(char **charptr);

void freeStringList(StringList **list);

StringList *splitByControlChars(char *str);

StringList *copyStringList(StringList *strlist);

void printStringlist(StringList *list, int start, int count);

int fprintStringlist(char *filepathname, StringList *list);

void deleteStatement(StringList **strlist, int index);

void replaceStatement(StringList *strlist, int index, char *newstmnt);

void insertStatement(StringList **strlist, int index, char *newstmnt);

void appendStatement(StringList *strlist, char *newstmnt);

char *wff(char *clause);

int loadKB(const char *pathname);

char *resolve(char *goal, char *unifier, int level);

#endif