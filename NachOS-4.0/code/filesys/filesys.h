// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"

#define MAX_OPEN_FILES 10
#define CONSOLE_INPUT 0
#define CONSOLE_OUTUT 1

#ifdef FILESYS_STUB 

typedef int OpenFileId;
// Temporarily implement file system calls as 
// calls to UNIX, until the real file system
// implementation is available

class FileSystem {
public:
    OpenFile** openingFiles;

    FileSystem()
    {
        openingFiles = new OpenFile * [MAX_OPEN_FILES];

        for (int i = 0; i < MAX_OPEN_FILES; i++)
            openingFiles[i] = NULL;
    }

    ~FileSystem()
    {
        for (int i = 0; i < MAX_OPEN_FILES; i++)
            if (openingFiles[i] != NULL)
                delete openingFiles[i];
        delete[] openingFiles;
    }

    bool Create(char* name)
    {
        int fileDescriptor = OpenForWrite(name);

        if (fileDescriptor == -1) return false;
        Close(fileDescriptor);
        return true;
    }

    OpenFile* Open(char* name)
    {
        int fileDescriptor = OpenForReadWrite(name, false);

        if (fileDescriptor == -1) return NULL;
        return new OpenFile(fileDescriptor, name);
    }

    bool Close(OpenFileId id)
    {
        if (id < 2 || id >= MAX_OPEN_FILES)
            return false;

        if (openingFiles[id] == NULL)
            return false;

        delete openingFiles[id];
        openingFiles[id] = NULL;

        return true;
    }

    OpenFileId GetOpenFileId()
    {
        for (int i = 2; i < MAX_OPEN_FILES; i++)
            if (openingFiles[i] == NULL)
                return i;
        return -1;
    }

    bool Remove(char* name)
    {
        return Unlink(name) == 0;
    }

    int Read(char* buffer, int size, OpenFileId id)
    {
        if (id < 2 || id >= MAX_OPEN_FILES)
            return -1;

        if (openingFiles[id] == NULL)
            return -1;

        return openingFiles[id]->Read(buffer, size);
    }

    int Write(char* buffer, int size, OpenFileId id)
    {
        if (id < 2 || id >= MAX_OPEN_FILES)
            return -1;

        if (openingFiles[id] == NULL)
            return -1;

        return openingFiles[id]->Write(buffer, size);
    }

    int Seek(int position, OpenFileId id)
    {
        if (id < 2 || id >= MAX_OPEN_FILES)
            return -1;

        if (openingFiles[id] == NULL)
            return -1;

        return openingFiles[id]->Seek(position);
    }
};

#else // FILESYS

class FileSystem {
public:
    FileSystem(bool format);		// Initialize the file system.
                    // Must be called *after* "synchDisk" 
                    // has been initialized.
                        // If "format", there is nothing on
                    // the disk, so initialize the directory
                        // and the bitmap of free blocks.

    bool Create(char* name, int initialSize);
    // Create a file (UNIX creat)

    OpenFile* Open(char* name); 	// Open a file (UNIX open)

    bool Remove(char* name);  		// Delete a file (UNIX unlink)

    void List();			// List all the files in the file system

    void Print();			// List all the files and their contents

private:
    OpenFile* freeMapFile;		// Bit map of free disk blocks,
                     // represented as a file
    OpenFile* directoryFile;		// "Root" directory -- list of 
                     // file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
