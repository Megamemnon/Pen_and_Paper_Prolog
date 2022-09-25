**unify(x, y, Θ)**  
- x - variable, term, compound term, or list of terms
- y - variable, term, compound term, or list of terms
- Θ - list of subsitutions identified thus far
- returns subsitutions that make x and y identical

> - if Θ = failure, then return failure  
>     - else if x=y then return Θ  
>     - else if x is variable then return **(unifyVariable(x, y, Θ))**  
>     - else if y is variable then return **(unifyVariable(y, x, Θ))**  
>     - else if x is compound and y is compound then return
  **(unify(args[x], args[y], unify(op[x], op[y], Θ)))**  
>     - else if x is body and y is body then return 
  **(unify(rest[x], rest[y], unify(first[x], first[y], Θ))**  
>     - else return failure


**unifyVariable(variable, x, Θ)**  
- variable - a variable
- x - variable, term, compound term, or list of terms
- Θ - list of subsitutions identified thus far
- adds/returns a substitution that binds variable to x

> - if variable is already bound return **(unify(bound value, x, Θ))**  
>     - else if x is a variable already bound then return **(unify(variable, x bound value, Θ))**  
>     - else if variable occurs in x then return failure  
>     - else add variable binds to x in Θ and return Θ

**backwardChain(KnowledgeBase, Goals, Θ)**
- KnowledgeBase - a list of all clauses
- Goals - 
- Θ - list of substitutions identified thus far  

> - If Goals is empty return Θ  
> - head' = substitue(Θ, first(Goals))  
> - for each entry in Knowledgebase  
>    - if NoSharedVariables and Θ' = unify(head of entry, head', Θ) does not fail  
>        - add **backwardChain(KnowledgeBase, [body of entry|rest(Goals)], compose(Θ, Θ')
)** to answer  
> - return answer

for each entry in Knowledgbase
  - if Θ' = unify(head, goal, Θ)
    - for each clause in body
      - if NOT Θ'' = unify(body, goal, Θ')
        - return FAILURE
