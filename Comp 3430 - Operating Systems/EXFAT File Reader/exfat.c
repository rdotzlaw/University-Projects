
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <assert.h>


#pragma pack(1)
#pragma pack(push)
typedef struct MAIN_BOOT_SECTOR {
	uint8_t jump_boot[3];
	char fs_name[8];
	uint8_t must_be_zero[53];
	uint64_t partition_offset;
	uint64_t volume_length;
	uint32_t fat_offset;
	uint32_t fat_length;
	uint32_t cluster_heap_offset;
	uint32_t cluster_count;
	uint32_t first_cluster_of_root_directory;
	uint32_t volume_serial_number;
	uint16_t fs_revision;
	uint16_t fs_flags;
	uint8_t bytes_per_sector_shift;
	uint8_t sectors_per_cluster_shift;
	uint8_t number_of_fats;
	uint8_t drive_select;
	uint8_t percent_in_use;
	uint8_t reserved[7];
	uint8_t bootcode[390];
	uint16_t boot_signature;
} main_boot_sector;
#pragma pack(pop)

#pragma pack(1)
#pragma pack(push)
typedef struct DIR_ENTRY {
	uint8_t entry_type;
	uint8_t bitmap_flags;
	uint8_t reserved[18];
	uint32_t first_cluster;
	uint64_t data_length;
} entry;
#pragma pack(pop)

#pragma pack(1)
#pragma pack(push)
typedef struct LAB_ENTRY {
	uint8_t entry_type;
	uint8_t char_count;
	uint16_t label[11];
	uint8_t reserved[8];
} label_entry;
#pragma pack(pop)

#pragma pack(1)
#pragma pack(push)
typedef struct FILE_ENTRY {
	uint8_t entry_type; // == 0x85
	uint8_t secondary_count; // MINIMUM 2 (for 1 stream EXT, and 1+ fileName)
	uint16_t checksum;
	uint16_t attributes;
	uint8_t reserved1[2];
	uint32_t time_create;
	uint32_t time_modify;
	uint32_t time_access;
	uint8_t create_inc;
	uint8_t modify_inc;
	uint8_t access_inc;
	uint8_t reserved2[9];
} file_entry;
#pragma pack(pop)

#pragma pack(1)
#pragma pack(push)
typedef struct STREAM_ENTRY {
	uint8_t entry_type; // == 0xC0
	uint8_t secondary_flags;
	// bit 0: allocation possible (0 - No cluster allocated; 1 - Cluster allocation possible)
	// bit 1: No FAT chain (0 - Clusters are NOT contiguous, need FAT chain; 1 - Clusters are contiguous for file, just read in file_len for data once in first_cluster)
	uint8_t reserved1;
	uint8_t name_len;
	uint16_t name_hash;
	uint8_t reserved2[2];
	uint64_t file_len;
	uint8_t reserved3[4];
	uint32_t first_cluster;
} stream_entry;
#pragma pack(pop)

#pragma pack(1)
#pragma pack(push)
typedef struct NAME_ENTRY {
	uint8_t entry_type; // == 0xC1
	uint8_t secondary_flags;
	// bit 0: allocation possible (0 - No cluster allocated; 1 - Cluster allocation possible)
	// bit 1: No FAT chain (0 - Clusters are NOT contiguous, need FAT chain; 1 - Clusters are contiguous for file, just read in file_len for data once in first_cluster)
	uint16_t name[15];
} name_entry;
#pragma pack(pop)

#pragma pack(1)
#pragma pack(push)
typedef struct FILE_DATA {
	int ind;
	file_entry *f;
	stream_entry *s;
	name_entry **n;
} file_dat;
#pragma pack(pop)

#pragma pack(1)
#pragma pack(push)
typedef struct CHAIN_LINK {
	uint32_t val;
	struct CHAIN_LINK *next;
} link_t;
#pragma pack(pop)

