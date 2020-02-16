/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

class ClockSkewNotifierEngine : public QObject
{
    Q_OBJECT

public:
    static ClockSkewNotifierEngine *create(QObject *parent);

protected:
    explicit ClockSkewNotifierEngine(QObject *parent);

Q_SIGNALS:
    void clockSkewed();
};
