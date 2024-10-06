# ProgLang2 Language Docs: 3 - Variables

## 3.1 Variable Declarations
A variable is typically declared by writing the type and the name, optionally followed by an equal with an initializer value.
```
int number; \ number is declared but not initialized
int othernumber = 42; \ othernumber is initialized to 42
```
A variable can be assigned by equaling it to a value. Variables can be used as values.
```
number = anothernumber; \ number is 42 now
```

## 3.2 Arithmetic Operations
A value can be calculated using arithmetic operations like `+`, `-`, `*` and `/`. The typical order of operations applies here.
> [!WARNING]
> This feature is partially implemented.
>
> You can't divide numbers yet.
```
int a = 12 + 3 * 4; \ a is 24 now.
```
