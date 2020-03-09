/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "ksystemclockmonitorengine_p.h"
#if defined(Q_OS_LINUX)
#include "ksystemclockmonitorengine_linux_p.h"
#endif

KSystemClockMonitorEngine *KSystemClockMonitorEngine::create(QObject *parent)
{
#if defined(Q_OS_LINUX)
    return KLinuxSystemClockMonitorEngine::create(parent);
#else
    return nullptr;
#endif
}

KSystemClockMonitorEngine::KSystemClockMonitorEngine(QObject *parent)
    : QObject(parent)
{
}
