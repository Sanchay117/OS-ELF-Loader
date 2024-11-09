# ELF Loader in C

This program is a simple ELF (Executable and Linkable Format) loader written in C. It loads, maps, and executes an ELF executable, managing memory mappings and providing cleanup after execution. The loader is primarily designed to load segments of type `PT_LOAD` and to locate and execute the entry point function (`_start`) in the ELF binary.

## Features
- **ELF Header Parsing**: Reads the ELF header to validate the ELF file format.
- **Program Header Parsing**: Reads and processes program headers to locate loadable segments.
- **Memory Mapping**: Maps segments into memory using `mmap` and manages cleanup after execution.
- **Entry Point Execution**: Locates the entry point address in the loaded segments, typecasts it as a function pointer, and calls it.
- **Cleanup**: Releases allocated memory and unmapped segments after execution.

## Code Structure

### Structs
- **`Elf32_Ehdr`**: Represents the ELF header structure, which includes metadata about the ELF file, such as entry point and program header offset.
- **`Elf32_Phdr`**: Represents a program header, which describes segments within the ELF file.
- **`SegmentMap`**: Stores the address and size of memory-mapped segments for cleanup.

### Global Variables
- **`ehdr`**: Pointer to the ELF header.
- **`phdr`**: Pointer to the program header.
- **`segments_for_cleanup`**: Array of `SegmentMap` structs for tracking mapped segments.
- **`entry_pt_addr`**: Address of the entry point function (`_start`).
- **`fd`**: File descriptor for the ELF file.

### Functions

- **`read_elf_header()`**: Reads and validates the ELF header. Exits if the file is not in ELF format.
- **`read_programm_header(int i, unsigned short int programm_header_size)`**: Reads the `i`th program header from the ELF file.
- **`map_segments(unsigned short int programm_header_size)`**: Maps segments of type `PT_LOAD` into memory, copying the contents from the file and zeroing the remaining space if needed.
- **`loader_cleanup()`**: Frees allocated memory and unmaps all segments recorded in `segments_for_cleanup`.
- **`load_and_run_elf(char** elf_file)`**: Coordinates loading of the ELF file, maps segments, and executes the entry point function (`_start`).

### Usage

1. **Compilation**
   ```bash
   gcc -o elf_loader elf_loader.c
   ```

2. **Running the ELF Loader**
   ```bash
   ./elf_loader <path_to_ELF_executable>
   ```

3. **Example Execution**
   - Run the loader with an ELF executable:
     ```bash
     ./elf_loader sample.elf
     ```

## Error Handling
- Checks if the specified ELF file exists and is readable.
- Validates ELF format by checking the magic bytes (`0x7f`, `E`, `L`, `F`).
- Ensures successful memory mapping and handles errors in `mmap` and `munmap`.

## Cleanup and Memory Management
- Segments are tracked in `segments_for_cleanup` to ensure they are properly unmapped after execution.
- The function `loader_cleanup()` frees `ehdr` and unmaps all mapped segments.

## Notes
- The program expects the ELF file to contain a valid entry point in a loadable segment.
- This loader performs basic ELF handling, suitable for educational purposes or simple ELF files.

## License
This project is licensed under the MIT License.
