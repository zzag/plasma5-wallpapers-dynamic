set(sunpositiontest_SRCS
    ../../src/plugin/SunPosition.cc
    sunpositiontest.cc
)

add_executable(sunpositiontest ${sunpositiontest_SRCS})
target_link_libraries(sunpositiontest Qt5::Gui Qt5::Test)
add_test(NAME sunpositiontest COMMAND sunpositiontest)