#define BYTES_PER_CLUSTER (1 << mbs->bytes_per_sector_shift) * (1 << mbs->sectors_per_cluster_shift)

int bytes_per_cluster;
/**
 * Convert a Unicode-formatted string containing only ASCII characters
 * into a regular ASCII-formatted string (16 bit chars to 8 bit
 * chars).
 *
 * NOTE: this function does a heap allocation for the string it
 *       returns, caller is responsible for `free`-ing the allocation
 *       when necessary.
 *
 * uint16_t *unicode_string: the Unicode-formatted string to be
 *                           converted.
 * uint8_t   length: the length of the Unicode-formatted string (in
 *                   characters).
 *
 * returns: a heap allocated ASCII-formatted string.
 */
static char *unicode2ascii( uint16_t *unicode_string, uint8_t length )
{
	assert( unicode_string != NULL );
	assert( length > 0 );

	char *ascii_string = NULL;

	if ( unicode_string != NULL && length > 0 )
	{
		// +1 for a NULL terminator
		ascii_string = calloc( sizeof(char), length + 1);

		if ( ascii_string )
		{
			// strip the top 8 bits from every character in the
			// unicode string
			for ( uint8_t i = 0 ; i < length; i++ )
			{
				ascii_string[i] = (char) unicode_string[i];
			}
			// stick a null terminator at the end of the string.
			ascii_string[length] = '\0';
		}
	}

	return ascii_string;
}

void info(char *image){
	main_boot_sector *mbs = malloc(sizeof(main_boot_sector));
	int fd = open(image, O_RDONLY);
	read(fd, mbs, 512);

	// volume label
	// search through root directory
	int root_address = mbs->cluster_heap_offset * (1 << mbs->bytes_per_sector_shift) + ((mbs->first_cluster_of_root_directory - 2) * BYTES_PER_CLUSTER);

	lseek(fd, root_address, SEEK_SET);

	// find correct entry type
	while(1){
		//printf("%d\n",x);
		label_entry *ent = malloc(sizeof(label_entry ));
		read(fd, ent, 32);
		if(ent->entry_type == 0x83){
			// is allocation bitmap entry
			// get first cluster field
			printf("Volume label is: %s\n", unicode2ascii(ent->label, ent->char_count));
			break;

		}

		free(ent);

	}

	// volume serial
	printf("Volume Serial Number: %u\n", mbs->volume_serial_number);


	// free space
	int alloc_count = 0;

	// cluster[n] = clusterHeapOffset + (n) * 2 ^(sectorsPerClusterShift)


	lseek(fd, root_address, SEEK_SET);
	uint32_t cluster_ind;
	uint64_t dat_len;
	// each entry is 32 bytes, first byte is type
	// find entry with type == 0x81 (indicating alloc bitmap entry)
	int x = 0;

	while(1){
		//printf("%d\n",x);
		entry *ent = malloc(sizeof(entry));
		read(fd, ent, 32);
		if(ent->entry_type == 0x81){
			// is allocation bitmap entry
			// get first cluster field
			cluster_ind = ent->first_cluster;
			dat_len = ent->data_length;
			break;
		}

		free(ent);
		x++;
	}

	// have cluster index for bitmap
	int bit_address = mbs->cluster_heap_offset * (1 << mbs->bytes_per_sector_shift) +  ((cluster_ind - 2) * BYTES_PER_CLUSTER);

	lseek(fd, bit_address, SEEK_SET);
	// read in bitmap, then do pop-count
	unsigned char bitmap[dat_len];
	//printf("size: %lu\n", sizeof(bitmap));
	read(fd, bitmap, dat_len);
	int i = 0;
	while((uint64_t)i < dat_len){
		int set_bits = __builtin_popcount(bitmap[i]);
		alloc_count += set_bits;
		i++;
	}


	printf("Clusters in use: %u/%u\n", alloc_count, mbs->cluster_count);
	printf("Total KB in use: %u\n", (alloc_count * BYTES_PER_CLUSTER) / 1024);
	printf("Total KB free: %u\n", ((mbs->cluster_count - alloc_count) * BYTES_PER_CLUSTER) / 1024);

	// cluster size
	printf("Sectors per Cluster: %u\nBytes per Sector: %u bytes\nBytes per Cluster: %u bytes\n", (1 << mbs->sectors_per_cluster_shift), (1 << mbs->bytes_per_sector_shift), BYTES_PER_CLUSTER);

}

