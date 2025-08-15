#include "posix.h"

#include <cassert>
#include <cerrno>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace posix {

int read_all(int fd, char* destination, std::size_t count) {
    std::size_t total = 0;
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

int write_all(int fd, const char* source, std::size_t count) {
    std::size_t total = 0;
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

int open_for_reading_and_writing(const char* path, unsigned mode) {
    int fd;
    do {
        fd = ::open(path, O_RDWR | O_CREAT | O_TRUNC, static_cast<mode_t>(mode));
    } while (fd == -1 && errno == EINTR);
    return fd  == -1 ? -errno : fd;
}

void close_file(int fd) {
    int rc;
    do {
        rc = ::close(fd);
    } while (rc == -1 && errno == EINTR);
    // All other errors are ignored.
}

FileStatusResult file_status(int fd) {
    struct stat file_info;
    if (::fstat(fd, &file_info)) {
        return {.error = errno, .status = {.mode = 0, .size = 0}};
    }
    return {.error = 0, .status = {.mode = file_info.st_mode, .size = std::size_t(file_info.st_size)}};
}

std::size_t page_size() {
    const long rc = ::sysconf(_SC_PAGE_SIZE);
    assert(rc != -1);
    return rc;
}

MemoryMapResult memory_map_for_reading(int fd, std::size_t count) {
    const int protection = PROT_READ;
    const int flags = MAP_PRIVATE;
    const off_t offset = 0;
    void* address = ::mmap(nullptr, count, protection, flags, fd, offset);
    if (address == MAP_FAILED) {
        return {.error=errno, .address=nullptr, .fd=fd};
    }
    return {.error=0, .address=address, .fd=fd};
}

MemoryMapResult open_and_memory_map_for_writing(const char* path, unsigned mode, std::size_t count) {
    const int fd = open_for_reading_and_writing(path, mode);
    if (fd == -1) {
        return {.error=errno, .address=nullptr, .fd=-1};
    }

    int rc;
    do {
        rc = ::ftruncate(fd, count);
    } while (rc == -1 && errno == EINTR);
    if (rc == -1) {
        close_file(fd);
        return {.error=errno, .address=nullptr, .fd=-1};
    }

    const int protection = PROT_WRITE;
    const int flags = MAP_SHARED;
    const off_t offset = 0;
    void* address = ::mmap(nullptr, count, protection, flags, fd, offset);
    if (address == MAP_FAILED) {
        close_file(fd);
        return {.error=errno, .address=nullptr, .fd=-1};
    }

    return {.error=0, .address=address, .fd=fd};
}

int memory_sync(void* address, std::size_t count) {
    const int flags = MS_SYNC;
    if (::msync(address, count, flags)) {
        return errno;
    }
    return 0;
}

int memory_unmap(void* address, std::size_t count) {
    if (::munmap(address, count)) {
        return errno;
    }
    return 0;
}

} // namespace posix
