#include "posix.h"

#include <iostream>
#include <ostream>
#include <string_view>

struct Options {
    bool help = false;
    std::string_view source;
    std::string_view destination;
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

    // TODO
}

void usage(std::string_view name, std::ostream& out) {
    out << "usage:\n\n"
        "    " << name << " [--help | -h] [--buffer BUFSIZE] <source file> <destination file>\n\n"
        "        --help or -h prints this message.\n"
        "        BUFSIZE is the read/write buffer size in bytes. It defaults to one page.\n"
        "        <source file> is the path to the input file, to be read from.\n"
        "        <destination file> is the path to the output file, to be created/truncated and written to.\n";
}

int parse_command_line(Options& options, int argc, char* argv[], std::ostream& out, std::ostream& error) {
    if (argc < 1 + 1 || argc > 1 + 1 + 1 + 2) {
        usage(argv[0], error);
        return 1;
    }
    // TODO
    (void)options;
    (void)argv;
    (void)out;
    return 0;
}
