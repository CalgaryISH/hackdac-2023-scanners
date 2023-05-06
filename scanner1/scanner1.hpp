#include <slang/ast/ASTVisitor.h>
#include <slang/ast/Expression.h>
#include <slang/ast/Symbol.h>
#include <slang/syntax/AllSyntax.h>

#include <algorithm>
#include <iostream>

using namespace slang::ast;
extern std::vector<std::string> secureAssets;
extern std::vector<std::string> insecureLocations;

struct Scanner1 : public ASTVisitor<Scanner1, true, true> {
    bool containsInsecureLocation, containsSecureAsset;
    // void handle(const InstanceSymbol& inst) {
    //     std::cout << "here\n";
    // }

    void handle(const AssignmentExpression& assignExpr) {
        bool lhsContainsInsecureLoc = false, rhsContainsSecureAsset = false;

        containsInsecureLocation = false;
        containsSecureAsset = false;
        visitDefault(assignExpr.left());
        if(containsInsecureLocation) {
            lhsContainsInsecureLoc = true;
        }

        containsInsecureLocation = false;
        containsSecureAsset = false;
        visitDefault(assignExpr.right());
        if(containsSecureAsset) {
            rhsContainsSecureAsset = true;
        }

        if(lhsContainsInsecureLoc && rhsContainsSecureAsset) {
            std::cout << "potential insecure assignment\n";
            if(assignExpr.syntax != nullptr) {
                std::cout << assignExpr.syntax->toString() << "\n";
            }
        }
    }

    void handle(const MemberAccessExpression& memberAccess) {
        if(std::find(secureAssets.begin(), secureAssets.end(), memberAccess.member.name) != secureAssets.end()) {
            containsSecureAsset = true;
        }
        if(std::find(insecureLocations.begin(), insecureLocations.end(), memberAccess.member.name) != insecureLocations.end()) {
            containsInsecureLocation = true;
        }
        // visitDefault(namedVal);
    }

    void handle(const NamedValueExpression& namedVal) {
        if(std::find(secureAssets.begin(), secureAssets.end(), namedVal.symbol.name) != secureAssets.end()) {
            containsSecureAsset = true;
        }
        if(std::find(insecureLocations.begin(), insecureLocations.end(), namedVal.symbol.name) != insecureLocations.end()) {
            containsInsecureLocation = true;
        }
        // visitDefault(namedVal);
    }
};
