//-----------------------------------------
// NAME: Ryan Dotzlaw
// COURSE: COMP 3430, SECTION: A02
// ASSIGNMENT: 1, QUESTION: 1
//
// REMARKS: Reading and interpreting ELF binary files
//
//-----------------------------------------


//------------------------------------------------------
// myRoutine
//
// PURPOSE: tell me what it does!
// INPUT PARAMETERS:
//     Describe those parameters which accept data values.
// OUTPUT PARAMETERS:
//     Describe those parameters which return values.
//     Include the value returned by the routine if not void.
//     Some of these may be the same as the input
//     parameters.
//------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

/* ELF Guide
 * [0,4): "Magic Number"
 * [4,5): 32/64-bit, 1=32bit, 2=64bit
 * [5,6): Endianness, 1=little, 2=big
 * [6,7): Set to 1 for the original and current version of ELF
 *
 *
 *
 *
 */

/* Data to grab:
 * : magic number
 * : 32 or 64 bit
 * : Endianness
 * : Target OS ABI
 * : Object file type
 * : ISA
 * : Process entry point address
 * : Header table address in file
 * : Size of entry in program header table
 * : Number of entries in program header table
 * : Size of entry in section header table
 * : Number of entries in section header table
 * : Entry in section headers that's the string table
 *
 */

/* EXAMPLE OUTPUT
 * ELF header:
 * 64-bit
 * little endian
 * compiled for 0x00 (operating system)
 * has type 0x02
 * compiled for 0x3e (isa)
 * entry point address 0x00000000004010c0
 * program header table starts at 0x0000000000000040
 * there are 11 program headers, each is 56 bytes
 * there are 33 section headers, each is 64 bytes
 * the section header string table is 32
 */

// constants for later use
const int32_t MAGIC_NUMBER = 0x464c457f;
const int PHEADER_OFFSET_32 = 0x20; // 32
const int PHEADER_OFFSET_64 = 0x38; // 56
const int SHEADER_OFFSET_32 = 0x28; // 40
const int SHEADER_OFFSET_64 = 0x40; // 64


typedef unsigned char BYTE;

// The data structure to hold all the data in the ELF header
#pragma pack(push)
#pragma pack(1)
typedef struct ELF_HEADER {
	int32_t magicNumber; // should equal MAGIC_NUMBER
	BYTE bitLen; // whether 32 or 64 bit
	BYTE endian; // endianness
	BYTE os_ABI;
	BYTE ABI_version; // further ABI specification
	int16_t obj_type; // object file type
	int16_t ISA;
	int64_t entry_point; // entry point address (diff len for 32/64 bit)
	int64_t phtable_addr; // program header table address
	int64_t shtable_addr; // section header table address
	int16_t ph_entry_size;
	int16_t pHead_count;
	int16_t sh_entry_size;
	int16_t sHead_count;
	int16_t sh_ind;
} e_header;

// The data structure to hold all the data in a single program header
typedef struct PROGRAM_HEADER {
	int32_t type;
	int64_t virtAddr;
	int64_t byte_count;
	int64_t byte_start;

} p_header;

// The data structure to hold all the data in a single section header
typedef struct SECTION_HEADER {
	int32_t name_offset;
	int32_t type;
	int64_t virtAddr;
	int64_t byte_count;
	int64_t byte_start;
	char * name;

} s_header;
#pragma pack(pop)



//------------------------------------------------------
// print32
//
// PURPOSE: Prints the first 32 bytes from a given address
// INPUT PARAMETERS:
//     fd: The file descriptor
//     addr: The address in the file to read from
// OUTPUT PARAMETERS:
//     None.
//------------------------------------------------------
void print32(int fd, int64_t addr){
	lseek(fd, addr, SEEK_SET);
	BYTE val = 0;
	printf("   ");
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 16; j++){
			read(fd, &val, 1); // read in a byte
			printf("%02x ", val);
		}
		printf("\n   ");
	}
}


