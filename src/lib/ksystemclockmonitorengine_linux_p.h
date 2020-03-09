/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include "ksystemclockmonitorengine_p.h"

class KLinuxSystemClockMonitorEngine : public KSystemClockMonitorEngine
{
    Q_OBJECT

public:
    ~KLinuxSystemClockMonitorEngine() override;

    static KLinuxSystemClockMonitorEngine *create(QObject *parent);

private Q_SLOTS:
    void handleTimerCancelled();

private:
    KLinuxSystemClockMonitorEngine(int fd, QObject *parent);

    int m_fd;
};
