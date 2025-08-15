#include "posix.h"

#include <cerrno>

#include <copyfile.h>

namespace posix {

int copy_all(const char* source_path, const char* destination_path) {
    copyfile_state_t state = ::copyfile_state_alloc();
    const int rc = ::copyfile(source_path, destination_path, state, COPYFILE_ALL);
    ::copyfile_state_free(state);
    if (rc < 0) {
        return -errno;
    }
    return 0;
}

} // namespace posix
