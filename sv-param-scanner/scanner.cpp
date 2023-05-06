#include "scanner.h"

int main(int argc, char const *argv[]) {

    std::string dir = argv[1];
    std::vector<std::string> files;
    auto client = std::make_shared<TextDiagnosticClient>();
    int fileCount = 0;
    int errCount = 0;
    int errFileCount = 0;

    getFiles(dir, files);
    printf("Found %lu .v/.sv files to scan...\n", files.size());
    for (auto file : files) {
        if(endsWith(file, ".sv") || endsWith(file, ".v")) {
            std::cout << "scanning file " << file << std::endl;
            auto tree = SyntaxTree::fromFile(file);

            ParameterScanner scanner(tree);
            scanner.scan();

            DiagnosticEngine diagEngine(tree->sourceManager());
            diagEngine.addClient(client);

            for (Diagnostic& diag : tree->diagnostics()) {
                diagEngine.issue(diag);
            }
            // std::cout << client->getString() << std::endl;
            errCount += tree->diagnostics().size();
            if(tree->diagnostics().size() > 0) errFileCount++;
            fileCount++;
        }
    }
    std::cout << "Found " << fileCount << ".v/.sv files...\n";
    std::cout << "Encountered " << errCount << " errors in " << errFileCount << " files...\n";
    return 0;
}

// check if fullString ends with ending
bool endsWith(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    return false;
}

// get files in dir recursively
void getFiles(std::string dir, std::vector<std::string> &files) {
    for(const auto &entry : std::filesystem::directory_iterator(dir)) {
        if(entry.is_directory()) {
            getFiles(entry.path().string(), files);
        }
        else {
            files.push_back(entry.path().string());
        }
    }
}

// helper function to print kind of node; useful when making visit functions
void printKind(const SyntaxNode *node) {
    std::cout << "Type: " << node->kind << " Value: " << node->toString() << std::endl;
    std::cout << "**************" << std::endl;

    for (int i = 0; i < node->getChildCount(); i++) {
        auto child = node->childNode(i);
        if(child == NULL) {
            printKind(node->childToken(i));
        }
        else {
            printKind(child);
        }
    }
}

// helper function to print kind of token; useful when making visit functions
void printKind(Token token) {
    std::cout << "Type: " << token.kind << " Value: " << token.valueText() << std::endl;
}
