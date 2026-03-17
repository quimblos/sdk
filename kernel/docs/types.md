# Quimblos Typing System

Quimblos is strongly typed.

## Type Casting

Casting a value from X to Y:

| | VOID | ERROR | PTR | BOOL | UINT8 | INT8 | UINT16 | INT16 | UINT32 | INT32 | FLOAT32 | STRING | ARRAY |
|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
|VOID||||||||||||||
|ERROR||☑||||||||||☑||
|PTR|||☑||☑|☑|☑|☑|☑|☑||||
|BOOL||||☑|☑|☑|☑|☑|☑|☑|☑|||
|UINT8|||☑|☑|☑|☑|☑|☑|☑|☑|☑|||
|INT8|||☑|☑|☑|☑|☑|☑|☑|☑|☑|||
|UINT16|||☑|☑|☑|☑|☑|☑|☑|☑|☑|||
|INT16|||☑|☑|☑|☑|☑|☑|☑|☑|☑|||
|UINT32|||☑|☑|☑|☑|☑|☑|☑|☑|☑|||
|INT32|||☑|☑|☑|☑|☑|☑|☑|☑|☑|||
|FLOAT32||||☑|☑|☑|☑|☑|☑|☑|☑|||
|STRING||☑||||||||||☑||
|ARRAY||||||||||||||

## Numerical Operators

When applying numerical operators to values, the _source_ value is cast to the _target_ value before the operation.

| | VOID | ERROR | PTR | BOOL | UINT8 | INT8 | UINT16 | INT16 | UINT32 | INT32 | FLOAT32 | STRING | ARRAY |
|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
|ADD|||☑||☑|☑|☑|☑|☑|☑|☑|☑||
|SUB|||☑||☑|☑|☑|☑|☑|☑|☑|||
|MULT|||☑||☑|☑|☑|☑|☑|☑|☑|||
|DIV|||☑||☑|☑|☑|☑|☑|☑|☑|||
|MOD|||☑||☑|☑|☑|☑|☑|☑|☑|||
|POW|||☑||☑|☑|☑|☑|☑|☑|☑|||
