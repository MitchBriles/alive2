#pragma once

// Copyright (c) 2018-present The Alive2 Authors.
// Distributed under the MIT license that can be found in the LICENSE file.

#include <compare>
#include <cstdint>
#include <ostream>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

typedef struct _Z3_context* Z3_context;
typedef struct _Z3_func_decl* Z3_decl;
typedef struct _Z3_app* Z3_app;
typedef struct _Z3_ast* Z3_ast;
typedef struct _Z3_sort* Z3_sort;

namespace smt {

class AndExpr;

class expr {
  uintptr_t ptr = 0;

  expr(Z3_ast ast) noexcept;
  bool isZ3Ast() const;
  Z3_ast ast() const;
  Z3_ast operator()() const { return ast(); }
  void incRef();
  void decRef();

  Z3_sort sort() const;
  Z3_decl decl() const;
  Z3_app isApp() const;
  Z3_app isAppOf(int app_type) const;

  expr binop_commutative(const expr &rhs,
                         Z3_ast(*op)(Z3_context, Z3_ast, Z3_ast),
                         expr (expr::*expr_op)(const expr &) const,
                         bool (expr::*identity)() const,
                         bool (expr::*absorvent)() const,
                         int z3_app = 0) const;
  expr binop_commutative(const expr &rhs,
                         Z3_ast(*op)(Z3_context, Z3_ast, Z3_ast)) const;

  expr unop_fold(Z3_ast(*op)(Z3_context, Z3_ast)) const;
  expr binop_fold(const expr &rhs,
                  Z3_ast(*op)(Z3_context, Z3_ast, Z3_ast)) const;

  bool alwaysFalse() const { return false; }

  static Z3_ast mkTrue();
  static Z3_ast mkFalse();
  static expr mkUInt(uint64_t n, Z3_sort sort);
  static expr mkInt(int64_t n, Z3_sort sort);
  static expr mkConst(Z3_decl decl);

  bool isUnOp(expr &a, int z3op) const;
  bool isBinOp(expr &a, expr &b, int z3op) const;
  bool isTernaryOp(expr &a, expr &b, expr &c, int z3op) const;

public:
  expr() = default;

  expr(expr &&other) noexcept {
    std::swap(ptr, other.ptr);
  }

  expr(const expr &other) noexcept;
  expr(bool val) noexcept : expr(val ? mkTrue() : mkFalse()) {}
  ~expr() noexcept;

  void operator=(expr &&other);
  void operator=(const expr &other);

  static expr mkUInt(uint64_t n, unsigned bits);
  static expr mkUInt(uint64_t n, const expr &type);
  static expr mkInt(int64_t n, unsigned bits);
  static expr mkInt(int64_t n, const expr &type);
  static expr mkInt(const char *n, unsigned bits);
  static expr mkFloat(double n, const expr &type);
  static expr mkHalf(float n);
  static expr mkBFloat(float n);
  static expr mkFloat(float n);
  static expr mkDouble(double n);
  static expr mkQuad(double n);
  static expr mkNaN(const expr &type);
  static expr mkNumber(const char *n, const expr &type);
  static expr mkQVar(unsigned n, const expr &type);
  static expr mkQVar(unsigned n, unsigned bits);
  static expr mkVar(const char *name, const expr &type);
  static expr mkVar(const char *name, unsigned bits, bool fresh = false);
  static expr mkBoolVar(const char *name);
  static expr mkHalfVar(const char *name);
  static expr mkBFloatVar(const char *name);
  static expr mkFloatVar(const char *name);
  static expr mkDoubleVar(const char *name);
  static expr mkQuadVar(const char *name);
  static expr mkFreshVar(const char *prefix, const expr &type);

  // return a constant value of the given type
  static expr some(const expr &type);

  static expr IntSMin(unsigned bits);
  static expr IntSMax(unsigned bits);
  static expr IntUMax(unsigned bits);

  // structural equivalence
  bool eq(const expr &rhs) const;

  bool isValid() const { return ptr != 0; }

