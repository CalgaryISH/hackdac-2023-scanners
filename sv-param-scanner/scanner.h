#include <slang/syntax/SyntaxTree.h>
#include <slang/syntax/SyntaxNode.h>
#include <slang/syntax/SyntaxVisitor.h>
#include <slang/text/SourceManager.h>
#include <slang/text/SourceLocation.h>
#include <slang/diagnostics/Diagnostics.h>
#include <slang/diagnostics/DiagnosticEngine.h>
#include <slang/diagnostics/DiagnosticClient.h>
#include <slang/diagnostics/TextDiagnosticClient.h>
#include <slang/numeric/SVInt.h>

#include <boost/algorithm/string.hpp>

#include <string>
#include <string_view>
#include <iostream>
#include <vector>
#include <filesystem>

using namespace slang;
using namespace parsing;
using namespace syntax;

void printKind(const SyntaxNode *node);
void printKind(Token token);
bool endsWith(std::string const &fullString, std::string const &ending);
void getFiles(std::string dir, std::vector<std::string> &files);

class ParameterVisitor : public SyntaxVisitor<ParameterVisitor> {
private:
    //list to store parameter identifiers of module
    std::vector<std::string_view> params;

    SourceManager *treeSourceManager;

    // adds the parameter identifier to params vector
    void getParameterIdentifier(const SyntaxNode& parent) {   
        for (int i = 0; i < parent.getChildCount(); i++) {

            auto childNode = parent.childNode(i);
    
            //child node is a token
            if(childNode == NULL) {
                auto childToken = parent.childToken(i);

                //add identifier to vector
                if(childToken.kind == TokenKind::Identifier) {
                    params.push_back(childToken.valueText());
                }
            }
            //recursively travel down tree
            else {
                getParameterIdentifier(*childNode);
            }
        }
    }

public:
    ParameterVisitor(SourceManager *sm) {
        this->treeSourceManager = sm;
    }

    //get module parameters
    void handle(const ParameterDeclarationSyntax& t) {
        getParameterIdentifier(t);
    }

    void handle(const NamedParamAssignmentSyntax &t) {        
        std::string expr;
        std::string_view identifier;

        // for (int i = 0; i < t.getChildCount(); i++) {

        //     auto childNode = t.childNode(i);
    
        //     //child node is a token
        //     if(childNode == NULL) {
        //         auto childToken = t.childToken(i);
        //         if(childToken.kind == TokenKind::Identifier) {
        //             identifier = childToken.valueText();
        //         }
        //     }
        //     else {
        //         val = childNode;
        //     }
        // }
        identifier = t.name.valueText();
        if(t.expr != nullptr) {
            expr = boost::trim_copy(t.expr->toString());
        }
        else {
            std::string_view filename = treeSourceManager->getFileName(t.sourceRange().start());
            std::size_t lineNum = treeSourceManager->getLineNumber(t.sourceRange().start());
            std::cerr << "Warning - In file " << filename << ", line " << lineNum << ": " << "Parameter " 
            << identifier << " declared with no expressions!\n";
        }

        // heuristic: ignore for parameter DW
        if(identifier == "DW") return;

        //parent and child have matching param identifiers; child doesn't assign parameter to parent identifier
        if(std::find(params.begin(), params.end(), identifier) != params.end()
           && expr != identifier) {

            std::string_view filename = treeSourceManager->getFileName(t.sourceRange().start());
            std::size_t lineNum = treeSourceManager->getLineNumber(t.sourceRange().start());
            std::cerr << "Warning - In file " << filename << ", line " << lineNum << ": " << "Parameter " 
            << identifier << " declared to " << expr << "!\n";
        }
    }
};


struct CaseVisitor : public SyntaxVisitor<CaseVisitor> {
    SourceManager *treeSourceManager;
    CaseVisitor(SourceManager *sm) {
        this->treeSourceManager = sm;
    }

    // void handle(const CaseStatementSyntax &t) {
    //     bool hasDefaultStmt = false;
    //     for(auto & caseItem : t.items) {
    //         if(caseItem->kind == syntax::SyntaxKind::DefaultCaseItem) {
    //             hasDefaultStmt = true;
    //         }
    //         // std::cout << caseItem->kind << "\n";

    //     }

    //     // //parent and child have matching param identifiers; child doesn't assign parameter to parent identifier
    //     if(!hasDefaultStmt) {

