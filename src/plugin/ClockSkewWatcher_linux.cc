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

#include "ClockSkewWatcher_linux.h"

#include <QSocketNotifier>

#include <fcntl.h>
#include <sys/timerfd.h>
#include <unistd.h>

LinuxClockSkewWatcher::LinuxClockSkewWatcher(QObject* parent)
    : PlatformClockSkewWatcher(parent)
{
    m_fd = timerfd_create(CLOCK_REALTIME, O_CLOEXEC | O_NONBLOCK);
    if (m_fd == -1) {
        qWarning("Failed to create timer: %s", strerror(errno));
        return;
    }

    const itimerspec spec = {};
    const int ret = timerfd_settime(m_fd, TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET, &spec, nullptr);
    if (ret == -1) {
        qWarning("Failed to initialize disarmed timer: %s", strerror(errno));
        return;
    }

    const QSocketNotifier* notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, &LinuxClockSkewWatcher::slotTimerCancelled);

    m_isValid = true;
}

LinuxClockSkewWatcher::~LinuxClockSkewWatcher()
{
    if (m_fd != -1)
        close(m_fd);
}

bool LinuxClockSkewWatcher::isValid() const
{
    return m_isValid;
}

void LinuxClockSkewWatcher::slotTimerCancelled()
{
    uint64_t expirationCount;
    read(m_fd, &expirationCount, sizeof(expirationCount));

    emit clockSkewed();
}
