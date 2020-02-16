/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QtTest>

#include "../../src/declarative/sunpath.h"
#include "../../src/declarative/sunposition.h"

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
