/*

	Copyright 2014 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <iostream>
#include <fstream>
#ifdef WIN32
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#else
#include <sys/time.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>

#include "ogldev_util.h"

bool ReadFile(const char* pFileName, string& outFile)
{
    ifstream f(pFileName);
    
    bool ret = false;
    
    if (f.is_open()) {
        string line;
        while (getline(f, line)) {
            outFile.append(line);
            outFile.append("\n");
        }
        
        f.close();
        
        ret = true;
    }
    else {
        OGLDEV_FILE_ERROR(pFileName);
    }
    
    return ret;
}


#ifdef WIN32

//char* ReadBinaryFileWin(const char* pFileName, int& size)
//{
//    HANDLE fileHandle = CreateFileA(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//
//    if (fileHandle == INVALID_HANDLE_VALUE) {
//        OGLDEV_FILE_ERROR(pFileName);
//        return false;
//    }
//
//    size = GetFileSize(fileHandle, NULL);
//
//    if (size == INVALID_FILE_SIZE) {
//        OGLDEV_ERROR("Invalid file size %s\n", pFileName);
//        return false;
//    }
//
//	char* pBuffer = (char*)malloc(size);
//    assert(pBuffer);
//    // wip for tutorial51
//    //assert(0);
//    ZeroMemory(pBuffer,size);
//    DWORD dwBytesToRead = size;
//    DWORD dwBytesRead = 0;
//    char* pTmpBuf = pBuffer;
//
//    do{                                       
//
//		ReadFile(fileHandle,pTmpBuf,dwBytesRead,&dwBytesRead,NULL);
//		
//        if (dwBytesRead == 0)
//            break;
//
//        dwBytesToRead -= dwBytesRead;
//        pTmpBuf += dwBytesRead;
//
//        } while (dwBytesToRead > 0);
//	 //free(p);
//	CloseHandle(fileHandle);
//	return pBuffer;
//}
char* ReadBinaryFileStd(const char* pFileName, int& size){
	FILE* pFile=fopen(pFileName,"r");
	if (pFile == NULL) {
        OGLDEV_ERROR("Error opening '%s': %s\n", pFileName, strerror(errno));
        return NULL;        
    }
	fseek(pFile, 0, SEEK_END); // seek to end of file
	size = ftell(pFile); // get current file pointer
	fseek(pFile, 0, SEEK_SET); // seek back to beginning of file
	char* pBuffer = (char*)malloc(size);
    assert(pBuffer);
	fread(pBuffer,size,1,pFile);
	fclose(pFile);
	return pBuffer;
}

char* ReadBinaryFile(const char* pFileName, int& size){
	//return ReadBinaryFileWin(pFileName,size);
	return ReadBinaryFileStd(pFileName,size);
}
#else
char* ReadBinaryFile(const char* pFileName, int& size)
{
    int f = open(pFileName, O_RDONLY);
    
    if (f == -1) {
        OGLDEV_ERROR("Error opening '%s': %s\n", pFileName, strerror(errno));
        return NULL;        
    }
    
    struct stat stat_buf;
    int error = stat(pFileName, &stat_buf);
    
    if (error) {
        OGLDEV_ERROR("Error getting file stats: %s\n", strerror(errno));
        return NULL;
    }
    
    size = stat_buf.st_size;
       
    char* p = (char*)malloc(size);
    assert(p);
    
    int read_len = read(f, p, size);
    
    if (read_len != size) {
        OGLDEV_ERROR("Error reading file: %s\n", strerror(errno));
        return NULL;        
    }
    
    close(f);
    
    return p;
}
#endif

void OgldevError(const char* pFileName, uint line, const char* format, ...)
{
    char msg[1000];    
    va_list args;
    va_start(args, format);
    VSNPRINTF(msg, sizeof(msg), format, args);
    va_end(args);

#ifdef WIN32 
    char msg2[1000];
    _snprintf_s(msg2, sizeof(msg2), "%s:%d: %s", pFileName, line, msg);
    MessageBoxA(NULL, msg2, NULL, 0);
#else
    fprintf(stderr, "%s:%d - %s", pFileName, line, msg);
#endif    
}


void OgldevFileError(const char* pFileName, uint line, const char* pFileError)
{
#ifdef WIN32
    char msg[1000];
    _snprintf_s(msg, sizeof(msg), "%s:%d: unable to open file `%s`", pFileName, line, pFileError);
    MessageBoxA(NULL, msg, NULL, 0);
#else
    fprintf(stderr, "%s:%d: unable to open file `%s`\n", pFileName, line, pFileError);
#endif    
}


long long GetCurrentTimeMillis()
{
#ifdef WIN32    
    return GetTickCount();
#else
    timeval t;
    gettimeofday(&t, NULL);

    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
#endif    
}

#ifdef WIN32
#if (_MSC_VER == 1900)
#elif (_MSC_VER == 1800)
#else
float fmax(float a, float b)
{
    if (a > b)
        return a;
    else
        return b;
}
#endif
#endif