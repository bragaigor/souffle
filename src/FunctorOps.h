/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2019, The Souffle Developers. All rights reserved.
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file FunctorOps.h
 *
 * Defines intrinsic functor operators for AST and RAM
 *
 ***********************************************************************/

#pragma once

#include "RamTypes.h"
#include "utility/MiscUtil.h"
#include <cassert>
#include <cstdlib>
#include <ostream>

namespace souffle {

enum class FunctorOp {
    /** Unary Functor Operators */
    ORD,       // ordinal number of a string
    STRLEN,    // length of a string
    NEG,       // Signed numeric negation
    FNEG,      // Float numeric negation
    BNOT,      // Signed bitwise negation
    UBNOT,     // Unsigned bitwise negation
    LNOT,      // Signed logical negation
    ULNOT,     // Unsigned logical negation
    TONUMBER,  // convert string to number
    TOSTRING,  // convert number to string
    ITOU,      // convert signed number to unsigned
    UTOI,      // convert unsigned number to signed
    ITOF,      // convert signed number to float
    FTOI,      // convert float number to signed number.
    UTOF,      // convert unsigned number to a float.
    FTOU,      // convert float to unsigned number.

    /** Binary Functor Operators */
    ADD,                 // addition
    SUB,                 // subtraction
    MUL,                 // multiplication
    DIV,                 // division
    EXP,                 // exponent
    MAX,                 // max of two numbers
    MIN,                 // min of two numbers
    MOD,                 // modulus
    BAND,                // bitwise and
    BOR,                 // bitwise or
    BXOR,                // bitwise exclusive or
    BSHIFT_L,            // bitwise shift left
    BSHIFT_R,            // bitwise shift right
    BSHIFT_R_UNSIGNED,   // bitwise shift right (unsigned)
    LAND,                // logical and
    LOR,                 // logical or
    UADD,                // addition
    USUB,                // subtraction
    UMUL,                // multiplication
    UDIV,                // division
    UEXP,                // exponent
    UMAX,                // max of two numbers
    UMIN,                // min of two numbers
    UMOD,                // modulus
    UBAND,               // bitwise and
    UBOR,                // bitwise or
    UBXOR,               // bitwise exclusive or
    UBSHIFT_L,           // bitwise shift right
    UBSHIFT_R,           // bitwise shift right
    UBSHIFT_R_UNSIGNED,  // bitwise shift right (unsigned)
    ULAND,               // logical and
    ULOR,                // logical or
    FADD,                // addition
    FSUB,                // subtraction
    FMUL,                // multiplication
    FDIV,                // division
    FEXP,                // exponent
    FMAX,                // max of two floats
    FMIN,                // min of two floats
    SMAX,                // max of two symbols
    SMIN,                // min of two symbols

    // Produces values within a numeric range. Format is `range(bgn, endExcl, step = 1)`.
    // e.g. `range(0, 5)` produces the sequence `0, 1, 2, 3, 4`.
    //      `range(5, 3.75, -0.5)` produces the sequence `5, 4.5, 4`.
    //      `range(5, x, 0)` produces the sequence `5` iff `x` != 5.
    RANGE,
    URANGE,
    FRANGE,

    CAT,  // string concatenation

