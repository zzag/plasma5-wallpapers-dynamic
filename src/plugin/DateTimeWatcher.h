/*
 * Copyright (C) 2019 Vlad Zagorodniy <vladzzag@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <QObject>

class DateTimeWatcherPrivate;

class DateTimeWatcher : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)

public:
    explicit DateTimeWatcher(QObject* parent = nullptr);
    ~DateTimeWatcher() override;

    bool isActive() const;
    void setActive(bool set);

signals:
    void activeChanged();
    void dateTimeChanged();

private:
    QScopedPointer<DateTimeWatcherPrivate> d;

    Q_DISABLE_COPY(DateTimeWatcher)
};
