//===- wpa.cpp -- Whole program analysis -------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-----------------------------------------------------------------------===//

/*
 // Whole Program Pointer Analysis
 //
 // Author: Yulei Sui,
 */

#include "WPA/WPAPass.h"

#include <llvm/Support/CommandLine.h>	// for cl
#include <llvm/Support/FileSystem.h>	// for sys::fs::F_None
#include <llvm/Bitcode/BitcodeWriterPass.h>  // for bitcode write
#include <llvm/PassManager.h>		// pass manager
#include <llvm/Support/Signals.h>	// singal for command line
#include <llvm/IRReader/IRReader.h>	// IR reader for bit file
#include <llvm/Support/ToolOutputFile.h> // for tool output file
#include <llvm/Support/PrettyStackTrace.h> // for pass list
#include <llvm/IR/LLVMContext.h>		// for llvm LLVMContext
#include <llvm/Support/SourceMgr.h> // for SMDiagnostic
#include <llvm/Bitcode/ReaderWriter.h>		// for createBitcodeWriterPass


using namespace llvm;

static cl::opt<std::string> InputFilename(cl::Positional,
		cl::desc("<input bitcode>"), cl::init("-"));


int main(int argc, char ** argv) {

	sys::PrintStackTraceOnErrorSignal();
	llvm::PrettyStackTraceProgram X(argc, argv);

	LLVMContext &Context = getGlobalContext();

	std::string OutputFilename;

	cl::ParseCommandLineOptions(argc, argv, "Whole Program Points-to Analysis\n");
	sys::PrintStackTraceOnErrorSignal();

	PassRegistry &Registry = *PassRegistry::getPassRegistry();

	initializeCore(Registry);
	initializeScalarOpts(Registry);
	initializeIPO(Registry);
	initializeAnalysis(Registry);
	initializeIPA(Registry);
	initializeTransformUtils(Registry);
	initializeInstCombine(Registry);
	initializeInstrumentation(Registry);
	initializeTarget(Registry);

	PassManager Passes;

	SMDiagnostic Err;
	std::auto_ptr<Module> M1;

	M1.reset(ParseIRFile(InputFilename, Err, Context));
	if (M1.get() == 0) {
		Err.print(argv[0], errs());
		return 1;
	}

	std::unique_ptr<tool_output_file> Out;
	std::string ErrorInfo;

	StringRef str(InputFilename);
	InputFilename = str.rsplit('.').first;
	OutputFilename = InputFilename + ".wpa";

	Out.reset(
			new tool_output_file(OutputFilename.c_str(), ErrorInfo,
					sys::fs::F_None));

	if (!ErrorInfo.empty()) {
		errs() << ErrorInfo << '\n';
		return 1;
	}

	Passes.add(new WPAPass());

	Passes.add(createBitcodeWriterPass(Out->os()));

	Passes.run(*M1.get());
	Out->keep();

	return 0;

}

