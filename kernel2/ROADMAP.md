# Quimblos Kernel
## v0 Roadmap

- **Memory**
    - **Types**: Type definitions and a solver which stores them.
        - `include/type.h`
        - `include/typesolver.h`
        - `src/types.cpp`
    - **Manipulation Methods**: Methods to manipulate memory based on type definitions.
        - `include/memory.h`
        - `include/memory.cpp`
    - **Data Structures**: Base data structures for the types and the application itsefl.
        - `include/memory.h`
- **Code**
    - **Instructions**: Data structures to hold each instruction.
        - `include/code.h`
    - **Parser**: Methods to transform a byte sequence into a vector of Instructions.
        - `include/parser.h`
        - `src/parser.cpp`
- **Runtime**
    - **Operator**: Methods to perform operations with memory (assign, math, etc).
        - `include/operator.h`
        - `src/operator.cpp`
    - **Stack**: Execution stack to allow method calls.
        - `include/stack.h`
        - `src/stack.cpp`
    - **Thread**: Code executor with it's own memory blocks.
        - `include/thread.h`
        - `src/thread.cpp`
- **Kernel**
    - **Driver**: Memory block tied to an external aspect of the application, which can be manipulated through code.
    - **Node**: A manageable set of _drivers_ and _threads_.
    - **Storage**: Methods to manipulate long-term storage.

### Tests

|OK|File            |Details|
|--|--------------|-------|
|✔ |type          ||
|✏ |memory        |Missing complex types|
|✔ |parser        ||
|✏ |operator_cast |Missing non-bultin types|
|✏ |operator      |Must expand test cases|
|✕ |stack         ||
|✕ |thread        ||
|✕ |driver        ||
|✕ |node          ||
|✕ |storage       ||
