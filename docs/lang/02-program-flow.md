# ProgLang2 Language Docs: 2 - Program Flow

## 2.1 Function Declarations
A function is typically declared by writing the return type, the function name, an opening bracket, the arguments it takes and a closing bracket. The function body is enclosed by curly brackets.
> [!WARNING]
> This feature is partially implemented.
>
> Functions do not take arguments yet.
```
int name() {
  \ ...
  \ Do some stuff
  \ ...
}
```

## 2.2 Return Statements
A function with a return type will return a value. Once a value is returned, the function is interrupted and the program continues where the function was called. A value is returned using the `return` statement.
```
int name() {
  \ Do something
  return 42;
}
```

## 2.3 Entry Point
Every program needs an entry point which is defined in the `main` function. The `main` function needs to have the signature `int main()`. The value main returns is the exit code of the program, 0 meaning successful.
```
int main() {
  \ Do something

  \ Success
  return 0;
}
```

## 2.4 Function Calls
A function is called by writing it's name followed by brackets containing the arguments.
```
test(); \ test is executed
other(2, 4); \ other is executed with arguments 2 and 4
```

## 2.5 External Functions
An external function is declared by an @ followed by the return type of the function, the function's name and brackets containing the list of function argument types. Function argument types can either be a primitive type, a pointer to a primitive type indicated by a primitive type followed by an @ or an ellipse (~).
```
@ int getchar();
@ int printf(char @, ~);
```
