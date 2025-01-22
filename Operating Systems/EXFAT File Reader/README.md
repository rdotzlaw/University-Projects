# EXFAT Reader
#### Ryan Dotzlaw

This program reads any `.exfat` formatted files that are disk images of EXFAT formated file systems

## Running and Compilation

Compile the code with:

        make

Clean up the compiled binaries with:

        make clean

Execute the 'info' command with:

        ./exfat <disk image> info

Execute the 'list' command with:

        ./exfat <disk image> list

Execute the 'get' command with:

        ./exfat <disk image> get "<path/to/file.txt>"

The 'get' command will only return files, not directories

The path needs to be in double quotes ("), and delimited by forward slashes (/)

The 'get' command also returns files in the current directory, so if there's a file in the disk image that shares a name with a file in the current directory,
the file in the current directory will be overwritten (such as, this README being overwritten from the one in the given disk image).