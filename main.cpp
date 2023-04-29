#include "scanner1.hpp"
#include <slang/ast/Compilation.h>
#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxTree.h>
#include "slang/diagnostics/DiagnosticClient.h"
#include "slang/diagnostics/DiagnosticEngine.h"
#include "slang/diagnostics/TextDiagnosticClient.h"
#include "slang/text/SourceManager.h"

using namespace slang;
using namespace slang::ast;
using namespace slang::syntax;

int main(int argc, char *argv[]) {
    Compilation compilation;
    auto tree = SyntaxTree::fromFile(argv[1]);
    compilation.addSyntaxTree(tree);
    
    auto &diags = compilation.getAllDiagnostics();
    DiagnosticEngine diagEngine(*compilation.getSourceManager());
    auto client = std::make_shared<TextDiagnosticClient>();
    diagEngine.addClient(client);

    for (const Diagnostic &diag : diags) {
        diagEngine.issue(diag);
    }

    std::string report = client->getString();
    std::cerr << report << std::endl;
    Scanner1 test;
    test.visit(compilation.getRoot());
}
