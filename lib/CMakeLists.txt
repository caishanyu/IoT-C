add_library(Lib STATIC
    type/type_list.c
)

target_include_directories(Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

