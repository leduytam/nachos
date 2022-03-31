// openfile.h 
//	Data structures for opening, closing, reading and writing to 
//	individual files.  The operations supported are similar to
//	the UNIX ones -- type 'man open' to the UNIX prompt.
//
//	There are two implementations.  One is a "STUB" that directly
//	turns the file operations into the underlying UNIX operations.
//	(cf. comment in filesys.h).
//
//	The other is the "real" implementation, that turns these
//	operations into read and write disk sector requests. 
//	In this baseline implementation of the file system, we don't 
//	worry about concurrent accesses to the file system
//	by different threads.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef OPENFILE_H
#define OPENFILE_H

#include "copyright.h"
#include "utility.h"
#include "sysdep.h"

#ifdef FILESYS_STUB
// Temporarily implement calls to 
// Nachos file system as calls to UNIX!
// See definitions listed under #else
class OpenFile {
private:
    char* name; // name of file
public:
    // default
    OpenFile(int f)
    {
        file = f;
        currentOffset = 0;
    }

    // open a file but need to specify the name
    OpenFile(int f, char* name)
    {
        file = f;
        currentOffset = 0;
        this->name = new char[strlen(name) + 1];
        strcpy(this->name, name);
    }

    ~OpenFile()
    {
        Close(file);
        delete[] name;
    } // close the file

    int ReadAt(char* into, int numBytes, int position) {
        Lseek(file, position, 0);
        return ReadPartial(file, into, numBytes);
    }

    int WriteAt(char* from, int numBytes, int position) {
        Lseek(file, position, 0);
        WriteFile(file, from, numBytes);
        return numBytes;
    }

    int Read(char* into, int numBytes) {
        int numRead = ReadAt(into, numBytes, currentOffset);
        currentOffset += numRead;
        return numRead;
    }

    int Write(char* from, int numBytes) {
        int numWritten = WriteAt(from, numBytes, currentOffset);
        currentOffset += numWritten;
        return numWritten;
    }

    /** Set the current offset to position
     *
     * @param position the position to set
     * @return actual position after seek, -1 if error
     * @idea if position is invalid then return -1
     *       if position == -1 then set currentOffset to the end of file
     *       otherwise set currentOffset to position and return position
     */
    int Seek(int position)
    {
        if (position < -1)
            return -1;

        int length = Length();

        if (position == -1 || position > length)
            currentOffset = length;
        else
            currentOffset = position;

        return currentOffset;
    }

    char* GetName()
    {
        return name;
    }

    int Length() { Lseek(file, 0, 2); return Tell(file); }

private:
    int file;
    int currentOffset;
};

#else // FILESYS
class FileHeader;

class OpenFile {
public:
    OpenFile(int sector);		// Open a file whose header is located
                    // at "sector" on the disk
    ~OpenFile();			// Close the file

    void Seek(int position); 		// Set the position from which to 
                    // start reading/writing -- UNIX lseek

    int Read(char* into, int numBytes); // Read/write bytes from the file,
                    // starting at the implicit position.
                    // Return the # actually read/written,
                    // and increment position in file.
    int Write(char* from, int numBytes);

    int ReadAt(char* into, int numBytes, int position);
    // Read/write bytes from the file,
// bypassing the implicit position.
    int WriteAt(char* from, int numBytes, int position);

    int Length(); 			// Return the number of bytes in the
                    // file (this interface is simpler 
                    // than the UNIX idiom -- lseek to 
                    // end of file, tell, lseek back 

private:
    FileHeader* hdr;			// Header for this file 
    int seekPosition;			// Current position within the file
};

#endif // FILESYS

#endif // OPENFILE_H