  bool isConst() const;
  bool isVar() const;
  bool isQVar() const;
  bool isBV() const;
  bool isBool() const;
  bool isFloat() const;
  bool isTrue() const;
  bool isFalse() const;
  bool isZero() const;
  bool isOne() const;
  bool isAllOnes() const;
  bool isSMin() const;
  bool isSMax() const;
  expr isNegative() const;

  unsigned bits() const;
  bool isUInt(uint64_t &n) const;
  bool isInt(int64_t &n) const;
  bool isSameTypeOf(const expr &other) const;

  bool isEq(expr &lhs, expr &rhs) const;
  bool isSLE(expr &lhs, expr &rhs) const;
  bool isULE(expr &lhs, expr &rhs) const;
  bool isIf(expr &cond, expr &then, expr &els) const;
  bool isConcat(expr &a, expr &b) const;
  bool isExtract(expr &e, unsigned &high, unsigned &low) const;
  bool isSignExt(expr &val) const;
  bool isAShr(expr &a, expr &b) const;
  bool isAnd(expr &a, expr &b) const;
  bool isNot(expr &neg) const;
  bool isAdd(expr &a, expr &b) const;
  bool isURem(expr &a, expr &b) const;
  bool isBasePlusOffset(expr &base, uint64_t &offset) const;
  bool isConstArray(expr &val) const;
  bool isStore(expr &array, expr &idx, expr &val) const;
  bool isLoad(expr &array, expr &idx) const;
  bool isLambda(expr &body) const;
  expr lambdaIdxType() const;

  bool isFPAdd(expr &rounding, expr &lhs, expr &rhs) const;
  bool isFPSub(expr &rounding, expr &lhs, expr &rhs) const;
  bool isFPMul(expr &rounding, expr &lhs, expr &rhs) const;
  bool isFPDiv(expr &rounding, expr &lhs, expr &rhs) const;
  bool isFPNeg(expr &neg) const;
  bool isFAbs(expr &val) const;
  bool isIsFPZero() const;
  bool isNaNCheck(expr &fp) const;
  bool isfloat2BV(expr &fp) const;

  // best effort; returns number of statically known bits
  unsigned min_leading_zeros() const;
  unsigned min_trailing_ones() const;

  expr operator+(const expr &rhs) const;
  expr operator-(const expr &rhs) const;
  expr operator*(const expr &rhs) const;
  expr sdiv(const expr &rhs) const;
  expr udiv(const expr &rhs) const;
  expr srem(const expr &rhs) const;
  expr urem(const expr &rhs) const;

  // saturating arithmetic
  expr sadd_sat(const expr &rhs) const;
  expr uadd_sat(const expr &rhs) const;
  expr ssub_sat(const expr &rhs) const;
  expr usub_sat(const expr &rhs) const;
  expr sshl_sat(const expr &rhs) const;
  expr ushl_sat(const expr &rhs) const;

  expr add_no_soverflow(const expr &rhs) const;
  expr add_no_uoverflow(const expr &rhs) const;
  expr add_no_usoverflow(const expr &rhs) const;
  expr sub_no_soverflow(const expr &rhs) const;
  expr sub_no_uoverflow(const expr &rhs) const;
  expr mul_no_soverflow(const expr &rhs) const;
  expr mul_no_uoverflow(const expr &rhs) const;
  expr sdiv_exact(const expr &rhs) const;
  expr udiv_exact(const expr &rhs) const;

  expr operator<<(const expr &rhs) const;
  expr ashr(const expr &rhs) const;
  expr lshr(const expr &rhs) const;

  static expr fshl(const expr &a, const expr &b, const expr &c);
  static expr fshr(const expr &a, const expr &b, const expr &c);
  static expr smul_fix(const expr &a, const expr &b, const expr &c);
  static expr smul_fix_no_soverflow(const expr &a, const expr &b, const expr &c);
  static expr umul_fix(const expr &a, const expr &b, const expr &c);
  static expr umul_fix_no_uoverflow(const expr &a, const expr &b, const expr &c);
  static expr smul_fix_sat(const expr &a, const expr &b, const expr &c);
  static expr umul_fix_sat(const expr &a, const expr &b, const expr &c);

