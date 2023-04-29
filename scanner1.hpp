#include <slang/ast/ASTVisitor.h>
#include <slang/ast/Expression.h>
#include <slang/ast/Symbol.h>
#include <algorithm>
#include <iostream>

using namespace slang::ast;
extern std::vector<std::string> secureAssets;
extern std::vector<std::string> insecureLocations;

class Scanner1 : public ASTVisitor<Scanner1, true, true> {
    bool containsInsecureLocation, containsSecureAsset;
    void handle(AssignmentExpression& assignExpr) {
        bool lhsContainsInsecureLoc, rhsContainsSecureAsset;

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
        }
    }

    void handle(NamedValueExpression& namedVal) {
        if(std::find(secureAssets.begin(), secureAssets.end(), namedVal.symbol.name) != secureAssets.end()) {
            containsSecureAsset = true;
        }
        if(std::find(insecureLocations.begin(), insecureLocations.end(), namedVal.symbol.name) != insecureLocations.end()) {
            containsInsecureLocation = true;
        }
    }
};
