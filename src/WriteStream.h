/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file WriteStream.h
 *
 ***********************************************************************/

#pragma once

#include "IODirectives.h"
#include "RamTypes.h"
#include "SymbolTable.h"

#include <cassert>
#include <string>
#include <vector>

namespace souffle {

class WriteStream {
public:
    WriteStream(const std::vector<RamPrimitiveType>& symbolMask, const SymbolTable& symbolTable,
            const size_t auxiliaryArity, bool summary = false)
            : symbolMask(symbolMask), symbolTable(symbolTable), summary(summary),
              arity(symbolMask.size() - auxiliaryArity) {}
    template <typename T>
    void writeAll(const T& relation) {
        if (summary) {
            return writeSize(relation.size());
        }
        auto lease = symbolTable.acquireLock();
        (void)lease;
        if (arity == 0) {
            if (relation.begin() != relation.end()) {
                writeNullary();
            }
            return;
        }
        for (const auto& current : relation) {
            writeNext(current);
        }
    }
    template <typename T>
    void writeSize(const T& relation) {
        writeSize(relation.size());
    }

    virtual ~WriteStream() = default;

protected:
    const std::vector<RamPrimitiveType>& symbolMask;
    const SymbolTable& symbolTable;
    const bool summary;
    const size_t arity;

    virtual void writeNullary() = 0;
    virtual void writeNextTuple(const RamDomain* tuple) = 0;
    virtual void writeSize(std::size_t size) {
        assert(false && "attempting to print size of a write operation");
    }
    template <typename Tuple>
    void writeNext(const Tuple tuple) {
        writeNextTuple(tuple.data);
    }
    void writeNextTupleElement(std::ostream& destination, RamPrimitiveType type, RamDomain value) {
        switch (type) {
            case RamPrimitiveType::String:
                destination << symbolTable.unsafeResolve(value);
                break;
            case RamPrimitiveType::Signed:
                destination << value;
                break;
            case RamPrimitiveType::Unsigned:
                destination << ramBitCast<RamUnsigned>(value);
                break;
            case RamPrimitiveType::Float:
                destination << ramBitCast<RamFloat>(value);
                break;
            case RamPrimitiveType::Record:
                assert(false && "Record not suported here");  // What should happen here?
        }
    }
};

class WriteStreamFactory {
public:
    virtual std::unique_ptr<WriteStream> getWriter(const std::vector<RamPrimitiveType>& symbolMask,
            const SymbolTable& symbolTable, const IODirectives& ioDirectives,
            const size_t auxiliaryArity) = 0;
    virtual const std::string& getName() const = 0;
    virtual ~WriteStreamFactory() = default;
};

template <>
inline void WriteStream::writeNext(const RamDomain* tuple) {
    writeNextTuple(tuple);
}

} /* namespace souffle */
