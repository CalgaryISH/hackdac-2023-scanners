#include "scanner1.hpp"
#include <slang/ast/Compilation.h>
#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxTree.h>
#include "slang/diagnostics/DiagnosticClient.h"
#include "slang/diagnostics/DiagnosticEngine.h"
#include "slang/diagnostics/TextDiagnosticClient.h"
#include "slang/text/SourceManager.h"
#include <vector>
#include <string>
#include <memory>

using namespace slang;
using namespace slang::ast;
using namespace slang::syntax;

std::vector<std::string> secureAssets {"key_share0", "key_share1"};
std::vector<std::string> insecureLocations {"reg_rdata_next"};

int main(int argc, char *argv[]) {
    SourceManager sm;
    DiagnosticEngine diagEngine(sm);
    std::shared_ptr<TextDiagnosticClient> client = std::make_shared<TextDiagnosticClient>();
    std::vector<SourceBuffer> buffers;
    Compilation compilation;

    for(int i = 1; i < argc; i++) {
        SourceBuffer buff = sm.readSource(argv[i]);
        buffers.push_back(buff);
    }

    for (const SourceBuffer& buffer : buffers) {
        auto tree = SyntaxTree::fromBuffer(buffer, sm);
        compilation.addSyntaxTree(tree);
    }
    
    auto &diags = compilation.getAllDiagnostics();
    diagEngine.addClient(client);

    for (const Diagnostic &diag : diags) {
        diagEngine.issue(diag);
    }

    std::string report = client->getString();
    std::cerr << report << std::endl;
    Scanner1 test;
    std::cout << compilation.getRoot().name << std::endl;
    test.visit(compilation.getRoot());
}
