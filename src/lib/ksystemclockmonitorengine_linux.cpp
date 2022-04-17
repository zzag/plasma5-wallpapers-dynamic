/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "ksystemclockmonitorengine_linux_p.h"

#include <QSocketNotifier>

#include <fcntl.h>
#include <sys/timerfd.h>
#include <unistd.h>

#ifndef TFD_TIMER_CANCEL_ON_SET
#define TFD_TIMER_CANCEL_ON_SET (1 << 1)
#endif

KLinuxSystemClockMonitorEngine *KLinuxSystemClockMonitorEngine::create(QObject *parent)
{
    const int fd = timerfd_create(CLOCK_REALTIME, O_CLOEXEC | O_NONBLOCK);
    if (fd == -1) {
        qWarning("Couldn't create clock skew notifier engine: %s", strerror(errno));
        return nullptr;
    }

    const itimerspec spec = {};
    const int ret = timerfd_settime(fd, TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET, &spec, nullptr);
    if (ret == -1) {
        qWarning("Couldn't create clock skew notifier engine: %s", strerror(errno));
        close(fd);
        return nullptr;
    }

    return new KLinuxSystemClockMonitorEngine(fd, parent);
}

KLinuxSystemClockMonitorEngine::KLinuxSystemClockMonitorEngine(int fd, QObject *parent)
    : KSystemClockMonitorEngine(parent)
    , m_fd(fd)
{
    const QSocketNotifier *notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated,
            this, &KLinuxSystemClockMonitorEngine::handleTimerCancelled);
}

KLinuxSystemClockMonitorEngine::~KLinuxSystemClockMonitorEngine()
{
    close(m_fd);
}

void KLinuxSystemClockMonitorEngine::handleTimerCancelled()
{
    uint64_t expirationCount;
    const ssize_t readCount = read(m_fd, &expirationCount, sizeof(expirationCount));
    if (readCount != -1 || errno != ECANCELED) {
        return;
    }

    Q_EMIT systemClockChanged();
}