  expr shl_no_soverflow(const expr &rhs) const;
  expr shl_no_uoverflow(const expr &rhs) const;
  expr ashr_exact(const expr &rhs) const;
  expr lshr_exact(const expr &rhs) const;

  expr isPowerOf2() const;
  expr log2(unsigned bw_output) const;
  expr bswap() const;
  expr bitreverse() const;
  expr cttz(const expr &val_zero) const;
  expr ctlz() const;
  expr ctpop() const;

  expr umin(const expr &rhs) const;
  expr umax(const expr &rhs) const;
  expr smin(const expr &rhs) const;
  expr smax(const expr &rhs) const;

  expr abs() const;

  expr round_up(const expr &power_of_two) const;
  expr round_up_bits(const expr &nbits) const;
  expr round_up_bits_no_overflow(const expr &nbits) const;

  expr isNaN() const;
  expr isInf() const;
  expr isFPZero() const;
  expr isFPNegative() const;
  expr isFPNegZero() const;
  expr isFPNormal() const;
  expr isFPSubNormal() const;

  static expr rne();
  static expr rna();
  static expr rtp();
  static expr rtn();
  static expr rtz();

  expr fadd(const expr &rhs, const expr &rm) const;
  expr fsub(const expr &rhs, const expr &rm) const;
  expr fmul(const expr &rhs, const expr &rm) const;
  expr fdiv(const expr &rhs, const expr &rm) const;
  expr frem(const expr &rhs) const;
  expr fabs() const;
  expr fneg() const;
  expr copysign(const expr &sign) const;
  expr sqrt(const expr &rm) const;
  std::pair<expr, expr> frexp() const;

  static expr fma(const expr &a, const expr &b, const expr &c, const expr &rm);

  expr ceil() const;
  expr floor() const;
  expr round(const expr &rm) const;

  expr foeq(const expr &rhs) const;
  expr fogt(const expr &rhs) const;
  expr foge(const expr &rhs) const;
  expr folt(const expr &rhs) const;
  expr fole(const expr &rhs) const;
  expr fone(const expr &rhs) const;
  expr ford(const expr &rhs) const;
  expr fueq(const expr &rhs) const;
  expr fugt(const expr &rhs) const;
  expr fuge(const expr &rhs) const;
  expr fult(const expr &rhs) const;
  expr fule(const expr &rhs) const;
  expr fune(const expr &rhs) const;
  expr funo(const expr &rhs) const;

  expr operator&(const expr &rhs) const;
  expr operator|(const expr &rhs) const;
  expr operator^(const expr &rhs) const;

  expr operator!() const;
  expr operator~() const;

  expr cmp_eq(const expr &rhs, bool simplify) const; // same as operator==
  expr operator==(const expr &rhs) const;
  expr operator!=(const expr &rhs) const;

  expr operator&&(const expr &rhs) const;
  expr operator||(const expr &rhs) const;
  // the following are boolean only:
  void operator&=(const expr &rhs);
  void operator|=(const expr &rhs);

  static expr mk_and(const std::vector<expr> &vals);
  static expr mk_and(const std::set<expr> &vals);
  static expr mk_or(const std::set<expr> &vals);

  expr implies(const expr &rhs) const;
  expr notImplies(const expr &rhs) const;

  expr ule(const expr &rhs) const;
  expr ult(const expr &rhs) const;
  expr uge(const expr &rhs) const;
  expr ugt(const expr &rhs) const;
  expr sle(const expr &rhs) const;
  expr slt(const expr &rhs) const;
  expr sge(const expr &rhs) const;
  expr sgt(const expr &rhs) const;

  expr ule(uint64_t rhs) const;
  expr ule_extend(uint64_t rhs) const;
  expr ult(uint64_t rhs) const;
  expr uge(uint64_t rhs) const;
  expr ugt(uint64_t rhs) const;
  expr sle(int64_t rhs) const;
  expr sge(int64_t rhs) const;
  expr operator==(uint64_t rhs) const;
  expr operator!=(uint64_t rhs) const;

