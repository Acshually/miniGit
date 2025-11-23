/**
 * minigit.h
 * * Header file for the MiniGit namespace.
 * It declares all the core functions and constants needed
 * for our version control system.
 * * Requires C++17 or later for <filesystem>
 */

#ifndef MINIGIT_H
#define MINIGIT_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <filesystem> // C++17 standard library for file system operations

// Define our file paths as constants
namespace MiniGit {
    
    // Use .minigit to avoid conflicts with a real .git folder
    const std::filesystem::path GIT_DIR = ".minigit";
    const std::filesystem::path OBJECTS_DIR = GIT_DIR / "objects";
    const std::filesystem::path HEAD_FILE = GIT_DIR / "HEAD";
    const std::filesystem::path INDEX_FILE = GIT_DIR / "index"; // Our staging area

    // --- Core Commands ---

    /**
     * @brief Initializes a new MiniGit repository in the current directory.
     * Creates the .minigit directory structure.
     */
    void init();

    /**
     * @brief Adds one or more files to the staging area (index).
     * This reads the file content, hashes it, saves it as a "blob"
     * object, and adds the file's name to the index.
     * @param filenames A vector of filepaths to add.
     */
    void add(const std::vector<std::string>& filenames);

    /**
     * @brief Creates a new commit from the staged files.
     * This creates a "commit" object that points to the previous
     * commit (its parent) and a "tree" (snapshot) of the staged files.
     * @param message The commit message.
     */
    void commit(const std::string& message);

    /**
     * @brief Displays the commit history, starting from HEAD.
     * This traverses the "commit" linked list.
     */
    void log();


    // --- Helper Functions ---

    /**
     * @brief Checks if a .minigit repository exists in the current directory.
     * @return true if it exists, false otherwise.
     */
    bool repoExists();

    /**
     * @brief Reads the content of a file into a string.
     * @param filename The path to the file.
     * @return The content of the file.
     */
    std::string readFileContent(const std::filesystem::path& filename);

    /**
     * @brief Writes content to a specific file.
     * @param filepath The path to the file to be written.
     * @param content The string content to write.
     */
    void writeFileContent(const std::filesystem::path& filepath, const std::string& content);

    /**
     * @brief Hashes a string content using std::hash.
     * This is our content-addressing mechanism.
     * @param content The string content to hash.
     * @return A string representation of the hash.
     */
    std::string hashString(const std::string& content);

    /**
     * @brief Reads the staging area (index file).
     * DATA STRUCTURE: Uses std::map as a hash map (filename -> content hash)
     * @return A map of staged files (filename: hash).
     */
    std::map<std::string, std::string> getStagingArea();

    /**
     * @brief Writes to the staging area (index file).
     * DATA STRUCTURE: Uses std::map as a hash map (filename -> content hash)
     * @param stagedFiles The map of staged files to save.
     */
    void setStagingArea(const std::map<std::string, std::string>& stagedFiles);

    /**
     * @brief Gets the hash of the current commit (from HEAD file).
     * @return The commit hash, or an empty string if no commits yet.
     */
    std::string getHEAD();

    /**
     * @brief Sets the HEAD to point to a new commit hash.
     * @param commitHash The hash of the new commit.
     */
    void setHEAD(const std::string& commitHash);

    /**
     * @brief Reads a commit object and returns its parent's hash.
     * @param commitHash The hash of the commit to read.
     * @return The hash of the parent commit.
     */
    std::string getCommitParent(const std::string& commitHash);

    /**
     * @brief Reads a commit object and returns its file map (tree).
     * DATA STRUCTURE: Uses std::map as a hash map (filename -> content hash)
     * @param commitHash The hash of the commit to read.
     * @return A map of files (filename: hash) in that commit.
     */
    std::map<std::string, std::string> getCommitFiles(const std::string& commitHash);

    void checkout(const std::string& commitHash);

} // namespace MiniGit

#endif // MINIGIT_H
