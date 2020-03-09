/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include <QObject>

class KSystemClockMonitorEngine : public QObject
{
    Q_OBJECT

public:
    static KSystemClockMonitorEngine *create(QObject *parent);

protected:
    explicit KSystemClockMonitorEngine(QObject *parent);

Q_SIGNALS:
    void systemClockChanged();
};