// takes an index for an entry in the FAT, and returns a linked list of cluster indexes
link_t *build_chain(int index, main_boot_sector *mbs, int fd){
	lseek(fd, (mbs->fat_offset * (1 << mbs->bytes_per_sector_shift)) + (index*4), SEEK_SET);
	/*int i = 0;
	while(1){
		uint32_t *buf = malloc(sizeof(uint32_t));
		read(fd, buf, 4);
		printf("%d: 0x%x\n", index+i, *buf);
		i++;
		sleep(1);
	}*/

	link_t *chain_link = malloc(sizeof(link_t));
	link_t *curr = chain_link;
	chain_link->next = NULL;
	read(fd, &chain_link->val, 4);

	while(curr->val != 0xFFFFFFFF){
		assert(curr->val < (mbs->cluster_count + 1) * 4);
		link_t *next_link = malloc(sizeof(link_t));
		next_link->next = NULL;
		lseek(fd, (mbs->fat_offset * (1 << mbs->bytes_per_sector_shift)) + curr->val*4, SEEK_SET);
		read(fd, &next_link->val, 4);
		curr->next = next_link;
		curr = next_link;
	}
	//printf("Built cluster chain for %d\n", index);
	return chain_link;
}
void deal_with_file(int fd, main_boot_sector *mbs, int depth, uint64_t address, const file_dat *file);
void list_recur(int fd, main_boot_sector *mbs, link_t *chain, int address);
void read_dir(int fd, int index, link_t *chain, main_boot_sector *mbs, int depth, void (*func)(int, main_boot_sector*, int, uint64_t, const file_dat*));
/**
 * General code flow
 *  - Start at root directory
 *  - Search through for files & other directories
 *  - If read a file, print out it's name
 *  - If read a directory, recurse on the directory
 *  	- Each recursion, add a '-' to denote depth
 *
 *	- In general, this is a depth first recursive search using the pointers for file entries to clusters in the cluster heap
 *
 */
void list(char *image){
	main_boot_sector *mbs = malloc(sizeof(main_boot_sector));
	int fd = open(image, O_RDONLY);
	read(fd, mbs, 512);
	// build cluster chain for root
	// root fat starts @ fatOffset*shift + (cluster index for root)

	lseek(fd, mbs->fat_offset * (1 << mbs->bytes_per_sector_shift), SEEK_SET);


	//printf("%d\n", mbs->first_cluster_of_root_directory);
	link_t *root_chain = build_chain(mbs->first_cluster_of_root_directory, mbs, fd);
	bytes_per_cluster = BYTES_PER_CLUSTER;
	//int root_address = mbs->cluster_heap_offset * (BYTES_PER_CLUSTER) + ((mbs->first_cluster_of_root_directory - 2) * BYTES_PER_CLUSTER);

	read_dir(fd, mbs->first_cluster_of_root_directory, root_chain, mbs, 0, deal_with_file);

}
/*
void list_read(int fd, main_boot_sector *mbs){

}

void list_recur(int fd, main_boot_sector *mbs, link *chain, int address){
	uint32_t val = chain->val;
	// read in current cluster
	lseek(address, SEEK_SET);
	list_read();

	while(val != 0xFFFFFFFF){
		// shift to new cluster
		lseek(mbs->cluster_heap_offset + (val * BYTES_PER_CLUSTER), SEEK_SET);

		// read in current cluster

		// no need to check if next is null, if val isn't all F's, then it isn't
		val = chain->next->val;
		chain = chain->next;

	}



}
*/

