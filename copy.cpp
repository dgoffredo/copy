#include "posix.h"

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

    if (const int rc = posix::copy_all(options.source.c_str(), options.destination.c_str())) {
        std::cerr << "Unable to copy bytes from \"" << options.source << "\" to \"" << options.destination << "\": " << std::strerror(rc) << '\n';
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
    if (argc < 1 + 1 || argc > 1 + 1 + 2) {
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
