/*
 * Copyright (C) 2019 Vlad Zahorodnii <vladzzag@gmail.com>
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

#include "DateTimeWatcher.h"
#if defined(Q_OS_LINUX)
#include "DateTimeWatcher_linux.h"
#endif

class DateTimeWatcherPrivate {
public:
    DateTimeWatcherPrivate(DateTimeWatcher* parent);

    void loadPlatformWatcher();
    void unloadPlatformWatcher();

    DateTimeWatcher* q;
    QScopedPointer<PlatformDateTimeWatcher> platformWatcher;
    bool isActive = false;
};

DateTimeWatcherPrivate::DateTimeWatcherPrivate(DateTimeWatcher* parent)
    : q(parent)
{
}

void DateTimeWatcherPrivate::loadPlatformWatcher()
{
#if defined(Q_OS_LINUX)
    platformWatcher.reset(new LinuxDateTimeWatcher());
#endif

    if (!platformWatcher)
        return;

    if (!platformWatcher->isValid()) {
        platformWatcher.reset();
        return;
    }

    QObject::connect(platformWatcher.data(), &PlatformDateTimeWatcher::dateTimeChanged,
        q, &DateTimeWatcher::dateTimeChanged);
}

void DateTimeWatcherPrivate::unloadPlatformWatcher()
{
    platformWatcher.reset();
}

DateTimeWatcher::DateTimeWatcher(QObject* parent)
    : QObject(parent)
    , d(new DateTimeWatcherPrivate(this))
{
}

DateTimeWatcher::~DateTimeWatcher()
{
}

bool DateTimeWatcher::isActive() const
{
    return d->isActive;
}

void DateTimeWatcher::setActive(bool set)
{
    if (d->isActive == set)
        return;

    d->isActive = set;

    if (d->isActive)
        d->loadPlatformWatcher();
    else
        d->unloadPlatformWatcher();

    emit activeChanged();
}
