## Makefile
#### Run *'make'* to compile all the binaries.

#### Run *'make clean'* to clean up all the compiled binaries.

## ELF File Reading

After compiling, run with `./elf {binary file name}`

The code in `elf.c` is designed to read and interpret ELF (Executable and Linkable Format) binary files. Here's a high-level overview of what it does:

1. **ELF Header Reading**: It reads the ELF header from a binary file to extract various metadata, such as the magic number, bit length (32 or 64-bit), endianness, target OS ABI, object file type, ISA (Instruction Set Architecture), entry point address, and the locations and sizes of the program and section header tables.

2. **Program Headers**: It reads the program headers, which describe segments of the program to be loaded into memory. For each program header, it extracts information like the segment type, virtual address, size in the file, and the starting byte address.

3. **Section Headers**: It reads the section headers, which describe sections of the program, such as code, data, and other resources. It extracts similar information as for program headers, including the section type, virtual address, size, and starting byte address.

4. **Output**: The program outputs the extracted information in a human-readable format, including the ELF header details, program headers, and section headers.

5. **Utility Functions**: It includes utility functions like `print32` to print the first 32 bytes from a given address in the file.

6. **Memory Management**: The program dynamically allocates memory for storing the headers and ensures proper cleanup by freeing allocated memory before exiting.

7. **Assertions and Error Handling**: It uses assertions to verify the validity of the ELF file and handles errors by exiting the program if the file is not valid or if data cannot be interpreted correctly.

This code is useful for analyzing ELF files, which are commonly used in Unix-like operating systems for executables, object code, shared libraries, and core dumps.