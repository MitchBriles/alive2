#pragma once

#include <ostream>
#include <string>

#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"

namespace llvm {
class Constant;
class Function;
class VectorType;
} // namespace llvm

namespace lifter {

/*
 * add debug into to an IR file that will help the lifter figure out
 * which LLVM instruction each asm instruction came from
 */
void addDebugInfo(llvm::Function *srcFn,
                  std::unordered_map<unsigned, llvm::Instruction *> &lineMap);

/*
 * lower LLVM IR to textual assembly
 */
std::unique_ptr<llvm::MemoryBuffer>
generateAsm(llvm::Module &, const llvm::Target *Targ, llvm::Triple DefaultTT,
            const char *DefaultCPU, const char *DefaultFeatures);

/*
 * lift textual assembly to LLVM IR
 */
std::pair<llvm::Function *, llvm::Function *>
liftFunc(llvm::Function *, std::unique_ptr<llvm::MemoryBuffer>,
         std::unordered_map<unsigned, llvm::Instruction *> &lineMap,
         std::string optimize_tgt, std::ostream *out, const llvm::Target *Targ,
         llvm::Triple DefaultTT, const char *DefaultCPU,
         const char *DefaultFeatures);

/*
 * random utility function
 */
inline std::string moduleToString(llvm::Module *M) {
  std::string sss;
  llvm::raw_string_ostream ss(sss);
  M->print(ss, nullptr);
  return sss;
}

} // namespace lifter