void print_name(int depth, const file_dat *file) {
	//int name_len = file->s->name_len;
	for(int i = 0; i < depth; i++){
		printf("-");
	}
	// convert each filename entry into ascii and print them out'
	int i = 0;
	while(file->n[i] != NULL){
		printf("%s", unicode2ascii(file->n[i]->name, 15));
		i++;
	}
	printf("\n");
}


/* Read all the entries in the current cluster (directory)
 * Doesn't use cluster chains, just keeps reading until read 'len' bytes
 * Each completed 'file' gets 'func' called on it
 * */
void read_dir_cont(int fd, int index, main_boot_sector *mbs, int depth, uint64_t len, void (*func)(int, main_boot_sector*, int, uint64_t, const file_dat*)){
	// noFatChain == 1, therefore, the data in this file is contiguous, meaning f->s->file_len is how many bytes to read
	//printf("reading cont dir\n");
	uint64_t bytes_left = len;
	//printf("bytes in dir: %lu\n", bytes_left);
	uint64_t address = (mbs->cluster_heap_offset * (1 << mbs->bytes_per_sector_shift)) + ((index - 2) * BYTES_PER_CLUSTER); // for returning after recursion
	//printf("%lu\n", address);
	lseek(fd, address, SEEK_SET);
	file_entry *current = malloc(sizeof(file_entry));
	file_dat *file = NULL;
	while(bytes_left > 0){
		// read in next value
		read(fd, current, 32);
		//printf("0x%x\n", current->entry_type);
		address += 32;
		bytes_left -= 32;
		if (current->entry_type == 0x85) {

			// deal with previous file
			(*func)(fd, mbs, depth, address, file);

			//printf("new file\n");
			// either file or directory
			// need to read in the whole thing first
			file = malloc(sizeof(file_dat)); // new file created,
			file->ind = 0;
			file->n = malloc(sizeof(name_entry) * 17); // max of 17 name entries

			// input new entry
			file->f = current;

		} else if (current->entry_type == 0xC0) { // is stream ext
			file->s = (stream_entry *) current;

		} else if (current->entry_type == 0xC1) { // is fileName
			file->n[file->ind] = (name_entry *) current;
			file->ind++;

		} else {
			//printf("%x\n", current->entry_type);
		}
		//printf("%d\n", bytes);
		current = malloc(sizeof(file_entry));

	}
	(*func)(fd, mbs, depth, address, file);



}


// Basically, takes a file, prints out the file name, and then recurses on it if needed
// will choose between the read_dir_cont if the directory is contiguous,
// or read_dir, if the file is not contiguous
// passes itself as an argument for 'func' when recursing
void deal_with_file(int fd, main_boot_sector *mbs, int depth, uint64_t address, const file_dat *file) {
	if (file != NULL) { // there is a previous file
		// if file type, print out it's name
		// else directory type, print out name, then recurse
		if (file->f->attributes & 0x10) { // if only 4th bit (start from 0) is set
			// its a directory
			//printf("is file[dir]\n");
			print_name(depth, file);
			// do tha recursion stuff, man
			// build FAT chain, if it NEEDS one
			//printf("^ first cluster: %u\n", file->s->first_cluster);
			if (file->s->secondary_flags == 0x3) { // no fat chain, contiguous read
				read_dir_cont(fd, file->s->first_cluster, mbs, depth+1, file->s->file_len, deal_with_file);

			} else if(file->s->secondary_flags == 0x1){ // non contiguous
				link_t *recurs_chain = build_chain(file->s->first_cluster, mbs, fd);
				read_dir(fd, file->s->first_cluster, recurs_chain, mbs, depth + 1, deal_with_file);
			}

			// return to current address
			lseek(fd, address, SEEK_SET);
		} else {
			// its a file
			//printf("is file[file]\n");
			print_name(depth, file);
		}

	}
}

