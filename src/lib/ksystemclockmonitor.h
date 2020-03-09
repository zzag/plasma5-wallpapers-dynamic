/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include "kdynamicwallpaper_export.h"

#include <QObject>

class KDYNAMICWALLPAPER_EXPORT KSystemClockMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)

public:
    explicit KSystemClockMonitor(QObject *parent = nullptr);
    ~KSystemClockMonitor() override;

    bool isActive() const;
    void setActive(bool active);

signals:
    void activeChanged();
    void systemClockChanged();

private:
    class Private;
    QScopedPointer<Private> d;
};
