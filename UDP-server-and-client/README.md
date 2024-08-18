Creating a shared library (DSO, Dynamic Shared Object) in Linux involves compiling source code into position-independent code (PIC) and then linking it into a shared library. Here is a step-by-step guide to create a shared library in Linux using gcc.

- Use the -fPIC option to generate position-independent code, which is required for shared libraries.

- Use the -shared option to create a shared library. The standard naming convention for shared libraries is to prefix the name with lib and use the .so extension.

### Compile

```
bash compile.sh
```

### Run Server

```
build/server
```

### Run Client

```
build/client
```
