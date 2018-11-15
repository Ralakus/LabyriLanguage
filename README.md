# Labyrinth Langauge

# Made in pure C11
* It is built upon the Labyrinth compiler framework

# Current state
All that there is of the compiler is the lexer

# Variable types
* Numbers
* Strings
* Arrays
* Structs which the user can define

## Other types
Since the language will be interpreted at first, it will be dynamic
### Planned types
* int8
* int16
* int32
* int64
* isize
* uint8
* uint16
* uint32
* uint64
* usize

# Sample syntax 
```CS
main :: (args: [] str) {
    x: var = "Hello world!" // var because the language is currently dynamic
    print(x ++ "\n");
    return 0;
}```