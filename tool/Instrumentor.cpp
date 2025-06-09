#include <string>
#include <iostream>
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

static llvm::cl::OptionCategory ToolCategory("cd-lab instrumentation options");

class InstrumentorCallback : public MatchFinder::MatchCallback {
public:
    InstrumentorCallback(Rewriter &R) : TheRewriter(R) {}

    virtual void run(const MatchFinder::MatchResult &Result) {
        if (const FunctionDecl *Func = Result.Nodes.getNodeAs<FunctionDecl>("funcDecl")) {
            if (!Func->hasBody() || Func->isMain()) return;

            const Stmt *Body = Func->getBody();
            SourceManager &SM = *Result.SourceManager;

            std::string FuncName = Func->getNameInfo().getName().getAsString();
            std::string entryCall = "runtime_function_entry(\"" + FuncName + "\");\n";
            std::string exitCall = "runtime_function_exit(\"" + FuncName + "\");\n";

            SourceLocation StartLoc = Body->getBeginLoc().getLocWithOffset(1);
            TheRewriter.InsertText(StartLoc, entryCall, true, true);

            // Insert exit before the final closing brace
            if (const CompoundStmt *Compound = dyn_cast<CompoundStmt>(Body)) {
                SourceLocation EndLoc = Compound->getRBracLoc();
                TheRewriter.InsertTextBefore(EndLoc, exitCall);
            }
        }
    }

private:
    Rewriter &TheRewriter;
};

class InstrumentorASTConsumer : public ASTConsumer {
public:
    InstrumentorASTConsumer(Rewriter &R) : Handler(R) {
        Matcher.addMatcher(functionDecl(isDefinition(), unless(isExpansionInSystemHeader())).bind("funcDecl"), &Handler);
    }

    void HandleTranslationUnit(ASTContext &Context) override {
        Matcher.matchAST(Context);
    }

private:
    InstrumentorCallback Handler;
    MatchFinder Matcher;
};

class InstrumentorFrontendAction : public ASTFrontendAction {
public:
    void EndSourceFileAction() override {
        SourceManager &SM = TheRewriter.getSourceMgr();
        llvm::errs() << "** Finished instrumenting: "
                     << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";
        TheRewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
    }

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override {
        TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
        return std::make_unique<InstrumentorASTConsumer>(TheRewriter);
    }

private:
    Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    ClangTool Tool(ExpectedParser->getCompilations(), ExpectedParser->getSourcePathList());
    return Tool.run(newFrontendActionFactory<InstrumentorFrontendAction>().get());
}