/*
 * Read all the entries in the current directory
 * Once read BYTES_PER_CLUSTER bytes, follow cluster chain to next cluster
 * Once reach end of cluster, and no more cluster chains, return
 * * When it gets a new file_entry value, call 'func' on 'file', then create new 'file'
 * * * This means 'read_dir', and 'read_dir_cont' are used for both the <list> and <get> commands
 */
void read_dir(int fd, int index, link_t *chain, main_boot_sector *mbs, int depth, void (*func)(int, main_boot_sector*, int, uint64_t, const file_dat*)){
	int bytes = bytes_per_cluster;
	uint64_t count = 0;
	//printf("%u\n", BYTES_PER_CLUSTER);
	//printf("%u\n", mbs->first_cluster_of_root_directory);
	//printf("%u\n", mbs->fat_offset);
	//printf("%u\n", 1 << mbs->bytes_per_sector_shift);
	//printf("%u\n", mbs->cluster_heap_offset);
	uint64_t address = (mbs->cluster_heap_offset * (1 << mbs->bytes_per_sector_shift)) + ((index - 2) * BYTES_PER_CLUSTER);
	//address = 612352;
	file_entry *current = malloc(sizeof(file_entry));
	//printf("%d seeking to: %lu\n",index, address);
	lseek(fd, address, SEEK_SET);
	file_dat *file = NULL;
	int flag = 1;
	while(flag && bytes != 0) { // until reach END of LAST cluster

		while (bytes != 0) { // until reach END of THIS cluster
			// read in next value
			read(fd, current, 32);
			//printf("0x%x\n", current->entry_type);
			address += 32;
			bytes -= 32;
			if (current->entry_type == 0x85) {

				// deal with previous file
				(*func)(fd, mbs, depth, address, file);


				//printf("new file\n");
				// either file or directory
				// need to read in the whole thing first
				file = malloc(sizeof(file_dat)); // new file created,
				file->ind = 0;
				file->n = malloc(sizeof(name_entry) * 17); // max of 17 name entries

				// input new entry
				file->f = current;

			} else if (current->entry_type == 0xC0) { // is stream ext
				file->s = (stream_entry *) current;

			} else if (current->entry_type == 0xC1) { // is fileName
				file->n[file->ind] = (name_entry *) current;
				file->ind++;
			} else {
				//printf("%x\n", current->entry_type);
			}
			current = malloc(sizeof(file_entry));

		}
		// go to next cluster, if applicable, update vars
		if (chain->val != 0xFFFFFFFF) {
			//printf("Going to next cluster: %d\n", chain->val);
			count++;
			bytes = bytes_per_cluster;
			address = (mbs->cluster_heap_offset * (1 << mbs->bytes_per_sector_shift)) +
					  ((chain->val - 2) * BYTES_PER_CLUSTER);
			chain = chain->next;
			//printf("seeking to : %lu\n", address);
			lseek(fd, address, SEEK_SET);
		} else {// otherwise, break out of loop
			flag = 0; // this was our last cluster to read, we done
			// deal with last file
			(*func)(fd, mbs, depth, address, file);

		}



	}

	//printf("Done reading current dir\n");
	// done reading the current dir
	// go up level of recursion
}

// some global variables to make the use of function arguments easier
int path_ind = 0;
char **parsed = NULL;

