set(sunpathtest_SRCS
    ../../src/declarative/SunPath.cc
    ../../src/declarative/SunPosition.cc
    sunpathtest.cc
)

add_executable(sunpathtest ${sunpathtest_SRCS})
target_link_libraries(sunpathtest
    Qt5::Gui
    Qt5::Positioning
    Qt5::Test
)
add_test(NAME sunpathtest COMMAND sunpathtest)
