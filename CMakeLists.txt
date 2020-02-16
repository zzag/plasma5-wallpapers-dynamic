cmake_minimum_required(VERSION 3.0)

set(KF5_MIN_VERSION "5.42.0")
set(QT_MIN_VERSION "5.9.0")

project(plasma5-wallpapers-dynamic)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMQMLModules)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

ecm_find_qmlmodule(QtQuick 2.5)
ecm_find_qmlmodule(QtQuick.Controls 2.3)
ecm_find_qmlmodule(QtQuick.Dialogs 1.2)
ecm_find_qmlmodule(QtQuick.Layouts 1.0)
ecm_find_qmlmodule(QtQuick.Window 2.2)
ecm_find_qmlmodule(org.kde.kcm 1.1)
ecm_find_qmlmodule(org.kde.kirigami 2.5)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    Package
    Plasma
)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
    Positioning
    Qml
)

find_package(Qt5Test ${QT_MIN_VERSION} CONFIG QUIET)
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