// takes a file_dat structure, goes and builds a FAT chain for it (if necessary)
// then reads in the entire file into a file created in the running directory
void extract_file(const file_dat *file, int fd, main_boot_sector *mbs) {
	//printf("***\n");
	char *str = malloc(sizeof(char) * 256); // max file name == 255 chars, [0,255) chars, [255] = '\0'
	str[255] = '\0';
	int i = 0;
	while(file->n[i] != NULL){
		str = strcat(str, unicode2ascii(file->n[i]->name, 15));
		i++;
	}

	int output = open(str, O_WRONLY | O_CREAT, 0x1FF);
	if(file->s->secondary_flags == 0x3){ // if contiguous
		// ez, just read in file_len bytes, put 'em into output
		uint64_t address = (mbs->cluster_heap_offset * (1 << mbs->bytes_per_sector_shift)) + ((file->s->first_cluster - 2) * BYTES_PER_CLUSTER);
		lseek(fd, address, SEEK_SET); // go to first cluster
		//printf("%lu\n", address);
		uint8_t *the_whole_damn_thing = malloc(sizeof(uint8_t) * file->s->file_len);
		read(fd, the_whole_damn_thing, file->s->file_len);
		//printf("%s\n", the_whole_damn_thing);
		// dump eet
		write(output, the_whole_damn_thing, file->s->file_len);
		exit(EXIT_SUCCESS);
	} else if(file->s->secondary_flags == 0x1){ // if not contiguous
		//printf("not\n");
		link_t *chain = build_chain(file->s->first_cluster, mbs, fd);
		/*int x = 0;
		link_t *temp = chain;
		while(temp->val != 0xFFFFFFFF){
			printf("[%d]: 0x%x\n", x, temp->val);
			x++;
			temp = temp->next;
		}
		printf("[%d]: 0x%x\n", x, temp->val);*/
		uint64_t address = (mbs->cluster_heap_offset * (1 << mbs->bytes_per_sector_shift)) + ((file->s->first_cluster - 2) * BYTES_PER_CLUSTER);
		lseek(fd, address, SEEK_SET); // go to first cluster
		//printf("%lu\n", address);

		// read in BYTES_PER_CLUSTER for all clusters in 'chain'
		int flag = 1;
		uint64_t bytes = BYTES_PER_CLUSTER;
		// loop, until it is done
		while(flag){
			// read the cluster
			uint8_t *the_cluster = malloc(sizeof(uint8_t) * BYTES_PER_CLUSTER);
			read(fd, the_cluster, BYTES_PER_CLUSTER);
			//printf("%s\n", the_cluster);
			// put it in the file
			write(output, the_cluster, BYTES_PER_CLUSTER);
			free(the_cluster);
			// go to next cluster, if applicable, update vars
			if (chain->val != 0xFFFFFFFF) {
				bytes = BYTES_PER_CLUSTER;
				address = (mbs->cluster_heap_offset * (1 << mbs->bytes_per_sector_shift)) +
						  ((chain->val - 2) * BYTES_PER_CLUSTER);
				chain = chain->next;
				lseek(fd, address, SEEK_SET); // go to next cluster

			} else { // break out of loop
				flag = 0;
				// read last cluster
				// read the cluster
				//the_cluster = malloc(sizeof(uint8_t) * BYTES_PER_CLUSTER);
				//read(fd, the_cluster, BYTES_PER_CLUSTER);
				// put it in the file
				//write(output, the_cluster, BYTES_PER_CLUSTER);
				exit(EXIT_SUCCESS);
			}
		}
	} else {
		printf("we have a problem\n");
	}


}

