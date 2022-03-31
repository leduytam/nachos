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
        // create array of open files
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

    /** Create a file
     *
     * @param name file name
     * @return 0 if successful, -1 otherwise (e.g. file already exists)
     * @idea if name is invalid then return -1
     *       if file is already exist then return -1
     *       if fileDescriptor == -1 when OpenForWrite then return -1 (e.g. no more space)
     *       otherwise create a new file and return 0
     */
    int Create(char* name)
    {
        if (name == NULL || strlen(name) == 0)
            return -1;

        // OpenForWrite has a bug when file is already existed so we use OpenForReadWrite to check file is existed
        int fileDescriptor = OpenForReadWrite(name, FALSE);

        // If file is existed, return -1
        if (fileDescriptor != -1) return -1;
        Close(fileDescriptor);

        // If not existed, create file with OpenForWrite
        fileDescriptor = OpenForWrite(name);

        // Create failed
        if (fileDescriptor == -1) return -1;
        Close(fileDescriptor);

        return 0;
    }

    // Default
    OpenFile* Open(char* name)
    {
        int fileDescriptor = OpenForReadWrite(name, FALSE);

        if (fileDescriptor == -1) return NULL;
        return new OpenFile(fileDescriptor, name);
    }

    /** Open a file
     *
     * @param name file name
     * @return file id if successful, -1 otherwise
     * @idea if name is not valid, return -1
     *       if file is opened, return -1
     *       if max number of files is reached, return -1
     *       if file is not existed, return -1
     *       otherwise, open file and return file id
     */
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

        if (openingFiles[id] == NULL)
            return -1;

        return id;
    }

    /** Close a file
     *
     * @param id file id
     * @return 0 if successful, -1 otherwise
     * @idea if id is not valid, return -1
     *       if file is not opened, return -1
     *       otherwise, close file and return 0
     */
    int Close(OpenFileId id)
    {
        if (id < 2 || id >= MAX_OPEN_FILES)
            return -1;

        if (openingFiles[id] == NULL)
            return -1;

        delete openingFiles[id];
        openingFiles[id] = NULL;

        return 0;
    }

    /** Read from a file
     *
     * @param buffer buffer to store data
     * @param size max size of buffer
     * @param id file id
     * @return number of bytes read
     * @idea if id is not valid, return 0
     *       if file is not opened, return 0
     *       otherwise, read data from file and return number of bytes read
     */
    int Read(char* buffer, int size, OpenFileId id)
    {
        if (id < 2 || id >= MAX_OPEN_FILES)
            return 0;

        if (openingFiles[id] == NULL)
            return 0;

        return openingFiles[id]->Read(buffer, size);
    }

    /** Write to a file
     *
     * @param buffer buffer to store data
     * @param size size of buffer
     * @param id file id
     * @return number of bytes written
     * @idea if id is not valid, return 0
     *       if file is not opened, return 0
     *       otherwise, write data to file and return number of bytes written
     */
    int Write(char* buffer, int size, OpenFileId id)
    {
        if (id < 2 || id >= MAX_OPEN_FILES)
            return 0;

        if (openingFiles[id] == NULL)
            return 0;

        return openingFiles[id]->Write(buffer, size);
    }

    /** Seek to a position in a file
     *
     * @param id file id
     * @param position position to seek to
     * @return actual position after seek, -1 if error
     * @idea if id is not valid, return -1
     *       if file is not opened, return -1
     *       if position == -1, set position to end of file
     *       if position > file size, set position to end of file
     *       otherwise, seek to position and return position
     * @note we added openfile->Seek(position) to filesys/openfile.h
     */
    int Seek(int position, OpenFileId id)
    {
        if (id < 2 || id >= MAX_OPEN_FILES)
            return -1;

        if (openingFiles[id] == NULL)
            return -1;

        return openingFiles[id]->Seek(position);
    }

    /** Remove a file
     *
     * @param name file name
     * @return 0 if successful, -1 otherwise
     * @idea if name is not valid, return -1
     *       if file is opened, return -1
     *       if file is not existed, return -1
     *       otherwise, remove file and return 0
     */
    int Remove(char* name)
    {
        if (name == NULL || strlen(name) == 0)
            return -1;

        if (IsOpened(name)) return -1;

        return Unlink(name);
    }
private:
    OpenFile** openingFiles;

    /** Get a free id
     *
     * @return free id if found, -1 otherwise
     * @idea traverse all id and find a free id
     */
    int GetFreeId()
    {
        for (int i = 2; i < MAX_OPEN_FILES; i++)
            if (openingFiles[i] == NULL)
                return i;

        return -1;
    }

    /** Get id of a file if is opened
     *
     * @param name file name
     * @return id of file if found, -1 otherwise
     * @idea traverse all id and compare name with name of file
     *       if name is same, return id
     *       otherwise, return -1
     * @note we added property name to filesys/openfile.h
     */
    OpenFileId GetIdByName(char* name)
    {
        for (int i = 2; i < MAX_OPEN_FILES; i++)
            if (openingFiles[i] != NULL && strcmp(openingFiles[i]->GetName(), name) == 0)
                return i;

        return -1;
    }

    /** Check if a file is opened
     *
     * @param name file name
     * @return TRUE if file is opened, FALSE otherwise
     * @idea traverse all id and compare name with name of file
     *       if name is same, return TRUE
     *       otherwise, return FALSE
     */
    bool IsOpened(char* name)
    {
        return GetIdByName(name) != -1;
    }

    /** Check an id is free
     *
     * @param id file id
     * @return TRUE if id is free, FALSE otherwise
     * @idea check if id is in range [2, MAX_OPEN_FILES) and file is NULL
     */
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
