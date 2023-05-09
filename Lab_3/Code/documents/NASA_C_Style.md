# NASA C Style Guide Cheat Sheet

### 1. **File and Directory Names**

- Use descriptive names, avoid abbreviations
- Separate words with underscores
- Use lowercase letters

### 2. **Comments**

- Use '**/* ... */**' for block comments
- Use '**//**' for single-line comments (only if your C compiler supports C++-style comments)
- Comment at the beginning of each file with the file name, author, and a brief description
- Comment each function with its purpose, inputs, outputs, and any side effects
- Add inline comments for complex code sections

### 3. **Indentation and Spacing**

- Use 3 spaces for each indentation level (do not use tabs)
- Use spaces around operators and after commas
- Place braces on a separate line and align with the control statement

### 4. **Variable and Function Names**

- Use descriptive names, avoid abbreviations
- Use lowercase letters and separate words with underscores
- Prefix global variable names with '**g_**'
- Prefix static variables and functions with '**s_**'
- Use verb-noun pairs for function names

### 5. **Constants and Macros**

- Use uppercase letters and separate words with underscores
- Use #define for constants and macros
- Enclose macro parameters in parentheses to avoid precedence issues
### 6. **Data Types**

- Use fixed-width integer types ('**int32_t**', '**uint32_t**', etc.) for predictable sizes
- Use '**float**' or '**double**' for floating-point numbers
- Use bool for boolean values (if your C compiler supports C99 or later)
- Use '**struct**' for grouping related data elements

### 7. **Control Structures**

- Use '**if**', '**else if**', '**else**' for conditional statements
- Use '**for**' and '**while**' loops for iteration
- Use '**switch**' and '**case**' for multi-way branching
- Keep control structures simple and avoid deep nesting

### 8. **Functions**

- Keep functions short and focused on a single task
- Use function prototypes and include parameter types
- Use '**static**' keyword for file-scoped functions
- Limit the use of global variables and use function parameters instead

### 9. **Error Handling**

- Check for errors after calling functions that may fail
- Use consistent error reporting mechanisms (e.g., return codes, global - error variables)
- Document error conditions in function comments

### 10. **Portability**

- Avoid compiler-specific features and extensions
- Use standard C library functions when possible
- Test code on multiple platforms and compilers