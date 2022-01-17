#ifndef _QUETZALCOATL_FRONTEND_SCOPE_HPP
#define _QUETZALCOATL_FRONTEND_SCOPE_HPP

#include <unordered_map>
#include <cstddef>

struct Type;

struct Scope {
    std::unordered_map<size_t, const Type*>;
};

class ScopeTable {
    std::vector<Scope> scopes;

public:
    using ScopeID = size_t;

    constexpr const ScopeID global_scope = 0;
};

#endif
