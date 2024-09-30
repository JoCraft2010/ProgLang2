# ProgLang2 Language Docs: 1 - Preprocessor

## 1.1 Syntax
In the ProgLang2 programming language preprocessor directives are defined by a `$` followed by the preprocessor directive's name and it's corresponding data.
```
$directive "some data"
```

## 1.2 Preprocessor Directives
This is a list of all preprocessor directives currently present in the ProgLang2 programming language.

- [Include](#include)

### Include
The include directive includes another source file into the current file. It takes one argument which is the relative path of the file to include relative to the current file. The below example includes everything implemented in `libfile.pl2` into `main.pl2`.
```
\ src/main.pl2
$include "libfile.pl2"

\ ...
\ Do something
\ ...
```
```
\ src/libfile.pl2

\ ...
\ Implement something
\ ...
```
