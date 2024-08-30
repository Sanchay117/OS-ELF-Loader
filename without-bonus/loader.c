#include "loader.h"

Elf32_Ehdr *ehdr; // pointer to ELF header struct
Elf32_Phdr *phdr; // pointer to Programm header struct

/*
u->unsigned
typedef struct {
u char		e_ident[ELF_NIDENT]; // Marks file as obj file and provides machine-independent data
u short  	e_type; // Object file type eg. executable file,core file, shared obj file etc.
u short     e_machine; // Tells the architecture of machine on which this ELF file was created
u int	    e_version; // Object file version
u int    	e_entry;// Entry point virtual address to which system first transfer control at process startup
u int    	e_phoff; // Program header table file offset
u int    	e_shoff; // Section header table file offset
u int    	e_flags; // Processor-specific flags
u short    	e_ehsize;// ELF header size in bytes
u short    	e_phentsize; // Size of each program header table structure
u short 	e_phnum; // Count of total program header table entry
u short    	e_shentsize;// Size of each Section header table structure
u short 	e_shnum; //Count of total section header table entry
u short    	e_shstrndx; // Section header table index of the entry associated with string table secion
} Elf32_Ehdr;

 */

int fd;

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** elf_file) {
  // 1. Load entire binary content into the memory from the ELF file.
  // 2. Iterate through the PHDR table and find the section of PT_LOAD 
  //    type that contains the address of the entrypoint method in fib.c
  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content
  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
  // 6. Call the "_start" method and print the value returned from the "_start"

  char* file_name = elf_file[1];
  // printf("%s %d\n",file_name,fd);

  // Now if we have come here clearly we have our Ehdr and fd as well

  // Next step: reading Programm header!

  // fd = open(exe, O_RDONLY);

  //ehdr = (Elf32_Ehdr *) malloc(sizeof(Elf32_Ehdr));
  //read(fd, ehdr, sizeof(Elf32_Ehdr));

  // int result = _start();
  // printf("User _start return value = %d\n",result);
}

int main(int argc, char** argv) 
{

  /*printf("Number of arguments: %d\n", argc);
  for (int i = 0; i < argc; i++) {
    printf("Argument %d: %s\n", i, argv[i]);
  }*/

  char* file_name;

  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \nYou need to provide ELF File as an argument\n",argv[0]);
    exit(1);
  }else{
    file_name = argv[1];
  }

  // printf("%s\n",file_name);

  // 1. carry out necessary checks on the input ELF file

  // Checking if file exists
  if (access(file_name, F_OK) != 0) {
    printf("Error: File %s does not exist.\n", file_name);
    exit(1);
  }

	// Checking if we have access to read said file
  fd = open(file_name, O_RDONLY);
  if (fd < 0) {
 	printf("Error opening file\n");
 	exit(1);
  }

  // Checking if the said file is an ELF

  ehdr = (Elf32_Ehdr*)(malloc(sizeof(Elf32_Ehdr))); // (Elf32_Ehdr*)-> typecasts void*

  if(read(fd,ehdr,sizeof(ehdr))!= sizeof(ehdr)){
  	printf("Error: Couldn't read ELF header\n");
  	exit(1);
  }

  if (ehdr->e_ident[0] != 0x7f || ehdr->e_ident[1] != 'E' || ehdr->e_ident[2] != 'L' || ehdr->e_ident[3] != 'F') {
    printf("Error: %s is not an ELF file.\n", file_name);
    exit(1);
  }

  // 2. passing it to the loader for carrying out the loading/execution
  // Now if we come so far, we are sure that we have an ELF!
  
  printf("YIPEE! (for debugging purposes)\n");
  load_and_run_elf(argv);
  
  // 3. invoke the cleanup routine inside the loader  
  //loader_cleanup();

  if (close(fd) < 0) {
	printf("Error closing file\n");
	exit(1);
  }

  free(ehdr);

  return 0;
}
