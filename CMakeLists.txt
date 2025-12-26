# SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.18)
project(plasma5-wallpapers-dynamic)

set(KF_MIN_VERSION "6.0.0")
set(QT_MIN_VERSION "6.6.0")

set(PROJECT_VERSION "5.0.0")
set(PROJECT_VERSION_MAJOR 5)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(GenerateExportHeader)
include(WriteBasicConfigVersionFile)
include(ECMGenerateHeaders)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(libavif REQUIRED)
find_package(libexif REQUIRED)
find_package(Plasma REQUIRED)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    I18n
    Package
)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Concurrent
    Core
    DBus
    Gui
    Positioning
    Qml
    Quick
    Xml
)

if (Qt6_VERSION VERSION_GREATER_EQUAL "6.10.0")
    find_package(Qt6QuickPrivate ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)
endif()

add_subdirectory(data)
add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