//------------------------------------------------------
// main
//
// PURPOSE:
// INPUT PARAMETERS:
//     argc: An integer count of the elements in the agrv array
//     argv: An array containing strings of the parameters entered when running the program from the command line
// OUTPUT PARAMETERS:
//     0: Main has completed successfully
//------------------------------------------------------
int main(int argc, char* argv[]){
	// throws an unused variable error when compiling
	// I don't actually want to use argc
	argc = 0;

	// argv[1] is the file name
	char* FILE_NAME = argv[1];

	// header struct to store all the data in the ELF header
	// assigning elf_h to EmptyStruct clears all values that would be in elf_h's memory
	// this means that you don't have to worry about putting a 32-bit value into an int64_t and having old data inside it
	e_header *elf_h = malloc(sizeof(e_header));

	// for output
	char* endian;

	// for dealing w/ 32 or 64 bit
	int pOffset;
	int sOffset;
	int increment; // byte increment (maybe need)
	//int sh_off;

	printf("Starting to read file: %s\n", FILE_NAME);

	// open file
	int fd = open(FILE_NAME, O_RDONLY);
	// start at beginning
	lseek(fd, 0x0, SEEK_SET);
	// read elf header
	read(fd, &elf_h->magicNumber, 4);

	//printf("%x\n",MAGIC_NUMBER);
	//printf("%x\n",elf_h->magicNumber);

	// test to ensure the file is valid
	assert(elf_h->magicNumber == MAGIC_NUMBER);

	// continue reading
	// this is just filling up the e_header struct w/ the relevant data and ignoring the rest
	read(fd, &elf_h->bitLen, 1);
	read(fd, &elf_h->endian, 1);

	// constants for use with reading and output of data
	if(elf_h->endian == 1){ // little
		endian = "little";
	} else if(elf_h->endian == 2){ // big
		endian = "big";
	} else { // wtf
		printf("Failed to interpret data properly.\n");
		exit(EXIT_FAILURE);
	}
	// deals w/ the variations created by having a 32 or 64 bit file
	if(elf_h->bitLen == 1){ // 32 bit
		pOffset = PHEADER_OFFSET_32;
		sOffset = SHEADER_OFFSET_32;
		increment = 4;

	} else if(elf_h->bitLen == 2){ // 64 bit
		pOffset = PHEADER_OFFSET_64;
		sOffset = SHEADER_OFFSET_64;
		increment = 8;
	} else { // wtf
		printf("Failed to interpret data properly.\n");
		exit(EXIT_FAILURE);
	}

	// continue reading, again
	lseek(fd, 0x7, SEEK_SET); // skip
	read(fd, &elf_h->os_ABI, 1);
	read(fd, &elf_h->ABI_version, 1);
	lseek(fd, 0x10, SEEK_SET); // skip
	read(fd, &elf_h->obj_type, 2);
	read(fd, &elf_h->ISA, 2);
	lseek(fd, 0x18, SEEK_SET); // skip
	read(fd, &elf_h->entry_point, increment);
	read(fd, &elf_h->phtable_addr, increment);
	read(fd, &elf_h->shtable_addr, increment);
	lseek(fd, 6, SEEK_CUR); // skip
	read(fd, &elf_h->ph_entry_size, 2);
	read(fd, &elf_h->pHead_count, 2);
	read(fd, &elf_h->sh_entry_size, 2);
	read(fd, &elf_h->sHead_count, 2);
	read(fd, &elf_h->sh_ind, 2);
	// done reading ELF header
	printf("Finished reading ELF Header\n");
	fflush(stdout);

	// create program headers array, allocate memory
	p_header **prog_hs = malloc(sizeof(p_header) * elf_h->pHead_count);
	// read in program headers

	for(int i = 0; i < elf_h->pHead_count; i++){
		p_header *ph = malloc(sizeof(p_header));
		// sets the address to the current program header
		int64_t current_addr = elf_h->phtable_addr + i*pOffset;
		lseek(fd, current_addr, SEEK_SET);
		read(fd, &ph->type, 4);

		// if 64-bit, need to skip 4 more bits
		if(elf_h->bitLen == 2)
			lseek(fd, 4,SEEK_CUR);
		lseek(fd, increment,SEEK_CUR);
		read(fd, &ph->virtAddr, increment);
		read(fd, &ph->byte_start, increment);
		read(fd, &ph->byte_count, increment);
		prog_hs[i] = ph;
	}
	printf("Finished reading Program Headers\n");
	fflush(stdout);

	// create section headers array, allocate memory
	s_header **sect_hs = malloc(sizeof(s_header) * elf_h->sHead_count);
	// read section headers
	for(int i = 0; i < elf_h->sHead_count; i++){
		//printf("%d\n",i);
		s_header *sh = malloc(sizeof(s_header));
		// sets the address to the current section header
		int64_t current_addr = elf_h->shtable_addr + i*sOffset;

		//printf("%d : 0x%016lx\n",i, elf_h->shtable_addr + i*sOffset);

		lseek(fd, current_addr, SEEK_SET);
		read(fd, &sh->name_offset, 4);
		//printf("%x\n", sh->name_offset);
		read(fd, &sh->type, 4);
		//skip flags
		lseek(fd, increment, SEEK_CUR);
		read(fd, &sh->virtAddr, increment);
		read(fd, &sh->byte_start, increment);
		read(fd, &sh->byte_count, increment);

		// reach the name for the section
		//int64_t address;

		/*
		 * To get the section name:
		 * 1. First start at the where section headers begin in memory
		 * 	-> elf_h->shtable_addr
		 * 2. Then go to the section header that contains the string table
		 * 	-> the elf_h->sh_ind th table (elf_h->sh_ind * elf_h->sh_entry_size)
		 * 3. Then use that sections' byte_start to find out where the data is
		 * 4. Then use the current sections' name_offset to find where the name starts
		 * 5. Then read from the file
		 * 6. ???
		 * 7. Profit.
		 *
		 */
		/*
		// 1
		int64_t address = elf_h->shtable_addr;
		// 2
		address = address + (elf_h->sh_ind * elf_h->sh_entry_size);
		// 3
		int64_t byte_start;
		lseek(fd, address, SEEK_SET);
		// skip to byte start
		lseek(fd, 8+(2*increment), SEEK_CUR);
		read(fd, &byte_start, increment);
		// reach the start of string table data
		lseek(fd, byte_start, SEEK_SET);
		// byte_start should be close to 0x00003662 for hello.out32
		//printf("%lx\n", byte_start);
		//printf(" * %x\n", sh->name_offset);
		// 4
		lseek(fd, sh->name_offset, SEEK_CUR);
		printf("%d: %lx\n",i, byte_start + sh->name_offset);
		// null terminated strings, so if grab too much, won't show in output
		read(fd, &sh->name, 8);
		*/
		sect_hs[i] = sh;
	}
	printf("Finished reading Section Headers\n");
	fflush(stdout);

	// output elf header data
	printf("ELF Header:\n");
	printf(" * %d-bit\n",increment*8);
	printf(" * %s endian\n",endian);
	printf(" * Compiled for 0x%02x (operating system)\n",elf_h->os_ABI);
	printf(" * Has type 0x%02x\n",elf_h->obj_type);
	printf(" * Compiled for 0x%02x (isa)\n",elf_h->ISA);
	printf(" * Entry point address 0x%016lx\n",elf_h->entry_point);
	printf(" * Program header table starts at 0x%016lx\n",elf_h->phtable_addr);
	printf(" * There are %d program headers, each is %d bytes\n",elf_h->pHead_count,elf_h->ph_entry_size);
	printf(" * There are %d section headers, each is %d bytes\n",elf_h->sHead_count,elf_h->sh_entry_size);
	printf(" * The section header string table is %d\n\n",elf_h->sh_ind);
	//printf(" * String table starts at:  0x%016lx\n",(elf_h->sh_ind * elf_h->sh_entry_size) + elf_h->shtable_addr);
	printf("--------------------------------------------------\n\n");



	// output program headers
	for(int i = 0; i < elf_h->pHead_count; i++){
		// maybe don't need the *'s???
		p_header *current = prog_hs[i];
		printf("Program Header #%d:\n", i);
		printf(" * Segment type: %08x\n", current->type);
		printf(" * Virtual address of section: 0x%016lx\n", current->virtAddr);
		printf(" * Size in file: 0x%016lx bytes\n", current->byte_count);
		printf(" * First up to 32 bytes starting at 0x%016lx:\n", current->byte_start);
		// print 32 bytes
		print32(fd, current->byte_start);
		printf("\n");
	}
	printf("--------------------------------------------------\n\n");
	// output section headers
	for(int i = 0; i < elf_h->sHead_count; i++){
		s_header *current = sect_hs[i];
		printf("Section Header #%d:\n", i);

		// printf(" * Section name: %s\n", &current->name);
		printf(" * Type: 0x%08x\n", current->type);
		printf(" * Virtual address of section: 0x%016lx\n", current->virtAddr);
		printf(" * Size in file: %ld bytes\n", current->byte_count);
		printf(" * First up to 32 bytes starting at 0x%016lx:\n", current->byte_start);
		//printf(" * Name offset : 0x%016x\n", current->name_offset);
		// print 32 bytes
		print32(fd, current->byte_start);
		printf("\n");
	}



	// clean up
	for(int i = 0; i < elf_h->pHead_count; i++){
		free(prog_hs[i]);
	}

	// free all sect headers
	for(int i = 0; i < elf_h->sHead_count; i++){
		free(sect_hs[i]);
	}


	// end program
	close(fd);
	printf("Program completed successfully.\n");
	return 0;
}
