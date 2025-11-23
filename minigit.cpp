/**
 * minigit.cpp
 * * Implementation of the MiniGit functions.
 * This is where all the DSA concepts come to life!
 */

#include "minigit.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional> // For std::hash
#include <stdexcept> // For std::runtime_error

// --- ADD THIS LINE ---
// This line tells the compiler that "fs" is an alias for "std::filesystem"
namespace fs = std::filesystem;
// ---------------------

namespace MiniGit {

    // --- Core Commands Implementation ---

    void init() {
        if (repoExists()) {
            std::cout << "MiniGit repository already initialized in " << GIT_DIR << std::endl;
            return;
        }
        
        // Create the main .minigit directory
        fs::create_directory(GIT_DIR);
        // Create the 'objects' directory for content-addressed storage
        fs::create_directory(OBJECTS_DIR);
        
        // Create HEAD file, initially empty (no commits)
        setHEAD(""); 
        // Create index file, initially empty (nothing staged)
        setStagingArea({}); 
        
        std::cout << "Initialized empty MiniGit repository in " << fs::absolute(GIT_DIR) << std::endl;
    }

    void add(const std::vector<std::string>& filenames) {
        // Load the current staging area
        std::map<std::string, std::string> stagedFiles = getStagingArea();
        
        for (const auto& filename : filenames) {
            fs::path filepath = filename;
            
            if (!fs::exists(filepath)) {
                std::cerr << "File not found: " << filename << ". Skipping." << std::endl;
                continue;
            }
            if (fs::is_directory(filepath)) {
                std::cerr << "Cannot add directories: " << filename << ". Skipping." << std::endl;
                continue;
            }

            // 1. Read file content
            std::string content = readFileContent(filepath);
            
            // 2. DSA: HASHING
            // Hash the content to get a unique ID
            std::string hash = hashString(content);
            
            // 3. Store the content in the 'objects' directory
            // This is our "blob" object.
            fs::path objectPath = OBJECTS_DIR / hash;
            if (!fs::exists(objectPath)) {
                writeFileContent(objectPath, content);
            }
            
            // 4. DSA: HASH MAP (std::map)
            // Add or update the file in our staging area (map)
            stagedFiles[filename] = hash;
            
            std::cout << "Staged " << filename << std::endl;
        }
        
        // Save the updated staging area back to the index file
        setStagingArea(stagedFiles);
    }

    void commit(const std::string& message) {
        // 1. Load the staging area
        std::map<std::string, std::string> stagedFiles = getStagingArea();
        
        if (stagedFiles.empty()) {
            std::cout << "Nothing to commit, working tree clean." << std::endl;
            return;
        }

        // 2. Get the current HEAD (parent commit)
        // This is the "pointer" for our linked list
        std::string parentCommit = getHEAD();

        // 3. DSA: HASH MAP (std::map)
        // Create the new commit's file map (tree).
        // Start by inheriting all files from the parent commit.
        std::map<std::string, std::string> commitFiles;
        if (!parentCommit.empty()) {
            commitFiles = getCommitFiles(parentCommit);
        }

        // 4. Update the map with the newly staged files
        for (const auto& pair : stagedFiles) {
            commitFiles[pair.first] = pair.second;
        }
        
        // 5. Build the "commit object" content
        std::stringstream commitContent;
        
        // DSA: LINKED LIST
        // Add a pointer to the parent commit
        commitContent << "parent: " << parentCommit << "\n";
        commitContent << "message: " << message << "\n";
        
        // Add all the tracked files (our "tree")
        for (const auto& pair : commitFiles) {
            commitContent << "file: " << pair.first << " " << pair.second << "\n";
        }
        
        // 6. DSA: HASHING
        // Hash the commit object itself to get its unique ID
        std::string commitString = commitContent.str();
        std::string commitHash = hashString(commitString);
        
        // 7. Save the commit object
        writeFileContent(OBJECTS_DIR / commitHash, commitString);
        
        // 8. DSA: LINKED LIST
        // Update HEAD to point to this new commit.
        // We are inserting at the head of the list.
        setHEAD(commitHash);
        
        // 9. Clear the staging area
        setStagingArea({});
        
        std::cout << "Committed [" << commitHash << "] " << message << std::endl;
    }

