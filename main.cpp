/**
 * main.cpp
 * * The command-line interface (CLI) for MiniGit.
 * This file is responsible for parsing the user's commands
 * (e.g., "init", "add", "commit") and calling the
 * corresponding functions from the MiniGit library.
 */

#include "minigit.h"
#include <iostream>
#include <vector>
#include <string>

// Helper function to print usage instructions
void printUsage() {
    std::cerr << "Usage: minigit <command> [options]\n"
              << "\n"
              << "Available commands:\n"
              << "  init                  Create an empty MiniGit repository\n"
              << "  add <file1> [file2]...  Add file(s) to the staging area\n"
              << "  commit -m \"<message>\"   Record changes to the repository\n"
              << "  log                   Show the commit history\n"
              << std::endl;
}

// Entry point of the program
int main(int argc, char* argv[]) {
    // Need at least one command
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string command = argv[1];

    try {
        if (command == "init") {
            MiniGit::init();
        } else if (command == "add") {
            // Check if we are in a repo
            if (!MiniGit::repoExists()) {
                std::cerr << "Fatal: Not a MiniGit repository. (Run 'minigit init' first)" << std::endl;
                return 1;
            }
            if (argc < 3) {
                std::cerr << "Usage: minigit add <file1> [file2]..." << std::endl;
                return 1;
            }
            // Add all files listed
            std::vector<std::string> filesToAdd;
            for (int i = 2; i < argc; ++i) {
                filesToAdd.push_back(argv[i]);
            }
            MiniGit::add(filesToAdd);
        } else if (command == "commit") {
            // Check if we are in a repo
            if (!MiniGit::repoExists()) {
                std::cerr << "Fatal: Not a MiniGit repository." << std::endl;
                return 1;
            }
            if (argc != 4 || std::string(argv[2]) != "-m") {
                std::cerr << "Usage: minigit commit -m \"<message>\"" << std::endl;
                return 1;
            }
            std::string message = argv[3];
            MiniGit::commit(message);
        } else if (command == "log") {
            // Check if we are in a repo
            if (!MiniGit::repoExists()) {
                std::cerr << "Fatal: Not a MiniGit repository." << std::endl;
                return 1;
            }
            MiniGit::log();
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            printUsage();
            return 1;
        }
    } catch (const std::exception& e) {
        // Catch any errors thrown from our functions
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
