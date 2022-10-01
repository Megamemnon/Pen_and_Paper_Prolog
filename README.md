# Pen & Paper Prolog
## What is Pen & Paper Prolog?
This is a minimal Prolog implementation with resolution and unification algorithms working on strings; no abstract syntax trees (ASTs). If you were to 'run' Prolog by hand, with pen and paper, this is (one way) how you might do it.  It's a tongue-in-cheek reference to 'pen and paper' RPGs, as opposed to computer software RPGs.  
## What does it do?
The current version supports facts and rules in a KnowledgeBase (KB) file, specified on the command line. The DEBUG define ignores command line parameters and looks for a file named 'testkb'.  
After loading KB, the ppp executable provides a prompt to the user where a query, in the form of a fact (ending in a period), or the atom 'quit.' can be submitted.  
ppp will attempt resolution and present the current Unifier and Goal upon Success, and prompt to continue. After completion, the final Unifier and all steps (in the order encountered by the resolution algortithm) are presented.  
## Language
Whitespace is ignored (in fact removed).  

> \<atom>               ::= [a-z][a-zA-Z0-9]*  
> \<variable>           ::= [A-Z][a-zA-Z0-9]*  
> \<functor> ::= \<atom> "(" \<term> ")"  
> \<term> ::= \<atom> | \<functor> | \<variable> | \<conjunction>  
> \<conjunction> ::= \<term> | \<conjunction> "," \<term>  
> \<implication> ::= \<term> ":-" \<conjunction>  
> \<complexconjunction> ::= \<conjunction> | \<complexconjunction> "," \<implication> | \<implication> "," \<complexconjunction>  
> \<fact> ::= \<term> "."  
> \<rule> ::= \<term> ":-" \<complexconjunction> "."  
> \<query> ::= \<fact> "."  

KB should consist of facts and rules.  
ppp expects a query at the "?-" prompt.  

## Usage
Specify a KB file when running ppp (e.g. "ppp database"). ppp will load contents of the specified text file into the global KnowledgeBase variable. Then ppp will present the Command prompt.

Command prompt ']' supports several commands.  
Queries can be entered directly from the Command prompt by starting the query with the traditional '?-'.  
Statement prompt '>' is presented when you can enter a statement.  

quit/0 - exits ppp.  
> ]quit.  

list/0 - lists all statements in KnowledgeBase. 
> ]list().  (List the entire KB)  

OK, that isn't really an arity of 0; I'll fix it.  

list/2 - lists statements in KB
  - Start - number of first statement to list, 0-based  
  - Count - number of statements to display  
> ]list(3, 1).  
> d(3).

edit/1  
  - index - 0-based index of statement to edit  
> ]edit(3).  
> Enter statement to replace statement 3:  
> d(3).  
> \>d(33).  
> Continue? (y/N) 

insert/1
  - index - 0-based index of statement before which new statement should be entered...  
> ]insert(3).  
> Enter statement to insert prior to statement 3:  
> d(3).  
> \> d(2).  
> Continue? (y/N)  

delete/1
  - index - 0-based index of statement to be deleted  
> ]delete(3).

append/0 - prompts for new statemnet and then appends it to KB.

## Project Goals
- Implement a functional (but minimal) form of Prolog
  - This goal is complete, for now, and tested with various included tests
    - likes(mary,wine). MaryLikesWine
    - true(X). Implication
    - a(s(s(0)), s(s(0)), X). Ackermann
  - Verified good Garbage Collection with Valgrind (no errors/leaks).
- Provide a readable proof to the user upon success (not quite there yet).  
- Support modifying the KB from within ppp (and saving the new KB file), using resolution as a method to verify correctness of new KB entries.
  - This goal is complete
    - Command prompt supports: list, edit, insert, append, delete and queries.
- provide additional language/KB features
  - Label individual KB entries
  - Comment on individual KB entries
