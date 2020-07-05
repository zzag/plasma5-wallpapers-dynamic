cmake_minimum_required(VERSION 3.0)
project(plasma5-wallpapers-dynamic)

set(KF5_MIN_VERSION "5.66.0")
set(QT_MIN_VERSION "5.12.0")
set(LIBHEIF_MIN_VERSION "1.3")

set(PROJECT_VERSION "1.0.0")
set(PROJECT_VERSION_MAJOR 1)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(GenerateExportHeader)
include(WriteBasicConfigVersionFile)
include(ECMGenerateHeaders)
include(ECMQMLModules)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

ecm_find_qmlmodule(QtQuick 2.5)
ecm_find_qmlmodule(QtQuick.Controls 2.3)
ecm_find_qmlmodule(QtQuick.Dialogs 1.3)
ecm_find_qmlmodule(QtQuick.Layouts 1.0)
ecm_find_qmlmodule(QtQuick.Window 2.2)
ecm_find_qmlmodule(QtPositioning 5.12)
ecm_find_qmlmodule(org.kde.kcm 1.1)
ecm_find_qmlmodule(org.kde.kirigami 2.10)

find_package(libheif ${LIBHEIF_MIN_VERSION} REQUIRED)
find_package(libexif REQUIRED)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    I18n
    Package
    Plasma
)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Concurrent
    Core
    DBus
    Gui
    Positioning
    Qml
    Quick
)

add_subdirectory(data)
add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
