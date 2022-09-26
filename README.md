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
> \<complexconjunction> ::= \<conjunction> | \<complexconjunction> "," \<implication> | > > \<implication> "," \<complexconjunction>  
> \<fact> ::= \<term> "."  
> \<rule> ::= \<term> ":-" \<complexconjunction> "."  
> \<query> ::= \<fact> "."  

KB should consist of facts and rules.  
ppp expects a query at the "?-" prompt.  

## Project Goals
- Implement a functional (but minimal) form of Prolog
  - This goal is complete, for now, and tested with various included tests
    - likes(mary,wine). MaryLikesWine
    - true(X). Implication
    - a(s(s(0)), s(s(0)), X). Ackermann
  - Verified good Garbage Collection with Valgrind (no errors/leaks).
- Provide a readable proof to the user upon success (not quite there yet).  
- Support modifying the KB from within ppp (and saving the new KB file), using resolution as a method to verify correctness of new KB entries.
- provide additional language/KB features
  - Label individual KB entries
  - Comment on individual KB entries
