#include "posix.h"

#include <algorithm>
#include <cstring>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>

struct Options {
    bool help = false;
    std::string source;
    std::string destination;
};

void usage(std::string_view name, std::ostream& out);

int parse_command_line(Options& options, int argc, char* argv[], std::ostream& out, std::ostream& error);

int main(int argc, char* argv[]) {
    Options options;
    if (const int rc = parse_command_line(options, argc, argv, std::cout, std::cerr)) {
        return rc;
    } else if (options.help) {
        return 0; // `parse_command_line` printed the usage already
    }

    class Closer {
        int fd;
     public:
        explicit Closer(int fd) : fd(fd) {}
        ~Closer() {
            posix::close_file(fd);
        }
    };

    class Unmapper {
        void *address;
        std::size_t count;
        std::string path;
     public:
        Unmapper(void* address, std::size_t count, const std::string& path) : address(address), count(count), path(path) {}
        ~Unmapper() {
            if (const int rc = posix::memory_unmap(address, count)) {
                std::cerr << "Failed to unmap memory for \"" << path << "\": " << std::strerror(rc) << '\n';
            }
        }
    };

    const int source_fd = posix::open_for_reading(options.source.c_str());
    if (source_fd < 0) {
        std::cerr << "Unable to open \"" << options.source << "\" for reading: " << std::strerror(-source_fd) << '\n';
        return 1;
    }
    Closer source_closer{source_fd};
    
    const auto [error, status] = posix::file_status(source_fd);
    if (error) {
        std::cerr << "Unable to determine the file mode/size of \"" << options.source << "\": " << std::strerror(error) << '\n';
        return 1;
    }

    const auto dest = posix::open_and_memory_map_for_writing(options.destination.c_str(), status.mode, status.size);
    if (dest.error) {
        std::cerr << "Unable to open/mmap \"" << options.destination << "\" for writing: " << std::strerror(dest.error) << '\n';
        return 1;
    }
    Closer destination_closer{dest.fd};
    Unmapper destination_unmapper{dest.address, status.size, options.destination};

    const int count = posix::read_all(source_fd, static_cast<char*>(dest.address), status.size);
    if (count < 0) {
        std::cerr << "read error: " << std::strerror(-count) << '\n';
        return 1;
    }

    if (const int rc = posix::memory_sync(dest.address, status.size)) {
        std::cerr << "Unable to synchronize written memory region to \"" << options.destination << "\": " << std::strerror(rc) << '\n';
        return 1;
    }
}

void usage(std::string_view program_name, std::ostream& out) {
    out << "usage:\n\n"
        "    " << program_name << " [--help | -h] <source file> <destination file>\n\n"
        "        --help or -h prints this message.\n"
        "        <source file> is the path to the input file, to be read from.\n"
        "        <destination file> is the path to the output file, to be created/truncated and written to.\n";
}

int parse_command_line(Options& options, int argc, char* argv[], std::ostream& out, std::ostream& error) {
    const std::string_view program_name =  argv[0];
    if (argc < 1 + 1 || argc > 1 + 1 + 1 + 2) {
        usage(program_name, error);
        return 1;
    }

    bool found_source = false;
    bool found_destination = false;

    for (++argv; *argv; ++argv) {
        const std::string_view arg = *argv;
        if (arg == "--help" || arg == "-h") {
            options.help = true;
            usage(program_name, out);
            return 0;
        } else if (arg.substr(0, 1) == "-") {
            usage(program_name, error);
            error << "\nerror: Unknown option \"" << arg << "\". If you meant a file name, use \"./" << arg << "\".\n";
            return 1;
        } else if (found_source && found_destination) {
            usage(program_name, error);
            return 1;
        } else if (found_source) {
            options.destination = arg;
            found_destination = true;
        } else {
            options.source = arg;
            found_source = true;
        }
    }

    if (!found_destination) {
        usage(program_name, error);
        error << "\nsource file and destination file arguments are required.\n";
        return 1;
    }

    return 0;
}