// Basically, takes a file, if the file name matches parsed[path_ind], then recurse if it's a directory,
// if it's a file, we've reached the goal, since "path/is.txt/this.txt" is not valid, you'll just get is.txt instead
// when performing recursion passes itself as 'func'
void check_file_name(int fd, main_boot_sector *mbs, int depth, uint64_t address, const file_dat *file){
	if (file != NULL) { // there is a previous file

		// if directory, check to see if it's on the path, then recurse
		if (file->f->attributes & 0x10) { // if only 4th bit (start from 0) is set
			// its a directory


			char *str = malloc(sizeof(char) * 256); // max file name == 255 chars, [0,255) chars, [255] = '\0'
			str[255] = '\0';
			int i = 0;
			while(file->n[i] != NULL){
				str = strcat(str, unicode2ascii(file->n[i]->name, 15));
				i++;
			}
			//printf("str[dir]: %s\n", str);

			// is the correct directory on path
			if(strcmp(str, parsed[path_ind]) == 0){
				//print_name(depth, file);
				path_ind++;
				// do tha recursion stuff, man
				// build FAT chain, if it NEEDS one;
				if (file->s->secondary_flags == 0x3) { // no fat chain, contiguous read
					read_dir_cont(fd, file->s->first_cluster, mbs, depth+1, file->s->file_len, check_file_name);

				} else if(file->s->secondary_flags == 0x1){ // non contiguous
					link_t *recurs_chain = build_chain(file->s->first_cluster, mbs, fd);
					read_dir(fd, file->s->first_cluster, recurs_chain, mbs, depth + 1, check_file_name);
				}


			} else {
				// no recursion
				// return to current address
				lseek(fd, address, SEEK_SET);
				return;
			}
		} else {
			// its a file, no more recursion
			// check to see if it's the right file
			char *str = malloc(sizeof(char) * 256); // max file name == 255 chars, [0,255) chars, [255] = '\0'
			str[255] = '\0';
			int i = 0;
			while(file->n[i] != NULL){
				//sprintf(str, unicode2ascii(file->n[i]->name, 15), str);
				str = strcat(str, unicode2ascii(file->n[i]->name, 15));
				i++;
			}
			//printf("str[file][%d]: %s\n",i, str);

			if(strcmp(str, parsed[path_ind]) == 0) { // 'file' is the file we want
				extract_file(file, fd, mbs);
				exit(EXIT_SUCCESS);
			} // otherwise
			return;
		}

	}
}

void get(char *image, char *path){
	main_boot_sector *mbs = malloc(sizeof(main_boot_sector));
	int fd = open(image, O_RDONLY);
	read(fd, mbs, 512);
	// count the '/'s to see how big the array needs to be
	int i = 0;
	int path_depth = 0;
	while(path[i] != '\0'){
		if(path[i] == '/')
			path_depth++;
		i++;
	}
	// parse the path
	parsed = malloc(256 * (path_depth + 1));
	if(path_depth != 0) {
		parsed[0] = strtok(path, "/");
		for (int x = 1; x <= path_depth; x++) {
			parsed[x] = strtok(NULL, "/");
		}
	} else {
		parsed[0] = path;
	}

	/*
	for(int x = 0; x <= path_depth; x++){
		printf("%d: %s\n", x, parsed[x]);
	}*/
	//exit(EXIT_SUCCESS);

	// build cluster chain for root
	// root fat starts @ fatOffset*shift + (cluster index for root)

	lseek(fd, mbs->fat_offset * (1 << mbs->bytes_per_sector_shift), SEEK_SET);


	//printf("%d\n", mbs->first_cluster_of_root_directory);
	link_t *root_chain = build_chain(mbs->first_cluster_of_root_directory, mbs, fd);
	bytes_per_cluster = BYTES_PER_CLUSTER;
	//int root_address = mbs->cluster_heap_offset * (BYTES_PER_CLUSTER) + ((mbs->first_cluster_of_root_directory - 2) * BYTES_PER_CLUSTER);

	read_dir(fd, mbs->first_cluster_of_root_directory, root_chain, mbs, 0, check_file_name);

}

int main(int argc, char *argv[]) {
	(void) argc;

	if(strcmp(argv[2], "info") == 0){
		info(argv[1]);
	} else if(strcmp(argv[2], "list") == 0){
		list(argv[1]);
	} else if(strcmp(argv[2], "get") == 0 && argc == 4){
		get(argv[1], argv[3]);
	} else {
		printf("Invalid command\nCorrect format: ./exfat <image> <info | list | get> <\"get path\">\n");
		printf("Examples:\n");
		printf(" -> ./exfat exfat-vol.exfat info\n");
		printf(" -> ./exfat exfat-vol.exfat list\n");
		printf(" -> ./exfat exfat-vol.exfat get \"path/to/file.txt\"\n");
		printf("* get command only gets FILES, not directories\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

}
