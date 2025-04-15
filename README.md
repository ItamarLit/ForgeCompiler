# ForgeCompilerProj
Hi and welcome to the Forge compiler, this compiler was made as a final project for my computer science college course, I think it came out pretty good!
## Features
This compiler has a fully working front end, (lexer, parser, semantic analyzer), and it creates ASM 64 bit code in MASM syntax.
- The compiler uses an FSM to lex, and an SLR parser using action and goto tables.
## How to run the compiler?
To run the compiler use x64 native tools command prompt for vs to run the batch file "run_compiler" and run the compiled code.
All files need to be .forge files and must be in the ProjectData folder, in this folder you will see the output of the compilation

**To run the compiler do:**
![image](https://github.com/user-attachments/assets/b2ccb652-51e6-4615-b94e-4b8e348b2a93)

You can also see debug data by using different flags:

-A for AST

-T for tokens

-S for symbol table

-O for output asm code


Full running code example:

![image](https://github.com/user-attachments/assets/bb2f7676-437f-468c-a959-d5bd58ca9d62)


To see more details on the Forge compiler please read the project book, it will be on my github aswell.

Thanks!
