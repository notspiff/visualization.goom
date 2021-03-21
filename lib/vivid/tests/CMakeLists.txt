cmake_minimum_required( VERSION 3.8 )
project( vivid_tests VERSION 1.0.0 LANGUAGES CXX )

add_executable( vivid_tests
    main.cpp
    test_conversion_manual.cpp
    test_profiles.cpp
	# test_conversion_fuzzy.cpp	# requires Qt
)

target_include_directories( vivid_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties( vivid_tests
    PROPERTIES
        CXX_STANDARD 20
)

target_link_libraries( vivid_tests
    PRIVATE
        libs::vivid
)