    /** Ternary Functor Operators */
    SUBSTR,  // substring
};

// does this functor produce multiple results?
inline bool isFunctorMultiResult(FunctorOp op) {
    switch (op) {
        case FunctorOp::RANGE:
        case FunctorOp::URANGE:
        case FunctorOp::FRANGE: return true;

        default: return false;
    }
}

/**
 * Checks whether a functor operation can have a given argument count.
 */
inline bool isValidFunctorOpArity(const FunctorOp op, const size_t arity) {
    switch (op) {
        /** Unary Functor Operators */
        case FunctorOp::ORD:
        case FunctorOp::STRLEN:
        case FunctorOp::NEG:
        case FunctorOp::FNEG:
        case FunctorOp::BNOT:
        case FunctorOp::UBNOT:
        case FunctorOp::LNOT:
        case FunctorOp::ULNOT:
        case FunctorOp::TONUMBER:
        case FunctorOp::TOSTRING:
        case FunctorOp::ITOU:
        case FunctorOp::UTOI:
        case FunctorOp::ITOF:
        case FunctorOp::FTOI:
        case FunctorOp::UTOF:
        case FunctorOp::FTOU: return arity == 1;

        /** Binary Functor Operators */
        case FunctorOp::ADD:
        case FunctorOp::SUB:
        case FunctorOp::MUL:
        case FunctorOp::DIV:
        case FunctorOp::EXP:
        case FunctorOp::MOD:
        case FunctorOp::BAND:
        case FunctorOp::BOR:
        case FunctorOp::BXOR:
        case FunctorOp::BSHIFT_L:
        case FunctorOp::BSHIFT_R:
        case FunctorOp::BSHIFT_R_UNSIGNED:
        case FunctorOp::LAND:
        case FunctorOp::LOR:
        case FunctorOp::UADD:
        case FunctorOp::USUB:
        case FunctorOp::UMUL:
        case FunctorOp::UDIV:
        case FunctorOp::UEXP:
        case FunctorOp::UMOD:
        case FunctorOp::UBAND:
        case FunctorOp::UBOR:
        case FunctorOp::UBXOR:
        case FunctorOp::UBSHIFT_L:
        case FunctorOp::UBSHIFT_R:
        case FunctorOp::UBSHIFT_R_UNSIGNED:
        case FunctorOp::ULAND:
        case FunctorOp::ULOR:
        case FunctorOp::FADD:
        case FunctorOp::FSUB:
        case FunctorOp::FMUL:
        case FunctorOp::FDIV:
        case FunctorOp::FEXP: return arity == 2;

        case FunctorOp::RANGE:
        case FunctorOp::URANGE:
        case FunctorOp::FRANGE: return 2 <= arity && arity < 4;

        /** Ternary Functor Operators */
        case FunctorOp::SUBSTR: return arity == 3;

        /** Non-fixed */
        case FunctorOp::MAX:
        case FunctorOp::MIN:
        case FunctorOp::UMAX:
        case FunctorOp::UMIN:
        case FunctorOp::FMAX:
        case FunctorOp::FMIN:
        case FunctorOp::SMAX:
        case FunctorOp::SMIN:
        case FunctorOp::CAT: return arity >= 2;
    }

    UNREACHABLE_BAD_CASE_ANALYSIS
}

/**
 * Return a string representation of a functor.
 */
inline char const* getSymbolForFunctorOp(const FunctorOp op) {
    switch (op) {
        /** Unary Functor Operators */
        case FunctorOp::ITOF: return "itof";
        case FunctorOp::ITOU: return "itou";
        case FunctorOp::UTOF: return "utof";
        case FunctorOp::UTOI: return "utoi";
        case FunctorOp::FTOI: return "ftoi";
        case FunctorOp::FTOU: return "ftou";
        case FunctorOp::ORD: return "ord";
        case FunctorOp::STRLEN: return "strlen";
        case FunctorOp::NEG:
        case FunctorOp::FNEG: return "-";
        case FunctorOp::BNOT:
        case FunctorOp::UBNOT: return "bnot";
        case FunctorOp::LNOT:
        case FunctorOp::ULNOT: return "lnot";
        case FunctorOp::TONUMBER: return "to_number";
        case FunctorOp::TOSTRING: return "to_string";

        /** Binary Functor Operators */
        case FunctorOp::ADD:
        case FunctorOp::FADD:
        case FunctorOp::UADD: return "+";
        case FunctorOp::SUB:
        case FunctorOp::USUB:
        case FunctorOp::FSUB: return "-";
        case FunctorOp::MUL:
        case FunctorOp::UMUL:
        case FunctorOp::FMUL: return "*";
        case FunctorOp::DIV:
        case FunctorOp::UDIV:
        case FunctorOp::FDIV: return "/";
        case FunctorOp::EXP:
        case FunctorOp::FEXP:
        case FunctorOp::UEXP: return "^";
        case FunctorOp::MOD:
        case FunctorOp::UMOD: return "%";
        case FunctorOp::BAND:
        case FunctorOp::UBAND: return "band";
        case FunctorOp::BOR:
        case FunctorOp::UBOR: return "bor";
        case FunctorOp::BXOR:
        case FunctorOp::UBXOR: return "bxor";
        case FunctorOp::BSHIFT_L:
        case FunctorOp::UBSHIFT_L: return "bshl";
        case FunctorOp::BSHIFT_R:
        case FunctorOp::UBSHIFT_R: return "bshr";
        case FunctorOp::BSHIFT_R_UNSIGNED:
        case FunctorOp::UBSHIFT_R_UNSIGNED: return "bshru";
        case FunctorOp::LAND:
        case FunctorOp::ULAND: return "land";
        case FunctorOp::LOR:
        case FunctorOp::ULOR: return "lor";

        case FunctorOp::RANGE:
        case FunctorOp::URANGE:
        case FunctorOp::FRANGE: return "range";

        /* N-ary Functor Operators */
        case FunctorOp::MAX:
        case FunctorOp::UMAX:
        case FunctorOp::FMAX:
        case FunctorOp::SMAX: return "max";
        case FunctorOp::MIN:
        case FunctorOp::UMIN:
        case FunctorOp::FMIN:
        case FunctorOp::SMIN: return "min";
        case FunctorOp::CAT: return "cat";

        /** Ternary Functor Operators */
        case FunctorOp::SUBSTR: return "substr";
    }

    UNREACHABLE_BAD_CASE_ANALYSIS
}

inline std::ostream& operator<<(std::ostream& os, FunctorOp op) {
    return os << getSymbolForFunctorOp(op);
}

/**
 * Check a functor's return type (codomain).
 */
inline TypeAttribute functorReturnType(const FunctorOp op) {
    switch (op) {
        case FunctorOp::ORD:
        case FunctorOp::STRLEN:
        case FunctorOp::NEG:
        case FunctorOp::BNOT:
        case FunctorOp::LNOT:
        case FunctorOp::TONUMBER:
        case FunctorOp::ADD:
        case FunctorOp::SUB:
        case FunctorOp::MUL:
        case FunctorOp::DIV:
        case FunctorOp::EXP:
        case FunctorOp::RANGE:
        case FunctorOp::BAND:
        case FunctorOp::BOR:
        case FunctorOp::BXOR:
        case FunctorOp::BSHIFT_L:
        case FunctorOp::BSHIFT_R:
        case FunctorOp::BSHIFT_R_UNSIGNED:
        case FunctorOp::LAND:
        case FunctorOp::LOR:
        case FunctorOp::MOD:
        case FunctorOp::MAX:
        case FunctorOp::MIN:
        case FunctorOp::FTOI:
        case FunctorOp::UTOI: {
            return TypeAttribute::Signed;
        }
        case FunctorOp::UBNOT:
        case FunctorOp::ITOU:
        case FunctorOp::FTOU:
        case FunctorOp::ULNOT:
        case FunctorOp::UADD:
        case FunctorOp::USUB:
        case FunctorOp::UMUL:
        case FunctorOp::UDIV:
        case FunctorOp::UEXP:
        case FunctorOp::URANGE:
        case FunctorOp::UMAX:
        case FunctorOp::UMIN:
        case FunctorOp::UMOD:
        case FunctorOp::UBAND:
        case FunctorOp::UBOR:
        case FunctorOp::UBXOR:
        case FunctorOp::UBSHIFT_L:
        case FunctorOp::UBSHIFT_R:
        case FunctorOp::UBSHIFT_R_UNSIGNED:
        case FunctorOp::ULAND:
        case FunctorOp::ULOR: {
            return TypeAttribute::Unsigned;
        }
        case FunctorOp::FMAX:
        case FunctorOp::FMIN:
        case FunctorOp::FNEG:
        case FunctorOp::FADD:
        case FunctorOp::FSUB:
        case FunctorOp::ITOF:
        case FunctorOp::UTOF:
        case FunctorOp::FMUL:
        case FunctorOp::FDIV:
        case FunctorOp::FEXP:
        case FunctorOp::FRANGE: {
            return TypeAttribute::Float;
        }
        case FunctorOp::SMAX:
        case FunctorOp::SMIN:
        case FunctorOp::TOSTRING:
        case FunctorOp::CAT:
        case FunctorOp::SUBSTR: {
            return TypeAttribute::Symbol;
        }
    }

    UNREACHABLE_BAD_CASE_ANALYSIS
}

/**
 * Check the type of argument indicated by arg (0-indexed) of a functor op.
 */
inline TypeAttribute functorOpArgType(const size_t arg, const FunctorOp op) {
    switch (op) {
        // Special case
        case FunctorOp::ORD: {
            fatal("ord is a special function that returns a Ram Representation of the element");
        }
        case FunctorOp::ITOF:
        case FunctorOp::ITOU:
        case FunctorOp::NEG:
        case FunctorOp::BNOT:
        case FunctorOp::LNOT:
        case FunctorOp::TOSTRING: {
            assert(arg == 0 && "unary functor out of bound");
            return TypeAttribute::Signed;
        }
        case FunctorOp::FNEG:
        case FunctorOp::FTOI:
        case FunctorOp::FTOU: {
            assert(arg == 0 && "unary functor out of bound");
            return TypeAttribute::Float;
        }
        case FunctorOp::STRLEN:
        case FunctorOp::TONUMBER: {
            assert(arg == 0 && "unary functor out of bound");
            return TypeAttribute::Symbol;
        }
        case FunctorOp::UBNOT:
        case FunctorOp::ULNOT:
        case FunctorOp::UTOI:
        case FunctorOp::UTOF: {
            assert(arg == 0 && "unary functor out of bound");
            return TypeAttribute::Unsigned;
        }
        case FunctorOp::ADD:
        case FunctorOp::SUB:
        case FunctorOp::MUL:
        case FunctorOp::DIV:
        case FunctorOp::EXP:
        case FunctorOp::MOD:
        case FunctorOp::BAND:
        case FunctorOp::BOR:
        case FunctorOp::BXOR:
        case FunctorOp::BSHIFT_L:
        case FunctorOp::BSHIFT_R:
        case FunctorOp::BSHIFT_R_UNSIGNED:
        case FunctorOp::LAND:
        case FunctorOp::LOR: {
            assert(arg < 2 && "binary functor out of bound");
            return TypeAttribute::Signed;
        }
        case FunctorOp::UADD:
        case FunctorOp::USUB:
        case FunctorOp::UMUL:
        case FunctorOp::UDIV:
        case FunctorOp::UEXP:
        case FunctorOp::UMOD:
        case FunctorOp::UBAND:
        case FunctorOp::UBOR:
        case FunctorOp::UBXOR:
        case FunctorOp::UBSHIFT_L:
        case FunctorOp::UBSHIFT_R:
        case FunctorOp::UBSHIFT_R_UNSIGNED:
        case FunctorOp::ULAND:
        case FunctorOp::ULOR: {
            assert(arg < 2 && "binary functor out of bound");
            return TypeAttribute::Unsigned;
        }
        case FunctorOp::FADD:
        case FunctorOp::FSUB:
        case FunctorOp::FMUL:
        case FunctorOp::FDIV:
        case FunctorOp::FEXP: {
            assert(arg < 2 && "binary functor out of bound");
            return TypeAttribute::Float;
        }
        case FunctorOp::SUBSTR: {
            assert(arg < 3 && "ternary functor out of bound");
            if (arg == 0) {
                return TypeAttribute::Symbol;
            } else {
                // In the future: Change to unsigned
                return TypeAttribute::Signed;
            }
        }
        case FunctorOp::MAX:
        case FunctorOp::MIN: {
            return TypeAttribute::Signed;
        }
        case FunctorOp::UMAX:
        case FunctorOp::UMIN: {
            return TypeAttribute::Unsigned;
        }
        case FunctorOp::FMAX:
        case FunctorOp::FMIN: {
            return TypeAttribute::Float;
        }
        case FunctorOp::SMAX:
        case FunctorOp::SMIN:
        case FunctorOp::CAT: {
            return TypeAttribute::Symbol;
        }

        case FunctorOp::RANGE: return TypeAttribute::Signed;
        case FunctorOp::URANGE: return TypeAttribute::Unsigned;
        case FunctorOp::FRANGE: return TypeAttribute::Float;
    }

    UNREACHABLE_BAD_CASE_ANALYSIS
}

/**
 * Indicate whether a functor is overloaded.
 * At the moment, the signed versions are treated as representatives (because parser always returns a signed
 * version).
 */
inline bool isOverloadedFunctor(const FunctorOp functor) {
    switch (functor) {
        /* Unary */
        case FunctorOp::NEG:
        case FunctorOp::BNOT:
        case FunctorOp::LNOT:
        /* Binary */
        case FunctorOp::ADD:
        case FunctorOp::SUB:
        case FunctorOp::MUL:
        case FunctorOp::DIV:
        case FunctorOp::EXP:
        case FunctorOp::BAND:
        case FunctorOp::BOR:
        case FunctorOp::BXOR:
        case FunctorOp::LAND:
        case FunctorOp::LOR:
        case FunctorOp::MOD:
        case FunctorOp::MAX:
        case FunctorOp::MIN:
        case FunctorOp::RANGE: {
            return true;
        }

        default: return false;
    }
}

/**
 * Convert an overloaded functor, so that it works with the requested type.
 * Example: toType = Float, functor = PLUS -> FPLUS (version of plus working on floats)
 */
inline FunctorOp convertOverloadedFunctor(const FunctorOp functor, const TypeAttribute toType) {
    // clang-format off
#define OVERLOAD_SIGNED(op)   if (toType == TypeAttribute::Signed   ) return FunctorOp::op
#define OVERLOAD_UNSIGNED(op) if (toType == TypeAttribute::Unsigned ) return FunctorOp::U##op
#define OVERLOAD_FLOAT(op)    if (toType == TypeAttribute::Float    ) return FunctorOp::F##op
#define OVERLOAD_SYMBOL(op)   if (toType == TypeAttribute::Symbol   ) return FunctorOp::S##op
    // clang-format on

#define CASE_INTEGRAL(op)      \
    case FunctorOp::op: {      \
        OVERLOAD_SIGNED(op);   \
        OVERLOAD_UNSIGNED(op); \
    } break;
#define CASE_NUMERIC(op)       \
    case FunctorOp::op: {      \
        OVERLOAD_SIGNED(op);   \
        OVERLOAD_UNSIGNED(op); \
        OVERLOAD_FLOAT(op);    \
    } break;
#define CASE_ORDERED(op)       \
    case FunctorOp::op: {      \
        OVERLOAD_SIGNED(op);   \
        OVERLOAD_UNSIGNED(op); \
        OVERLOAD_FLOAT(op);    \
        OVERLOAD_SYMBOL(op);   \
    } break;

    switch (functor) {
        default: fatal("functor is not overloaded");

        case FunctorOp::NEG: {
            OVERLOAD_SIGNED(NEG);
            OVERLOAD_FLOAT(NEG);
        } break;

            // clang-format off
        CASE_INTEGRAL(LAND)
        CASE_INTEGRAL(LNOT)
        CASE_INTEGRAL(LOR)

        CASE_INTEGRAL(BAND)
        CASE_INTEGRAL(BNOT)
        CASE_INTEGRAL(BOR)
        CASE_INTEGRAL(BSHIFT_L)
        CASE_INTEGRAL(BSHIFT_R)
        CASE_INTEGRAL(BSHIFT_R_UNSIGNED)
        CASE_INTEGRAL(BXOR)

        CASE_NUMERIC(ADD)
        CASE_NUMERIC(SUB)
        CASE_NUMERIC(MUL)
        CASE_NUMERIC(DIV)
        CASE_INTEGRAL(MOD)
        CASE_NUMERIC(EXP)

        CASE_NUMERIC(RANGE)

        CASE_ORDERED(MAX)
        CASE_ORDERED(MIN)
            // clang-format on
    }

#undef OVERLOAD_SIGNED
#undef OVERLOAD_UNSIGNED
#undef OVERLOAD_FLOAT
#undef OVERLOAD_SYMBOL
#undef CASE_INTERGRAL
#undef CASE_NUMERIC
#undef CASE_ORDERED

    fatal("invalid functor conversion");
}

/**
 * Determines whether a functor should be written using infix notation (e.g. `a + b + c`)
 * or prefix notation (e.g. `+(a,b,c)`)
 */
inline bool isInfixFunctorOp(const FunctorOp op) {
    switch (op) {
        case FunctorOp::ADD:
        case FunctorOp::FADD:
        case FunctorOp::UADD:
        case FunctorOp::SUB:
        case FunctorOp::USUB:
        case FunctorOp::FSUB:
        case FunctorOp::MUL:
        case FunctorOp::FMUL:
        case FunctorOp::UMUL:
        case FunctorOp::DIV:
        case FunctorOp::FDIV:
        case FunctorOp::UDIV:
        case FunctorOp::EXP:
        case FunctorOp::FEXP:
        case FunctorOp::UEXP:
        case FunctorOp::BAND:
        case FunctorOp::UBAND:
        case FunctorOp::BOR:
        case FunctorOp::UBOR:
        case FunctorOp::BXOR:
        case FunctorOp::BSHIFT_L:
        case FunctorOp::UBSHIFT_L:
        case FunctorOp::BSHIFT_R:
        case FunctorOp::UBSHIFT_R:
        case FunctorOp::BSHIFT_R_UNSIGNED:
        case FunctorOp::UBSHIFT_R_UNSIGNED:
        case FunctorOp::UBXOR:
        case FunctorOp::LAND:
        case FunctorOp::ULAND:
        case FunctorOp::LOR:
        case FunctorOp::ULOR:
        case FunctorOp::MOD:
        case FunctorOp::UMOD: {
            return true;
        }

        default: return false;
    }
}

}  // end of namespace souffle