    void log() {
        // 1. Get the current HEAD (start of the linked list)
        std::string currentCommitHash = getHEAD();
        
        if (currentCommitHash.empty()) {
            std::cout << "No commits yet." << std::endl;
            return;
        }
        
        // 2. DSA: LINKED LIST TRAVERSAL
        // Loop as long as we have a valid commit hash
        while (!currentCommitHash.empty()) {
            fs::path commitPath = OBJECTS_DIR / currentCommitHash;
            if (!fs::exists(commitPath)) {
                std::cerr << "Fatal: Missing commit object " << currentCommitHash << std::endl;
                break;
            }
            
            std::string commitContent = readFileContent(commitPath);
            std::stringstream ss(commitContent);
            std::string line;
            
            std::string parentHash = "";
            std::string message = "";
            
            // Read the commit object line by line
            while (std::getline(ss, line)) {
                if (line.rfind("parent: ", 0) == 0) {
                    parentHash = line.substr(8);
                } else if (line.rfind("message: ", 0) == 0) {
                    message = line.substr(9);
                }
            }
            
            // Print commit info
            std::cout << "commit " << currentCommitHash << "\n";
            std::cout << "    " << message << "\n" << std::endl;
            
            // 3. Move to the next node in the list
            currentCommitHash = parentHash;
        }
    }


    // --- Helper Functions Implementation ---

    bool repoExists() {
        return fs::exists(GIT_DIR) && fs::is_directory(GIT_DIR);
    }

    std::string readFileContent(const fs::path& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename.string());
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    void writeFileContent(const fs::path& filepath, const std::string& content) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not write to file: " + filepath.string());
        }
        file << content;
    }

    std::string hashString(const std::string& content) {
        // Use std::hash to generate a hash value (a number)
        std::hash<std::string> hasher;
        size_t hashVal = hasher(content);
        
        // Convert the number to a hexadecimal string
        std::stringstream ss;
        ss << std::hex << hashVal;
        return ss.str();
    }

    std::map<std::string, std::string> getStagingArea() {
        // DSA: The index file *is* our staging area.
        // We parse it into a std::map (Hash Map) for easy manipulation.
        std::map<std::string, std::string> stagedFiles;
        if (!fs::exists(INDEX_FILE)) {
            return stagedFiles;
        }
        
        std::ifstream file(INDEX_FILE);
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string filename, hash;
            if (ss >> filename >> hash) {
                stagedFiles[filename] = hash;
            }
        }
        return stagedFiles;
    }

    void setStagingArea(const std::map<std::string, std::string>& stagedFiles) {
        // Write the std::map (Hash Map) back to the index file.
        std::ofstream file(INDEX_FILE);
        for (const auto& pair : stagedFiles) {
            file << pair.first << " " << pair.second << "\n";
        }
    }

    std::string getHEAD() {
        if (!fs::exists(HEAD_FILE)) {
            return "";
        }
        return readFileContent(HEAD_FILE);
    }

    void setHEAD(const std::string& commitHash) {
        writeFileContent(HEAD_FILE, commitHash);
    }

    std::map<std::string, std::string> getCommitFiles(const std::string& commitHash) {
        // This function parses a commit object and reconstructs its file map
        std::map<std::string, std::string> files;
        if (commitHash.empty()) {
            return files;
        }
        
        fs::path commitPath = OBJECTS_DIR / commitHash;
        if (!fs::exists(commitPath)) {
            throw std::runtime_error("Cannot find commit object: " + commitHash);
        }
        
        std::stringstream ss(readFileContent(commitPath));
        std::string line;
        
        while (std::getline(ss, line)) {
            if (line.rfind("file: ", 0) == 0) {
                std::stringstream line_ss(line.substr(6));
                std::string filename, hash;
                if (line_ss >> filename >> hash) {
                    files[filename] = hash;
                }
            }
        }
        return files;
    }

} // namespace MiniGit