  expr sext(unsigned amount) const;
  expr zext(unsigned amount) const;
  expr trunc(unsigned tobw) const;
  expr sextOrTrunc(unsigned tobw) const;
  expr zextOrTrunc(unsigned tobw) const;

  expr concat(const expr &rhs) const;
  expr concat_zeros(unsigned bits) const;
  expr extract(unsigned high, unsigned low, unsigned depth = 3) const;
  expr sign() const;

  expr toBVBool() const;
  expr float2BV() const;
  expr float2Real() const;
  expr BV2float(const expr &type) const;
  expr float2Float(const expr &type, const expr &rm) const;

  expr fp2sint(unsigned bits, const expr &rm) const;
  expr fp2uint(unsigned bits, const expr &rm) const;
  expr sint2fp(const expr &type, const expr &rm) const;
  expr uint2fp(const expr &type, const expr &rm) const;

  // we don't expose SMT expr types, so range must be passed as a dummy value
  // of the desired type
  static expr mkUF(const char *name, const std::vector<expr> &args,
                   const expr &range);
  static expr mkUF(const std::string &name, const std::vector<expr> &args,
                   const expr &range) {
    return mkUF(name.data(), args, range);
  }

  static expr mkArray(const char *name, const expr &domain, const expr &range);
  static expr mkConstArray(const expr &domain, const expr &value);

  expr store(const expr &idx, const expr &val) const;
  expr load(const expr &idx, uint64_t max_idx = UINT64_MAX) const;

  static expr mkIf(const expr &cond, const expr &then, const expr &els);
  static expr mkForAll(const std::set<expr> &vars, expr &&val);
  static expr mkForAll(unsigned num_vars, const expr *vars, const char **names,
                       expr &&val);
  static expr mkLambda(const expr &var, const char *var_name, const expr &val);

  expr simplify() const;
  expr simplifyNoTimeout() const;

  expr foldTopLevel() const;

  // replace v1 -> v2
  expr subst(const std::vector<std::pair<expr, expr>> &repls) const;
  expr subst_simplify(const std::vector<std::pair<expr, expr>> &repls) const;
  expr subst(const expr &from, const expr &to) const;

  // replace the 1st quantified variable
  expr subst_var(const expr &repl) const;

  // turn all expressions in 'constraints' into true
  expr propagate(const AndExpr &constraints) const;

  std::set<expr> vars() const;
  static std::set<expr> vars(const std::vector<const expr*> &exprs);

  std::set<expr> leafs(unsigned max = 64) const;

  std::set<expr> get_apps_of(const char *fn_name, const char *prefix) const;

  void printUnsigned(std::ostream &os) const;
  void printSigned(std::ostream &os) const;
  void printHexadecimal(std::ostream &os) const;
  // WARNING: these are temporary strings; don't store them
  std::string_view numeral_string() const;
  std::string_view fn_name() const; // empty if not a function

  unsigned getFnNumArgs() const;
  expr getFnArg(unsigned i) const;
  friend std::ostream &operator<<(std::ostream &os, const expr &e);

  // for container use only
  std::strong_ordering operator<=>(const expr &rhs) const;
  unsigned id() const;
  unsigned hash() const;


  template <typename... Exprs>
  static bool allValid(const expr &e, Exprs&&... exprs) {
    return e.isValid() && allValid(exprs...);
  }
  static bool allValid(const expr &e) { return e.isValid(); }
  static bool allValid() { return true; }

  friend class Solver;
  friend class FnModel;
  friend class Model;
};


#define mkIf_fold(c, a, b) \
  mkIf_fold_fn<decltype(a)>(c, [&]() { return a; }, [&]() { return b; })

template <typename RetTy, typename T1, typename T2>
static RetTy mkIf_fold_fn(const expr &cond, T1 &&a, T2 &&b) {
  if (cond.isTrue())
    return a();
  if (cond.isFalse())
    return b();
  return RetTy::mkIf(cond, a(), b());
}

}
