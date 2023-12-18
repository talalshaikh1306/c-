#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <algorithm>

using namespace std;

// Structure to represent a revision in the version control system
struct Revision {
    string identifier;
    string timestamp;
    string message;
    string filename;
    string content;
};

// Class representing a simple version control system
class VersionControlSystem {
private:
    // History of revisions in the version control system
    vector<Revision> revisionHistory;

public:
    // Function to initialize a new repository
    void setupRepository() {
        cout << "Repository initialized." << endl;
    }

    // Function to commit changes to a file
    void makeCommit(const string& filename, const string& message) {
        // Read the content of the file
        string fileContent = readFileContent(filename);

        // Create a new revision
        Revision revision;
        revision.timestamp = getCurrentTimestamp();
        revision.message = message;
        revision.filename = filename;
        revision.content = fileContent;
        revision.identifier = computeIdentifier(revision);

        // Add revision to the history
        revisionHistory.push_back(revision);

        cout << "Changes committed to '" << filename << "'." << endl;
    }

    // Function to view revision log
    vector<Revision> displayRevisionLog() {
        return revisionHistory;
    }

    // Function to revert to a previous version of a file
    void revertToFile(const string& filename, const string& revisionIdentifier) {
        // Find the revision with the specified identifier
        auto revisionIt = find_if(revisionHistory.rbegin(), revisionHistory.rend(),
                                  [filename, revisionIdentifier](const Revision& revision) {
                                      return revision.filename == filename && revision.identifier == revisionIdentifier;
                                  });

        if (revisionIt != revisionHistory.rend()) {
            // Revert the file to the state at the specified revision
            writeToFile(filename, revisionIt->content);
            cout << "File '" << filename << "' reverted to the state at revision " << revisionIdentifier << "." << endl;
        } else {
            cout << "Error: Revision with identifier '" << revisionIdentifier << "' for file '" << filename << "' not found." << endl;
        }
    }

private:
    // Function to calculate SHA-256 hash of a revision
    string computeIdentifier(const Revision& revision) {
        string dataToHash = revision.filename + revision.content + revision.timestamp + revision.message;
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, dataToHash.c_str(), dataToHash.length());
        SHA256_Final(hash, &sha256);
        stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
        }
        return ss.str();
    }

    // Function to get current timestamp
    string getCurrentTimestamp() {
        time_t now = time(0);
        tm* localTime = localtime(&now);
        stringstream ss;
        ss << put_time(localTime, "%Y-%m-%d %X");
        return ss.str();
    }

    // Function to read file content
    string readFileContent(const string& filename) {
        ifstream fileStream(filename);
        stringstream buffer;
        buffer << fileStream.rdbuf();
        return buffer.str();
    }

    // Function to write content to a file
    void writeToFile(const string& filename, const string& content) {
        ofstream fileStream(filename);
        fileStream << content;
    }
};

int main() {
    VersionControlSystem vcs;

    // Example usage
    vcs.setupRepository();

    // Make changes to the file
    ofstream fileStream("example.txt");
    fileStream << "This is the initial content.";

    // Commit changes
    vcs.makeCommit("example.txt", "Initial commit");

    // Make more changes to the file
    fileStream << " Additional content.";

    // Commit changes
    vcs.makeCommit("example.txt", "Second commit");

    // View revision log
    vector<Revision> revisionLog = vcs.displayRevisionLog();
    for (const auto& revision : revisionLog) {
        cout << "Identifier: " << revision.identifier << " | Timestamp: " << revision.timestamp
             << " | Message: " << revision.message << " | File: " << revision.filename << endl;
    }

    // Revert to a previous version
    if (revisionLog.size() >= 2) {
        vcs.revertToFile("example.txt", revisionLog[1].identifier);
    }

    return 0;
}