    //         std::string_view filename = treeSourceManager->getFileName(t.sourceRange().start());
    //         std::size_t lineNum = treeSourceManager->getLineNumber(t.sourceRange().start());
    //         std::cerr << "Warning - In file " << filename << ", line " << lineNum << ": " << "No default case\n";
    //     }
    // }
    bool isZero;
    void handle(const BinaryExpressionSyntax &t) {
        if(t.kind == syntax::SyntaxKind::BinaryAndExpression) {
            auto lhs = t.left;
            auto rhs = t.right;
            if(lhs && rhs) {
                if(lhs->kind == syntax::SyntaxKind::IntegerLiteralExpression) {
                    isZero = false;
                    lhs->visit(*this);

                    if(isZero) {
                        std::string_view filename = treeSourceManager->getFileName(t.sourceRange().start());
                        std::size_t lineNum = treeSourceManager->getLineNumber(t.sourceRange().start());
                        std::cerr << "Warning - In file " << filename << ", line " << lineNum << ": " << "LHS of AND is zero\n";
                        return;
                    }
                }
                if(rhs->kind == syntax::SyntaxKind::IntegerLiteralExpression) {
                    isZero = false;
                    rhs->visit(*this);
                    if(isZero) {
                        std::string_view filename = treeSourceManager->getFileName(t.sourceRange().start());
                        std::size_t lineNum = treeSourceManager->getLineNumber(t.sourceRange().start());
                        std::cerr << "Warning - In file " << filename << ", line " << lineNum << ": " << "RHS of AND is zero\n";
                        return;
                    }
                }
                
            }
        }
    }

    void handle (const LiteralExpressionSyntax &t) {
        if(t.kind == syntax::SyntaxKind::IntegerLiteralExpression) {
            isZero = exactlyEqual(t.literal.intValue(), SVInt(0));
        }
    }
    
};


struct ANDVisitor : public SyntaxVisitor<ANDVisitor> {
    SourceManager *treeSourceManager;
    ANDVisitor(SourceManager *sm) {
        this->treeSourceManager = sm;
    }

    bool isZero;
    void handle(const BinaryExpressionSyntax &t) {
        if(t.kind == syntax::SyntaxKind::BinaryAndExpression) {
            auto lhs = t.left;
            auto rhs = t.right;
            if(lhs && rhs) {
                if(lhs->kind == syntax::SyntaxKind::IntegerLiteralExpression) {
                    isZero = false;
                    lhs->visit(*this);

                    if(isZero) {
                        std::string_view filename = treeSourceManager->getFileName(t.sourceRange().start());
                        std::size_t lineNum = treeSourceManager->getLineNumber(t.sourceRange().start());
                        std::cerr << "Warning - In file " << filename << ", line " << lineNum << ": " << "LHS of AND is zero\n";
                        return;
                    }
                }
                if(rhs->kind == syntax::SyntaxKind::IntegerLiteralExpression) {
                    isZero = false;
                    rhs->visit(*this);
                    if(isZero) {
                        std::string_view filename = treeSourceManager->getFileName(t.sourceRange().start());
                        std::size_t lineNum = treeSourceManager->getLineNumber(t.sourceRange().start());
                        std::cerr << "Warning - In file " << filename << ", line " << lineNum << ": " << "RHS of AND is zero\n";
                        return;
                    }
                }
                
            }
        }
    }

    void handle (const LiteralExpressionSyntax &t) {
        if(t.kind == syntax::SyntaxKind::IntegerLiteralExpression) {
            isZero = exactlyEqual(t.literal.intValue(), SVInt(0));
        }
    }
};

class ParameterScanner {
private:
    //root node of AST
    SyntaxNode *root;

    //Parameter visitor
    ParameterVisitor *paramVisitor;
    CaseVisitor *caseVisitor;
    ANDVisitor * andVisitor;

public:
    ParameterScanner(std::shared_ptr<SyntaxTree> tree) {
        this->root = &(tree->root());
        this->paramVisitor = new ParameterVisitor(&(tree->sourceManager()));
        this->caseVisitor = new CaseVisitor(&(tree->sourceManager()));
        this->andVisitor = new ANDVisitor(&(tree->sourceManager()));

    }
    
    ~ParameterScanner() {
        if(this->paramVisitor != nullptr) {
            delete this->paramVisitor;
        }
        if(this->caseVisitor != nullptr) {
            delete this->caseVisitor;
        }
    }

    void scan() {
        // root->visit(*paramVisitor);
        // root->visit(*caseVisitor);
        root->visit(*andVisitor);
    }
};
