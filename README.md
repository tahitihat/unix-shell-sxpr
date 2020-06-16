## unix-shell-sxpr
Unix shell that handles commands in the form of s-expressions.

Implementation of a Unix shell that handles commands in the form of s-expressions.

*TODO: Combine with parser code from another old C project to build a traditional Unix shell (i.e. one that does not require s-expressions as input).*

## To run: 
`make`

`./shell <s-expression>` where the s-expression is a single s-expression of the form *("&&" ("true") ("echo" "true"))*, or a file of newline-separated s-expressions, such as: 

("rm" "-f" "sample.txt")

("mkdir" "-p" "tmp")

("cp" "tests/sample.txt" "tmp")

("cd" "tmp")

("wc" "-l" "sample.txt")

### Operators

Operators either modify the behavior of one command or chain together multiple commands into a single command line.

1. Redirect input: ("<" ("sort") "foo.txt") - Take stdin from a file
2. Redirect output: (">" ("sort" "foo.txt") "output.txt") - Put stdout to a file
3. Background: ("&" ("sleep" "10")) - Run a command in the background, without waiting for it to finish; can also take two arguments.
4. And: ("&&" ("true") ("echo" "one")) - Execute the second command only if the first suceeds.
5. Or: ("||" ("true") ("echo" "one")) - Execute the second command only if the first fails.
6. Semicolon: (";" ("echo" "one") ("echo" "two")) - Execute two commands in sequence. Can also occur with one argument.


## Implementation
The general strategy for running any operator is to
- first fork()
- In the parent, wait for the child
- In the child do any manipulation of the file descriptor table and then A.) recursively run one command or B.) fork *again* and then recursively run two commands in the parent and child respectively.
