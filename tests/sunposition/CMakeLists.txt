set(sunpositiontest_SRCS
    ../../src/declarative/sunposition.cpp
    sunpositiontest.cpp
)

add_executable(sunpositiontest ${sunpositiontest_SRCS})
target_link_libraries(sunpositiontest
    Qt5::Gui
    Qt5::Positioning
    Qt5::Test
)
add_test(NAME sunpositiontest COMMAND sunpositiontest)
