#pragma once

#include <cstddef>

namespace posix {

// Read from the file associated with the file descriptor, `fd`, `count` bytes
// into the buffer referred to by `destination`. Return `count`, or a smaller
// number of bytes if the end of the file was reached. Return `-errno` if an
// error occurs.
int read_all(int fd, char* destination, std::size_t count);

// Write into the file associated with the file descriptor, `fd`, `count` bytes
// from the buffer referred to by `source`. Return `count` on success, or
// return `-errno` if an error occurs. Note that there is a possibility that
// more than zero but fewer than `count` bytes may be written when an error
// occurs, such as if the destination file becomes full.
int write_all(int fd, const char* source, std::size_t count);

// Open the existing file indicated by its `path` on the file system and return
// a file descriptor to that file open for reading. Return `-errno` if an error
// occurs.
int open_for_reading(const char* path);

// Open or create a file indicated by its `path` on the file system and return
// a file descriptor to that file open for writing. If the file already exists,
// then truncate its contents. If the file does not already exist, then create
// it with `mode` (permissions). Return `-errno` if an error occurs.
int open_for_writing(const char* path, unsigned mode);

// Open or create a file indicated by its `path` on the file system and return
// a file descriptor to that file open for reading and writing. If the file
// already exists, then truncate its contents. If the file does not already
// exist, then create it with `mode` (permissions). Return `-errno` if an error
// occurs.
int open_for_reading_and_writing(const char* path, unsigned mode);

// Close the file associated with the file descriptor, `fd`.
void close_file(int fd);

struct FileStatus {
    unsigned mode;
    std::size_t size;
};

struct FileStatusResult {
    int error;
    FileStatus status;
};

// Get the metadata (status, "stat") of the file associated with the open file
// descriptor `fd`. Return `{.error=0, ...}` on success, or return
// `{.error=errno, ...}` if an error occurs.
FileStatusResult file_status(int fd);

// Return the size of a memory page, in bytes.
std::size_t page_size();

struct MemoryMapResult {
    int error;
    void* address;
    int fd;
};

// Map the file associated with file descriptor `fd` to a region of readable
// memory that is `count` bytes in size. On success, return
// `{.error=0, .address=address, .fd=fd}` with the starting `address` of the
// mapped region of memory, or return
// `{.error=errno, .address=nullptr, .fd=fd}` if an error occurs.
MemoryMapResult memory_map_for_reading(int fd, std::size_t count);

// Open or create a file indicated by its `path` on the file system, resize it
// to `count` bytes of unspecified data, and map the file to a region of
// writable memory that is `count` bytes in size. If the file does not already
// exist, then create it with `mode` (permissions). On success, return
// `{.error=0, .address=address, .fd=fd}` with the starting `address` of the
// mapped region of memory and the associated file descriptor `fd`, or return
// `{.error=errno, .address=nullptr, .fd=-1}` if an error occurs.
MemoryMapResult open_and_memory_map_for_writing(const char* path, unsigned mode, std::size_t count);

// Commit to the associated file any writes made to the mapped memory region
// begining at `address` and having length `count` bytes and wait for the
// resulting file I/O to finish. Return zero on success, or return `errno` if
// an error occurs.
int memory_sync(void* address, std::size_t count);

// Remove the memory mapping associated with the region of memory beginning at
// `address` and having length `count` bytes. Return zero on success, or return
// `errno` if an error occurs.
int memory_unmap(void* address, std::size_t count);

} // namespace posix
