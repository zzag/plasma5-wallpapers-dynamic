/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "ksystemclockmonitor.h"
#include "ksystemclockmonitorengine_p.h"

/*!
 * \class KSystemClockMonitor
 * \brief The KSystemClockMonitor class provides a way for monitoring system clock changes.
 *
 * The KSystemClockMonitor class makes it possible to detect discontinuous changes to the system
 * clock. Such changes are usually initiated by the user adjusting values in the Date and Time
 * KCM or calls made to functions like settimeofday().
 */

class KSystemClockMonitor::Private
{
public:
    void loadMonitorEngine();
    void unloadMonitorEngine();

    KSystemClockMonitor *monitor = nullptr;
    KSystemClockMonitorEngine *engine = nullptr;
    bool isActive = false;
};

void KSystemClockMonitor::Private::loadMonitorEngine()
{
    engine = KSystemClockMonitorEngine::create(monitor);

    if (engine) {
        QObject::connect(engine, &KSystemClockMonitorEngine::systemClockChanged,
                         monitor, &KSystemClockMonitor::systemClockChanged);
    }
}

void KSystemClockMonitor::Private::unloadMonitorEngine()
{
    if (!engine)
        return;

    QObject::disconnect(engine, &KSystemClockMonitorEngine::systemClockChanged,
                        monitor, &KSystemClockMonitor::systemClockChanged);
    engine->deleteLater();

    engine = nullptr;
}

/*!
 * Constructs an inactive KSystemClockMonitor object with the given \p parent.
 */
KSystemClockMonitor::KSystemClockMonitor(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    d->monitor = this;
}

/*!
 * Destructs the KSystemClockMonitor object.
 */
KSystemClockMonitor::~KSystemClockMonitor()
{
}

/*!
 * Returns \c true if the KSystemClockMonitor is active; otherwise returns \c false.
 */
bool KSystemClockMonitor::isActive() const
{
    return d->isActive;
}

/*!
 * Sets the active status of the KSystemClockMonitor to \p active.
 *
 * systemClockChanged() signal won't be emitted while the monitor is inactive.
 *
 * The monitor is inactive by default.
 *
 * @see activeChanged
 */
void KSystemClockMonitor::setActive(bool set)
{
    if (d->isActive == set)
        return;

    d->isActive = set;

    if (d->isActive)
        d->loadMonitorEngine();
    else
        d->unloadMonitorEngine();

    Q_EMIT activeChanged();
}

/*!
 * @fn void KSystemClockMonitor::activeChanged()
 *
 * This signal is emitted when the active property has been changed.
 */

/*!
 * @fn void KSystemClockMonitor::systemClockChanged()
 *
 * This signal is emitted when the system clock has been changed.
 */
