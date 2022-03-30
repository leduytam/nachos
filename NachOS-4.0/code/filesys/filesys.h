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
// Temporarily implement file system calls as 
// calls to UNIX, until the real file system
// implementation is available

typedef int OpenFileId;

class FileSystem {
public:
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

    int Create(char* name)
    {
        if (name == NULL)
            return -1;

        // OpenForWrite has a bug when file is already existed so we use OpenForReadWrite to check file is existed
        int fileDescriptor = OpenForReadWrite(name, FALSE);

        // If file is existed, return -1
        if (fileDescriptor != -1) return -1;
        Close(fileDescriptor);

        // If not existed, create file with OpenForWrite
        fileDescriptor = OpenForWrite(name);

        if (fileDescriptor == -1) return -1;
        Close(fileDescriptor);

        return 0;
    }

    OpenFile* Open(char* name)
    {
        int fileDescriptor = OpenForReadWrite(name, FALSE);

        if (fileDescriptor == -1) return NULL;
        return new OpenFile(fileDescriptor, name);
    }


    OpenFileId OpenGetId(char* name)
    {
        if (name == NULL || strlen(name) == 0)
            return -1;

        if (IsOpened(name))
            return -1;

        OpenFileId id = GetFreeId();

        if (id == -1)
            return -1;

        openingFiles[id] = Open(name);

        return id;
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

    int Read(char* buffer, int size, OpenFileId id)
    {
        if (id < 2 || id >= MAX_OPEN_FILES)
            return 0;

        if (openingFiles[id] == NULL)
            return 0;

        return openingFiles[id]->Read(buffer, size);
    }

    int Write(char* buffer, int size, OpenFileId id)
    {
        if (id < 2 || id >= MAX_OPEN_FILES)
            return 0;

        if (openingFiles[id] == NULL)
            return 0;

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

    int Remove(char* name)
    {
        if (name == NULL || strlen(name) == 0)
            return -1;

        if (IsOpened(name)) return -1;

        return Unlink(name) == 0;
    }
private:
    OpenFile** openingFiles;

    int GetFreeId()
    {
        for (int i = 2; i < MAX_OPEN_FILES; i++)
            if (openingFiles[i] == NULL)
                return i;

        return -1;
    }

    OpenFileId GetIdByName(char* name)
    {
        for (int i = 2; i < MAX_OPEN_FILES; i++)
            if (openingFiles[i] != NULL && strcmp(openingFiles[i]->GetName(), name) == 0)
                return i;

        return -1;
    }

    bool IsOpened(char* name)
    {
        return GetIdByName(name) != -1;
    }

    bool IsFreeId(OpenFileId id)
    {
        return id >= 2 && id < MAX_OPEN_FILES&& openingFiles[id] == NULL;
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
