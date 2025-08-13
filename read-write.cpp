#include "posix.h"

#include <cstring>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

struct Options {
    bool help = false;
    std::string source;
    std::string destination;
    int buffer_size = posix::page_size();
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
            posix::close(fd);
        }
    };

    const int source_fd = posix::open_for_reading(options.source.c_str());
    if (source_fd < 0) {
        std::cerr << "Unable to open \"" << options.source << "\" for reading: " << std::strerror(-source_fd) << '\n';
        return 1;
    }
    Closer source_closer{source_fd};
    
    const auto [error, mode] = posix::file_mode(source_fd);
    if (error) {
        std::cerr << "Unable to determine the file mode of \"" << options.source << "\": " << std::strerror(error) << '\n';
        return 1;
    }

    const int destination_fd = posix::open_for_writing(options.destination.c_str(), mode);
    if (destination_fd < 0) {
        std::cerr << "Unable to open or create \"" << options.destination << "\" for writing: " << std::strerror(-destination_fd) << '\n';
        return 1;
    }
    Closer destination_closer{destination_fd};

    std::vector<char> buffer(options.buffer_size);
    for (;;) {
        const int count = posix::read_all(source_fd, buffer.data(), buffer.size());
        if (count < 0) {
            std::cerr << "read error: " << std::strerror(-count) << '\n';
            return 1;
        }
        if (count == 0) {
            // end of input file: we're done
            break;
        }
        const int rc = posix::write_all(destination_fd, buffer.data(), count);
        if (rc < 0) {
            std::cerr << "write error: " << std::strerror(-rc) << '\n';
            return 1;
        }
    }
}

void usage(std::string_view program_name, std::ostream& out) {
    out << "usage:\n\n"
        "    " << program_name << " [--help | -h] [--buffer BUFSIZE] <source file> <destination file>\n\n"
        "        --help or -h prints this message.\n"
        "        BUFSIZE is the read/write buffer size in bytes. It defaults to one page.\n"
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
        } else if (arg == "--buffer") {
            ++argv;
            if (!*argv) {
                usage(program_name, error);
                error << "\nerror: --buffer requires an integer argument.\n";
                return 1;
            }
            try {
                options.buffer_size = std::stoi(*argv);
            } catch (const std::exception&) {
                usage(program_name, error);
                error << "\nerror: \"" << *argv << "\" is not a valid integer argument for --buffer\n";
                return 1;
            }
            if (options.buffer_size < 1) {
                usage(program_name, error);
                error << "\nerror: --buffer argument must be at least 1.\n";
                return 1;
            }
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
