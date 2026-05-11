add_library(Lib STATIC
    type/type_list.c
    type/type_hash.c
    type/type_skiplist.c
    type/trie.c
    type/type_atom_queue.c
    mempool/mp.c
    mempool/mp_thd_lo.c
)

target_include_directories(Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

