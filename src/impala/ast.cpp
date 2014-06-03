#include "impala/ast.h"

namespace impala {

const char* Visibility::str() {
    if (visibility_ == Pub)  return "pub ";
    if (visibility_ == Priv) return "priv ";
    return "";
}

const FnASTType* FnASTType::ret_fn_type() const {
    if (!elems().empty()) {
        if (auto fn_type = elems().back()->isa<FnASTType>())
            return fn_type;
    }
    return nullptr;
}

PrimTypeKind LiteralExpr::literal2type() const {
    switch (kind()) {
#define IMPALA_LIT(itype, atype) \
        case LIT_##itype: return PrimType_##itype;
#include "impala/tokenlist.h"
        case LIT_bool:    return PrimType_bool;
        default: THORIN_UNREACHABLE;
    }
}

uint64_t LiteralExpr::get_u64() const { return thorin::bcast<uint64_t, thorin::Box>(box()); }

bool IfExpr::has_else() const {
    if (auto block = else_expr_->isa<BlockExpr>())
        return !block->empty();
    return true;
}

//------------------------------------------------------------------------------

/*
 * is_lvalue
 */

bool PathExpr::is_lvalue() const { 
    if (value_decl())
        return value_decl()->is_mut();
    return false;
}

bool MapExpr::is_lvalue() const {
    return (lhs()->type().isa<ArrayType>() || lhs()->type().isa<TupleType>()) ?  lhs()->is_lvalue() : false;
}

bool PrefixExpr::is_lvalue() const {
    return kind() == MUL && rhs()->is_lvalue();
}

bool FieldExpr::is_lvalue() const {
    return true; // TODO
}

//------------------------------------------------------------------------------

}
