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

#include <QtTest>

#include "../../src/declarative/SunPath.h"
#include "../../src/declarative/SunPosition.h"

class SunPathTest : public QObject
{
    Q_OBJECT

private slots:
    void pole_data();
    void pole();
};

void SunPathTest::pole_data()
{
    QTest::addColumn<QDateTime>("dateTime");
    QTest::addColumn<QGeoCoordinate>("location");

    const QDateTime dateTime = QDateTime::currentDateTime();

    QTest::newRow("north pole") << dateTime << QGeoCoordinate(90, -135);
    QTest::newRow("south pole") << dateTime << QGeoCoordinate(-90, 45);
}

void SunPathTest::pole()
{
    QFETCH(QDateTime, dateTime);
    QFETCH(QGeoCoordinate, location);

    const SunPath path = SunPath::create(dateTime, location);

    QVERIFY(!path.isValid());
}

QTEST_MAIN(SunPathTest)
#include "sunpathtest.moc"
