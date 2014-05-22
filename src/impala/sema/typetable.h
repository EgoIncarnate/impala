#ifndef IMPALA_SEMA_TYPETABLE_H
#define IMPALA_SEMA_TYPETABLE_H

#include "thorin/util/hash.h"

#include "impala/sema/errorhandler.h"
#include "impala/sema/unifiable.h"

namespace impala {

template<class T> struct TypetableHash {
    size_t operator () (const T* t) const { return t->hash(); }
};
template<class T> struct TypetableEqual {
    bool operator () (const T* t1, const T* t2) const { return t1->equal(t2); }
};
template<class T> using TypetableSet = thorin::HashSet<T*, TypetableHash<T>, TypetableEqual<T>>;

class TypeTable : public ErrorHandler {
public:
    TypeTable();
    ~TypeTable();

#define IMPALA_TYPE(itype, atype) PrimType type_##itype() { return itype##_; }
#include "impala/tokenlist.h"
    TypeError   type_error() { return type_error_; }
    Trait       trait_error() { return trait_error_; }
    Bound       bound_error() { return bound_error_; }
    NoRetType   type_noret() { return type_noret_; }
    PrimType    type(PrimTypeKind kind);
    FnType      fn_type(ArrayRef<Type> params) { return join(new FnTypeNode(*this, params)); }
    TupleType   tuple_type(ArrayRef<Type> elems) { return join(new TupleTypeNode(*this, elems)); }
    TupleType   unit() { return tuple_type({}); }
    StructType  struct_type(const StructDecl* struct_decl) { return join(new StructTypeNode(*this, struct_decl)); }
    OwnedPtr    owned_ptr(Type referenced_type) { return join(new OwnedPtrNode(*this, referenced_type)); }
    BorrowedPtr borrowd_ptr(Type referenced_type) { return join(new BorrowedPtrNode(*this, referenced_type)); }
    TypeVar     type_var(Symbol name = Symbol()) { return join(new TypeVarNode(*this, name)); }
    UnknownType unknown_type() { return join(new UnknownTypeNode(*this)); }
    Bound       bound(Trait trait, ArrayRef<Type> args) { return join(new BoundNode(trait, args)); }
    Trait       trait(const TraitDecl* trait_decl) { return join(new TraitNode(*this, trait_decl)); }
    Impl        impl(const ImplItem* impl, Bound bound, Type type) { return join(new ImplNode(*this, impl, bound, type)); }

    /// Unify a type and return its representative.
    template<class T> Proxy<T> unify(Proxy<T> proxy) { return unify(*proxy)->template as<T>(); }
    const Unifiable* unify(const Unifiable*);
    void verify() const; ///< Checks if all types in the type tables are sane and correctly unified.

private:
    template<class T> 
    Proxy<T> join(T* tn) { garbage_.push_back(tn); return Proxy<T>(tn); }

    TypetableSet<const Unifiable> unifiables_;
    std::vector<const Unifiable*> garbage_;
    NoRetType type_noret_;
#define IMPALA_TYPE(itype, atype) PrimType itype##_;
#include "impala/tokenlist.h"
    TypeError type_error_;
    Trait trait_error_;
    Bound bound_error_;
};

}

#endif
