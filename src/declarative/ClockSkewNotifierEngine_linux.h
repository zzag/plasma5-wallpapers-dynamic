/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "ClockSkewNotifierEngine.h"

class LinuxClockSkewNotifierEngine : public ClockSkewNotifierEngine
{
    Q_OBJECT

public:
    ~LinuxClockSkewNotifierEngine() override;

    static LinuxClockSkewNotifierEngine *create(QObject *parent);

private Q_SLOTS:
    void handleTimerCancelled();

private:
    LinuxClockSkewNotifierEngine(int fd, QObject *parent);

    int m_fd;
};
