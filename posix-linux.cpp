#include "posix.h"

#include <cerrno>

#include <sys/sendfile.h>

namespace posix {

int copy_all(const char* source_path, const char* destination_path) {
    class Closer {
        int fd;
     public:
        explicit Closer(int fd) : fd(fd) {}
        ~Closer() {
            close_file(fd);
        }
    };

    const int source_fd = open_for_reading(source_path);
    if (source_fd < 0) {
        return -source_fd;
    }
    Closer source_closer{source_fd};
    
    const auto [error, status] = posix::file_status(source_fd);
    if (error) {
        return error;
    }

    const int destination_fd = posix::open_for_writing(destination_path, status.mode);
    if (destination_fd < 0) {
        return -destination_fd;
    }
    Closer destination_closer{destination_fd};

    std::size_t total = 0;
    while (total < status.size) {
        off_t* const offset = nullptr;
        const ssize_t rc = ::sendfile(destination_fd, source_fd, offset, status.size - total);
        if (rc == -1) {
            return errno;
        }
        total += rc;
    }
    return 0;
}

} // namespace posix
