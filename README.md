MiniGit - A C++ Version Control System

This is a simplified, educational implementation of a Git-like version control system, written in C++ for a Data Structures and Algorithms (DSA) lab.

It implements the core commands: init, add, commit, and log.

Core DSA Concepts Used

This project is built directly on top of several fundamental DSA concepts:

Linked List (for Commit History):

Every commit object stores the hash of its parent commit.

This creates a singly linked list, where HEAD points to the newest commit (the head of the list).

The log command works by performing a linked list traversal, starting at HEAD and following the parent pointers until it reaches the initial (empty) commit.

The commit command performs a head insertion on this linked list.

Hashing (for Content-Addressing):

We use std::hash to generate a unique hash for the content of any file (a "blob") and for the content of any "commit" object.

These hashes are used as filenames in the .minigit/objects directory.

This is Git's core idea: content-addressable storage. It ensures data integrity and de-duplicates files (if two files have the same content, they share the same hash and are only stored once).

Hash Maps (std::map) (for File Snapshots):

A commit needs to store a snapshot (a "tree") of all the files it tracks.

We use a std::map<string, string> as a hash map to store this snapshot, mapping filename (key) to its content_hash (value).

The staging area (.minigit/index) also uses a std::map to efficiently manage which files are staged for the next commit.

How It Works: The .minigit Directory

When you run minigit init, it creates this structure:

.minigit/: The main directory for all our data.

.minigit/objects/: This is our database. It stores two types of objects, both named by their hash:

"Blob" objects: The raw, compressed content of your files.

"Commit" objects: A text file containing the parent hash, the commit message, and the file snapshot (the hash map).

.minigit/HEAD: A simple file that stores only the hash of the most recent commit. This is the "head" pointer of our linked list.

.minigit/index: This is our "staging area." It's a simple text file that lists all the files staged for the next commit, along with their content hashes.

How to Build and Run

You will need a C++ compiler that supports C++17 (like g++ 8 or newer) and cmake.

1. Build the Project

# 1. Create a build directory (good practice)
mkdir build
cd build

# 2. Run CMake to configure the project
cmake ..

# 3. Run make (or your build tool) to compile
make


This will create an executable file named minigit inside the build directory.

2. Run the Commands

You must run the executable from the directory above build (your project's root) so it can create the .minigit folder in the correct place.

# Go back to the root directory
cd ..

# Make a test directory to work in
mkdir my_test_repo
cd my_test_repo

# --- Try the workflow ---

# 1. Initialize the repo
# (Use ../build/minigit to run the executable from the build folder)
../build/minigit init

# 2. Create a file
echo "Hello World" > file1.txt

# 3. Add the file
../build/minigit add file1.txt

# 4. Commit the file
../build/minigit commit -m "Initial commit"

# 5. Make another change
echo "This is a new line" >> file1.txt
echo "A new file" > file2.txt

# 6. Add both files (one is modified, one is new)
../build/minigit add file1.txt file2.txt

# 7. Commit again
../build/minigit commit -m "Add file2 and modify file1"

# 8. View the history!
../build/minigit log


Expected log Output:

commit a1b2c3d4... (some hash)
    Add file2 and modify file1

commit e5f6a7b8... (some other hash)
    Initial commit
