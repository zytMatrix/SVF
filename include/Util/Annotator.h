/*
 * Annotator.h
 *
 *  Created on: May 4, 2014
 *      Author: Yulei Sui
 */

#ifndef ANNOTATOR_H_
#define ANNOTATOR_H_

#include <llvm/IR/Instruction.h>
#include <llvm/IR/Metadata.h>	// for llvm MDNode
#include <llvm/IR/Type.h>
#include <vector>

/*!
 * Program annotator to write meta data information on LLVM IR
 */
class Annotator {

public:
	/// Constructor
	Annotator() {
		SB_SLICESOURCE = "SOURCE_";
		SB_SLICESINK = "SINK_";
		SB_FESIBLE = "FESIBLE_";
		SB_INFESIBLE = "INFESIBLE_";

		MTA_DR_NOT_CHECK = "DRNOTCHECK_";
		MTA_DR_CHECK = "DRCHECK_";
	}

	/// Destructor
	virtual ~Annotator() {

	}

	/// SB Has flag methods
	//@{
	inline bool hasSBSourceFlag(llvm::Instruction *inst) const {
		std::vector<llvm::Value *> values;
		return evalMDTag(inst, inst, SB_SLICESOURCE, values);
	}
	inline bool hasSBSinkFlag(llvm::Instruction *inst) const {
		std::vector<llvm::Value *> values;
		return evalMDTag(inst, inst, SB_SLICESINK, values);
	}
	//@}

	/// MTA Has flag methods
	//@{
	inline bool hasMTADRNotCheckFlag(llvm::Instruction *inst) const {
		//std::vector<llvm::Value *> values;
		//return evalMDTag(inst, inst, MTA_DR_NOT_CHECK, values);
		if (inst->getMetadata(MTA_DR_NOT_CHECK))
			return true;
		else
			return false;
	}
	inline bool hasMTADRNotCheckFlag(const llvm::Instruction *inst) const {
		//std::vector<llvm::Value *> values;
		//return evalMDTag(inst, inst, MTA_DR_NOT_CHECK, values);
		if (inst->getMetadata(MTA_DR_NOT_CHECK))
			return true;
		else
			return false;
	}

	inline bool hasMTADRCheckFlag(llvm::Instruction *inst) const {
		//std::vector<llvm::Value *> values;
		//return evalMDTag(inst, inst, MTA_DR_CHECK, values);
		if (inst->getMetadata(MTA_DR_CHECK))
			return true;
		else
			return false;
	}
	inline bool hasMTADRCheckFlag(const llvm::Instruction *inst) const {
		//std::vector<llvm::Value *> values;
		//return evalMDTag(inst, inst, MTA_DR_CHECK, values);
		if (inst->getMetadata(MTA_DR_CHECK))
			return true;
		else
			return false;
	}
	//@}

	/// Simple add/remove meta data information
	//@{
	inline void addMDTag(llvm::Instruction *inst, std::string str) {
		addMDTag(inst, inst, str);
	}
	inline void removeMDTag(llvm::Instruction *inst, std::string str) {
		removeMDTag(inst, inst, str);
	}
	//@}

	/// manipulate llvm meta data on instructions for a specific value
	//@{
	/// add flag to llvm metadata
	inline void addMDTag(llvm::Instruction *inst, llvm::Value *val, std::string str) {
		assert(!val->getType()->isVoidTy() && "expecting non-void value for MD!");
		std::vector<llvm::Value *> values;
		// add the flag if we did not see it before
		if (evalMDTag(inst, val, str, values) == false) {
			values.push_back(val);
			// FIXME: delete the old MDNode
			inst->setMetadata(str, llvm::MDNode::get(inst->getContext(), values));
		}
	}

	/// remove flag from llvm metadata
	inline void removeMDTag(llvm::Instruction *inst, llvm::Value *val, std::string str) {
		assert(!val->getType()->isVoidTy() && "expecting non-void value for MD!");
		std::vector<llvm::Value *> values;
		// remove the flag if it is there
		if (evalMDTag(inst, val, str, values) == true) {
			// FIXME: delete the old MDNode
			inst->setMetadata(str, llvm::MDNode::get(inst->getContext(), values));
		}
	}
	//@}

private:

	/// evaluate llvm metadata
	inline bool evalMDTag(const llvm::Instruction *inst, const llvm::Value *val, std::string str,
			std::vector<llvm::Value *> &values) const {

		assert(val && "value should not be null");

		bool hasFlag = false;
		if (llvm::MDNode *mdNode = inst->getMetadata(str)) {
			/// When mdNode has operands and value is not null
			for (unsigned k = 0; k < mdNode->getNumOperands(); ++k) {
				llvm::Value *v = mdNode->getOperand(k);
				if (v == val)
					hasFlag = true;
				else
					values.push_back(v);
			}
		}
		return hasFlag;
	}

protected:

	/// Saber annotations
	//@{
	const char* SB_SLICESOURCE;
	const char* SB_SLICESINK;
	const char* SB_FESIBLE;
	const char* SB_INFESIBLE;
	//@}

	/// MTA annotations
	//@{
	const char* MTA_DR_NOT_CHECK;
	const char* MTA_DR_CHECK;
	//@}
};

#endif /* ANNOTATOR_H_ */
