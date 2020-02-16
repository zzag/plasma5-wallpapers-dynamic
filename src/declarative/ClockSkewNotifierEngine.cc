/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ClockSkewNotifierEngine.h"
#if defined(Q_OS_LINUX)
#include "ClockSkewNotifierEngine_linux.h"
#endif

ClockSkewNotifierEngine *ClockSkewNotifierEngine::create(QObject *parent)
{
#if defined(Q_OS_LINUX)
    return LinuxClockSkewNotifierEngine::create(parent);
#else
    return nullptr;
#endif
}

ClockSkewNotifierEngine::ClockSkewNotifierEngine(QObject *parent)
    : QObject(parent)
{
}
