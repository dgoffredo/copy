#include "posix.h"

#include <cassert>
#include <cerrno>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace posix {

int read_all(int fd, char* destination, int count) {
    int total = 0;
    while (total < count) {
        const int rc = ::read(fd, destination + total, count - total);
        if (rc == -1 && errno == EINTR) {
            continue; // interrupted by signal before a byte was read, try again
        } else if (rc == -1) {
            return -errno; // error, return a negative error code
        } else if (rc == 0) {
            break; // end of file, stop reading
        }
        total += rc; // got some bytes
    }
    return total;
}

int write_all(int fd, const char* source, int count) {
    int total = 0;
    while (total < count) {
        const int rc = ::write(fd, source + total, count - total);
        if (rc == -1 && errno == EINTR) {
            continue; // interrupted by signal before a byte was written, try again
        } else if (rc == -1) {
            return -errno; // error, return a negative error code
        }
        total += rc;
    }
    return total;
}

int open_for_reading(const char* path) {
    int fd;
    do {
        fd = ::open(path, O_RDONLY);
    } while (fd == -1 && errno == EINTR);
    return fd  == -1 ? -errno : fd;
}

int open_for_writing(const char* path, unsigned mode) {
    int fd;
    do {
        fd = ::open(path, O_WRONLY | O_CREAT | O_TRUNC, static_cast<mode_t>(mode));
    } while (fd == -1 && errno == EINTR);
    return fd  == -1 ? -errno : fd;
}

void close(int fd) {
    int rc;
    do {
        rc = ::close(fd);
    } while (rc == -1 && errno == EINTR);
    // All other errors are ignored.
}

FileModeResult file_mode(int fd) {
    struct stat file_info;
    if (::fstat(fd, &file_info)) {
        return {.error = errno, .mode = 0};
    }
    return {.error = 0, .mode = file_info.st_mode};
}

int page_size() {
    // `sysconf` returns `long`, not `int`. The POSIX (OpenGroup) documentation
    // notes that this is particularly relevant for page size due to possible
    // future gigantic values for page size. But in a 32-bit `int` world, I'm
    // not trying to support page sizes larger than 2 gigabytes.
    // Wiser would probably be to use `size_t` and `ssize_t` for unsigned and
    // signed I/O quantities, respectively. But I like `int`.
    const int rc = ::sysconf(_SC_PAGE_SIZE);
    assert(rc != -1);
    return rc;
}

} // namespace posix
