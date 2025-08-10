namespace posix {

// Read from the file associated with the file descriptor, `fd`, `count` bytes
// into the buffer referred to by `destination`. Return `count`, or a smaller
// count if the end of the file was reached. Return `-errno` if an error
// occurs.
int read_all(int fd, char* destination, int count);

// Write into the file associated with the file descriptor, `fd`, `count` bytes
// from the buffer referred to by `source`. Return `count` on success, or
// return `-errno` if an error occurs. Note that there is a possibility that
// more than zero but fewer than `count` bytes may be written when an error
// occurs, such as if the destination file becomes full.
int write_all(int fd, const char* source, int count);

// Return the size of a memory page, in bytes.
int page_size();

} // namespace posix
