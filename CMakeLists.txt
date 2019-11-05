cmake_minimum_required(VERSION 3.0)

project(plasma5-wallpapers-dynamic)

find_package(ECM 1.4.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(KF5 REQUIRED COMPONENTS
    I18n
    Package
    Plasma
)

find_package(Qt5 CONFIG REQUIRED COMPONENTS
    Core
    Gui
    Positioning
    Qml
)

find_package(Qt5Test CONFIG QUIET)
set_package_properties(Qt5Test PROPERTIES
    PURPOSE "Required for tests"
    TYPE OPTIONAL
)
if (NOT Qt5Test_FOUND)
    set(BUILD_TESTING OFF CACHE BOOL "Build tests.")
endif()

add_subdirectory(data)
add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
