#include "loader.h"

Elf32_Ehdr *ehdr; // pointer to ELF header struct
Elf32_Phdr *phdr; // pointer to Programm header struct

/*
u->unsigned
typedef struct {
u char		  e_ident[ELF_NIDENT]; // Marks file as obj file and provides machine-independent data
u short  	  e_type; // Object file type eg. executable file,core file, shared obj file etc.
u short     e_machine; // Tells the architecture of machine on which this ELF file was created
u int	      e_version; // Object file version
u int    	  e_entry;// Entry point virtual address to which system first transfer control at process startup
u int    	  e_phoff; // Program header table file offset
u int    	  e_shoff; // Section header table file offset
u int      	e_flags; // Processor-specific flags
u short    	e_ehsize;// ELF header size in bytes
u short    	e_phentsize; // Size of each program header table structure
u short 	  e_phnum; // Count of total program header table entry
u short    	e_shentsize;// Size of each Section header table structure
u short 	  e_shnum; //Count of total section header table entry
u short    	e_shstrndx; // Section header table index of the entry associated with string table secion
} Elf32_Ehdr;

*/

/*
typedef struct
{
unsigned int    p_type; // type of segment being described (e.g., PT_LOAD, etc.) 
unsigned int    p_offset; // segment offset from the beginning of the ELF file //
unsigned int    p_vaddr;//  virtual address at which first byte of the segment resides in memory
unsigned int    p_paddr;// ignore this for assignment 
unsigned int    p_filesz;// ignore this for assignment 
unsigned int    p_memsz;// total bytes this segment will occupy in memory
unsigned int    p_flags;// ignore this for assignment
unsigned int    p_align;// ignore this for assignment
} Elf32_Phdr;
*/

int fd;
void* entry_pt_addr;
/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  
}

/*
 * Load and run the ELF executable file
*/

void load_and_run_elf(char** elf_file) {
  printf("INSIDE LOAD_AND_RUN_ELF FUNC [for debugging]\n");

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

  // Next step: reading all the Programm headers!
  
  unsigned short int programm_header_size = ehdr->e_phentsize;
  
  for(int i = 0;i<ehdr->e_phnum;i++){

    phdr = (Elf32_Phdr*)(malloc(sizeof(Elf32_Phdr)));
    lseek(fd,ehdr->e_phoff + i*programm_header_size,SEEK_SET);
    
    if (read(fd, phdr, sizeof(Elf32_Phdr)) != sizeof(Elf32_Phdr)) {
      printf("Error: Couldn't read program header\n");
      exit(1);
    }
    
    // printf("Program Header %d:\n", i);
    // printf("  Type: %u\n", phdr->p_type);
    // printf("  Offset: %u\n", phdr->p_offset);
    // printf("  Virtual Address: %u\n", phdr->p_vaddr);
    // printf("  File Size: %u\n", phdr->p_filesz);
    // printf("  Memory Size: %u\n", phdr->p_memsz);
    // printf("  Flags: %u\n", phdr->p_flags);
    // printf("  Alignment: %u\n", phdr->p_align);

    // printf("----------------------\n");

    // now loading PT_LOAD

    if(phdr->p_type==PT_LOAD){
      void* virtual_mem = mmap(NULL,phdr->p_memsz,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_ANONYMOUS|MAP_PRIVATE,0,0);

      if(virtual_mem==MAP_FAILED){
        printf("Error Mapping memory\n");
        exit(1);
      }

      lseek(fd,phdr->p_offset,SEEK_SET);

      // Reading data
      if (read(fd, virtual_mem, phdr->p_filesz) != phdr->p_filesz) {
        printf("Error reading segment data\n");
        exit(1);
      }

      // Zero out the rest of the memory if the segment is larger than the file size
      if (phdr->p_filesz < phdr->p_memsz) {
        memset((char*)virtual_mem + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
      }

      printf("Segment loaded at address %p\n", virtual_mem);

       if (ehdr->e_entry >= phdr->p_vaddr && ehdr->e_entry < phdr->p_vaddr + phdr->p_memsz) {
          size_t offset_inside_segment = ehdr->e_entry - phdr->p_vaddr;
          entry_pt_addr = (char*) virtual_mem + offset_inside_segment;
          printf("Entry point located at address %p\n", entry_pt_addr);
       }

      // free(virtual_mem);
    }
    
    free(phdr);
  }

  if (entry_pt_addr != NULL) {
        // Typecast the entry_point_address to a function pointer and call it
        int (*entry_func)() = (int (*)())entry_pt_addr;
        int result = entry_func();
        printf("User _start return value = %d\n", result);
    } else {
        printf("Error: Entry point not found in any segment\n");
    }

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

  if(read(fd,ehdr,sizeof(Elf32_Ehdr))!= sizeof(Elf32_Ehdr)){
  	printf("Error: Couldn't read ELF header\n");
  	exit(1);
  }

  if (ehdr->e_ident[0] != 0x7f || ehdr->e_ident[1] != 'E' || ehdr->e_ident[2] != 'L' || ehdr->e_ident[3] != 'F') {
    printf("Error: %s is not an ELF file.\n", file_name);
    exit(1);
  }

  // 2. passing it to the loader for carrying out the loading/execution
  // Now if we come so far, we are sure that we have an ELF!

  printf("YIPEE! [File is valid and ELF header is loaded ] (for debugging purposes)\n");
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
