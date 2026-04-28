add_executable(Entry
    test.c
    main.c
)

target_link_libraries(Entry PRIVATE Lib)