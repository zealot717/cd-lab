#include <string>
#include <iostream>
#include <set>
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

    void run(const MatchFinder::MatchResult &Result) override {
        const FunctionDecl *Func = Result.Nodes.getNodeAs<FunctionDecl>("funcDecl");
        if (!Func || !Func->hasBody() || Func->isMain()) return;

        const Stmt *Body = Func->getBody();
        SourceManager &SM = *Result.SourceManager;

        std::string FuncName = Func->getNameInfo().getName().getAsString();
        std::string entryCall = "runtime_function_entry(\"" + FuncName + "\");\n";
        std::string exitCall = "runtime_function_exit(\"" + FuncName + "\");\n";

        // Insert entry at beginning of body
        SourceLocation StartLoc = Body->getBeginLoc().getLocWithOffset(1);
        TheRewriter.InsertText(StartLoc, entryCall, true, true);

        // Visit returns and insert exit before each
        class ReturnVisitor : public RecursiveASTVisitor<ReturnVisitor> {
        public:
            ReturnVisitor(Rewriter &R, const std::string &exitCall, SourceManager &SM)
                : TheRewriter(R), ExitCall(exitCall), SM(SM) {}

            bool VisitReturnStmt(ReturnStmt *Ret) {
                SourceLocation RetLoc = Ret->getBeginLoc();
                if (RetLoc.isValid() && !SM.isInSystemHeader(RetLoc)) {
                    TheRewriter.InsertText(RetLoc, ExitCall, true, true);
                    FoundReturn = true;
                }
                return true;
            }

            bool foundReturn() const { return FoundReturn; }

        private:
            Rewriter &TheRewriter;
            const std::string &ExitCall;
            SourceManager &SM;
            bool FoundReturn = false;
        };

        ReturnVisitor visitor(TheRewriter, exitCall, SM);
        visitor.TraverseStmt(const_cast<Stmt*>(Body));

        // If no return was found, insert exit before final closing brace
        if (!visitor.foundReturn()) {
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
        Matcher.addMatcher(
            functionDecl(isDefinition(), unless(isExpansionInSystemHeader())).bind("funcDecl"),
            &Handler
        );
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
