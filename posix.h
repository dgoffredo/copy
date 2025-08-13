#pragma once

#include <string_view>

namespace posix {

// Read from the file associated with the file descriptor, `fd`, `count` bytes
// into the buffer referred to by `destination`. Return `count`, or a smaller
// number of bytes if the end of the file was reached. Return `-errno` if an
// error occurs.
int read_all(int fd, char* destination, int count);

// Write into the file associated with the file descriptor, `fd`, `count` bytes
// from the buffer referred to by `source`. Return `count` on success, or
// return `-errno` if an error occurs. Note that there is a possibility that
// more than zero but fewer than `count` bytes may be written when an error
// occurs, such as if the destination file becomes full.
int write_all(int fd, const char* source, int count);

// Open the existing file indicated by its `path` on the file system and return
// a file descriptor to that file open for reading. Return `-errno` if an error
// occurs.
int open_for_reading(const char* path);

// Open or create a file indicated by its `path` on the file system and return
// a file descriptor to that file open for writing. If the file already exists,
// then truncate its contents. If the file does not already exist, then create
// it with `mode` (permissions). Return `-errno` if an error occurs.
int open_for_writing(const char* path, unsigned mode);

// Close the file associated with the file descriptor, `fd`.
void close(int fd);

struct FileModeResult {
    int error;
    unsigned mode;
};

// Get the mode of the file associated with the open file descriptor `fd`.
// Return `{.error=0, .mode=mode}` with the file's `mode` on success, or return
// `{.error=errno, .mode=0}` if an error occurs.
FileModeResult file_mode(int fd);

// Return the size of a memory page, in bytes.
int page_size();

} // namespace posix
