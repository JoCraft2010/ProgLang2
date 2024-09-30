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
