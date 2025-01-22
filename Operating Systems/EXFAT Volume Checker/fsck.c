// Ryan Dotzlaw - 


#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <limits.h>

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

#define BYTES_PER_CLUSTER (1 << mbs->bytes_per_sector_shift) * (1 << mbs->sectors_per_cluster_shift)

int main(int argc, char *argv[]) {
	(void)argc;

	char *volume_name = argv[1];
	main_boot_sector *mbs = malloc(sizeof(main_boot_sector));

	int fd = open(volume_name, O_RDONLY);

	// just put tha whole thang in there
	read(fd, mbs, 512);

	//check JumpBoot
	printf("Checking JumpBoot\n");
	if(mbs->jump_boot[0] != 0xEB && mbs->jump_boot[1] != 0x76 && mbs->jump_boot[2] != 0x90 ){
		printf("Inconsistent file system: JumpBoot must be: '0xEB7690', value is '0x%x%x%x'.\n", mbs->jump_boot[0], mbs->jump_boot[1], mbs->jump_boot[2]);
		exit(EXIT_FAILURE);
	}
	printf("* JumpBoot OK w/ value: '0x%x%x%x'.\n", mbs->jump_boot[0], mbs->jump_boot[1], mbs->jump_boot[2]);
	// check name
	printf("Checking file system name\n");
	if(strcmp("EXFAT   ", mbs->fs_name) != 0){
		printf("Inconsistent file system: File System name must be : 'EXFAT   ', value is '%s'\n", mbs->fs_name);
		exit(EXIT_FAILURE);
	}
	printf(" * File System name OK w/ value: 'EXFAT   '.\n");


	// check must be zero
	printf("Checking must be zero area\n");
	for(int i = 0; i < 53; i++){
		if(mbs->must_be_zero[i] != 0){
			printf("Inconsistent file system: Must Be Zero section needs all zeros, non-zero at index: %d.\n", i);
			exit(EXIT_FAILURE);
		}
	}
	printf(" * Must be zero area OK, has only zeros.\n");
	// no need to check partition offset
	// check volume length



	printf("Checking volume length\n");
	if(mbs->volume_length > ULLONG_MAX || mbs->volume_length < ((1 << 20) / (1 << mbs->bytes_per_sector_shift))){
		printf("Inconsistent file system: Volume length out of range, length is %lu instead\n", mbs->volume_length);
		exit(EXIT_FAILURE);
	}
	printf(" * Volume length OK w/ value: '%lu'.\n", mbs->volume_length);

	// check fatOffset
	printf("Checking fatOffset \n");
	if((mbs->fat_offset < 24) || mbs->fat_offset > (mbs->cluster_heap_offset - (mbs->fat_length * mbs->number_of_fats))){
		printf("Inconsistent file system: fatOffset out of range, offset is %u instead\n", mbs->fat_offset);
		exit(EXIT_FAILURE);
	}
	printf(" * Fat Offset OK w/ value: '%u'\n", mbs->fat_offset);

	// check fatLength
	printf("Checking fatLength\n");
	unsigned int most = floor((mbs->cluster_heap_offset - mbs->fat_offset) / mbs->number_of_fats);
	unsigned int least = (mbs->cluster_count + 2) * (1 << 2) / (1 << mbs->bytes_per_sector_shift);
	if(mbs->fat_length > most || mbs->fat_length < least){
		printf("Inconsistent file system: fatLength our of range [%d, %d], instead %d\n",least, most, mbs->fat_length );
		exit(EXIT_FAILURE);
	}
	printf(" * Fat Length OK w/ value: '%d'.\n", mbs->fat_length);

	// check First Cluster of Root Directory
	printf("First Cluster of Root Directory\n");
	if(mbs->first_cluster_of_root_directory < 2 || mbs->first_cluster_of_root_directory > mbs->cluster_count + 1){
		printf("Inconsistent file system: First Cluster of Root Directory out of range [2, %d], instead is %d\n",mbs->cluster_count + 1, mbs->first_cluster_of_root_directory );
		exit(EXIT_FAILURE);
	}
	printf(" * First Cluster of Root Directory OK w/ value: '%d'.\n", mbs->first_cluster_of_root_directory);

	// check bootSignature
	printf("Checking bootSignature\n");
	if(mbs->boot_signature != 0xAA55){
		printf("Inconsistent file system: Boot signature is invalid, should be 0xAA55, instead: %x\n", mbs->boot_signature);
		exit(EXIT_FAILURE);
	}
	printf(" * Boot Signature OK w/ value '0x%x'.\n", mbs->boot_signature);

	printf("MBR appears to be consistent\n");

	/*
    * Find the cluster for the root directory using the FirstClusterOfRootDirectory cluster.
    * Scan through the DirectoryEntrys in the root directory until you find the entry with entry type 0x81 (indicating the Allocation Bitmap directory entry).
        * Each DirectoryEntry is 32 bytes in size, and the EntryType is always the first byte.
    * Find the first cluster of the Allocation Bitmap using the FirstCluster field from the Allocation Bitmap DirectoryEntry.
    * Read all bytes from the cluster. The number of bytes you should read from the cluster are DataLength bytes.
    * Count the number of set bits in each byte using the __builtin_popcount function (population count counts the number of set bits).
    * Check that the fraction of set bits out of all bits in the allocation bitmap matches PercentInUse in the MBR.
	 */

	int allocation_bit_percent = 0;
	int alloc_count = 0;

	printf("Checking allocation bitmap\n");

	// cluster[n] = clusterHeapOffset + (n) * 2 ^(sectorsPerClusterShift)
	int root_address = mbs->cluster_heap_offset * (1 << mbs->bytes_per_sector_shift) + ((mbs->first_cluster_of_root_directory - 2) * BYTES_PER_CLUSTER);




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
		if(ent->entry_type == 0x81 && ent->data_length == 303){
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


	allocation_bit_percent = floor(((double)alloc_count / mbs->cluster_count) * 100);

	if(mbs->percent_in_use != allocation_bit_percent){
		printf("Inconsistent file system: Bitmap allocation doesn't match percent in use (%d%%), instead: %d/%u bits -> %d%%\n", mbs->percent_in_use, alloc_count, mbs->cluster_count, allocation_bit_percent);
		exit(EXIT_FAILURE);
	}

	printf(" * Allocation bitmap matches MBR percent in use.\n");

	printf("File System appears to be consistent\n");
	close(fd);
	exit(EXIT_SUCCESS);
}
