// A data-structure to hold the file-info

#ifndef MYFILE_H
#define MYFILE_H

// This structure will probably vanish if the CORBA stuff is up again

struct myFile {
    unsigned char *data;
    unsigned long length;
};
#endif // MYFILE_H
