# Labyrinth Langauge

[![build](https://gitlab.com/Ralakus/LabyrinthLanguage/badges/master/build.svg)](https://gitlab.com/Ralakus/LabyrinthLanguage/pipelines)
[![coverage](https://gitlab.com/Ralakus/LabyrinthLanguage/badges/master/coverage.svg)](https://gitlab.com/Ralakus/LabyrinthLanguage/-/jobs)
[![license](http://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)

# Made in pure C11
Why not use C++?
C was chosen due to it being more challenging

# Current state
All that there is of the compiler is the lexer

# Variable types
Since the language is under development, there will only be 2 basic types at first ( int and float )
* Numbers  ( int and float )
* Strings  ( str )
* Arrays   ( [num] or [..] for dynamic, [num..] to preallocate )
* Booleans ( bool )
* Structs which the user can define

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
* float32
* float64

# Sample syntax 
```javascript
main :: (args: [] str) {
    x: str = "Hello world!"; // Test variable
    print(x ++ "\n");
    return 0;
}
```