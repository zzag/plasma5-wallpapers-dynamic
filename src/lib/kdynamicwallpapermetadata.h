/*
 * SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include "kdaynightdynamicwallpapermetadata.h"
#include "ksolardynamicwallpapermetadata.h"

#include <variant>

using KDynamicWallpaperMetaData = std::variant<KSolarDynamicWallpaperMetaData,
                                               KDayNightDynamicWallpaperMetaData>;
