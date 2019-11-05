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

#include <QGeoCoordinate>

#include "../../src/plugin/SunPosition.h"

const static qreal s_precision = 0.01;

class SunPositionTest : public QObject
{
    Q_OBJECT

private slots:
    void location_data();
    void location();
};

void SunPositionTest::location_data()
{
    QTest::addColumn<QDateTime>("dateTime");
    QTest::addColumn<QGeoCoordinate>("location");
    QTest::addColumn<qreal>("elevation");
    QTest::addColumn<qreal>("azimuth");

    const QTimeZone gmt = QTimeZone("UTC+00:00");
    const QTimeZone eet = QTimeZone("UTC+02:00");
    const QTimeZone pst = QTimeZone("UTC-08:00");
    const QTimeZone nzdt = QTimeZone("UTC+13:00");

    const QGeoCoordinate coordinateGreenwich(51.47777778, 0.00138889);
    const QGeoCoordinate coordinateKyiv(50.45000000, 30.52333333);
    const QGeoCoordinate coordinateSeattle(47.60972222, -122.33305556);
    const QGeoCoordinate coordinateAuckland(-36.84055556, 174.74000000);

    QTest::newRow("10.10.2019 01:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(1, 0, 0), gmt)
        << coordinateGreenwich
        << -42.52
        << 24.91;

    QTest::newRow("10.10.2019 02:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(2, 0, 0), gmt)
        << coordinateGreenwich
        << -37.29
        << 43.16;

    QTest::newRow("10.10.2019 03:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(3, 0, 0), gmt)
        << coordinateGreenwich
        << -30.04
        << 58.86;

    QTest::newRow("10.10.2019 04:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(4, 0, 0), gmt)
        << coordinateGreenwich
        << -21.55
        << 72.49;

    QTest::newRow("10.10.2019 05:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(5, 0, 0), gmt)
        << coordinateGreenwich
        << -12.40
        << 84.83;

    QTest::newRow("10.10.2019 06:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(6, 0, 0), gmt)
        << coordinateGreenwich
        << -3.01
        << 96.58;

    QTest::newRow("10.10.2019 07:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(7, 0, 0), gmt)
        << coordinateGreenwich
        << 6.11
        << 108.41;

    QTest::newRow("10.10.2019 08:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(8, 0, 0), gmt)
        << coordinateGreenwich
        << 14.50
        << 120.88;

    QTest::newRow("10.10.2019 09:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(9, 0, 0), gmt)
        << coordinateGreenwich
        << 21.85
        << 134.53;

    QTest::newRow("10.10.2019 10:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(10, 0, 0), gmt)
        << coordinateGreenwich
        << 27.57
        << 149.69;

    QTest::newRow("10.10.2019 11:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(11, 0, 0), gmt)
        << coordinateGreenwich
        << 31.05
        << 166.32;

    QTest::newRow("10.10.2019 12:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(12, 0, 0), gmt)
        << coordinateGreenwich
        << 31.84
        << 183.78;

    QTest::newRow("10.10.2019 13:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(13, 0, 0), gmt)
        << coordinateGreenwich
        << 29.82
        << 200.98;

    QTest::newRow("10.10.2019 14:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(14, 0, 0), gmt)
        << coordinateGreenwich
        << 25.28
        << 217.00;

    QTest::newRow("10.10.2019 15:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(15, 0, 0), gmt)
        << coordinateGreenwich
        << 18.76
        << 231.46;

    QTest::newRow("10.10.2019 16:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(16, 0, 0), gmt)
        << coordinateGreenwich
        << 10.87
        << 244.52;

    QTest::newRow("10.10.2019 17:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(17, 0, 0), gmt)
        << coordinateGreenwich
        << 2.26
        << 256.63;

    QTest::newRow("10.10.2019 18:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(18, 0, 0), gmt)
        << coordinateGreenwich
        << -7.23
        << 268.32;

    QTest::newRow("10.10.2019 19:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(19, 0, 0), gmt)
        << coordinateGreenwich
        << -16.57
        << 280.22;

    QTest::newRow("10.10.2019 20:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(20, 0, 0), gmt)
        << coordinateGreenwich
        << -25.53
        << 293.01;

    QTest::newRow("10.10.2019 21:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(21, 0, 0), gmt)
        << coordinateGreenwich
        << -33.62
        << 307.43;

    QTest::newRow("10.10.2019 22:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(22, 0, 0), gmt)
        << coordinateGreenwich
        << -40.14
        << 324.22;

    QTest::newRow("10.10.2019 23:00:00 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(23, 0, 0), gmt)
        << coordinateGreenwich
        << -44.28
        << 343.61;

    QTest::newRow("10.10.2019 23:59:59 (GMT)")
        << QDateTime(QDate(2019, 10, 10), QTime(23, 59, 59), gmt)
        << coordinateGreenwich
        << -45.26
        << 4.61;

    QTest::newRow("10.10.2019 01:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(1, 0, 0), eet)
        << coordinateKyiv
        << -43.28
        << 25.98;

    QTest::newRow("10.10.2019 02:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(2, 0, 0), eet)
        << coordinateKyiv
        << -37.78
        << 44.27;

    QTest::newRow("10.10.2019 03:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(3, 0, 0), eet)
        << coordinateKyiv
        << -30.26
        << 59.85;

    QTest::newRow("10.10.2019 04:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(4, 0, 0), eet)
        << coordinateKyiv
        << -21.51
        << 73.30;

    QTest::newRow("10.10.2019 05:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(5, 0, 0), eet)
        << coordinateKyiv
        << -12.14
        << 85.44;

    QTest::newRow("10.10.2019 06:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(6, 0, 0), eet)
        << coordinateKyiv
        << -2.52
        << 97.02;

    QTest::newRow("10.10.2019 07:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(7, 0, 0), eet)
        << coordinateKyiv
        << 6.76
        << 108.69;

    QTest::newRow("10.10.2019 08:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(8, 0, 0), eet)
        << coordinateKyiv
        << 15.33
        << 121.08;

    QTest::newRow("10.10.2019 09:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(9, 0, 0), eet)
        << coordinateKyiv
        << 22.83
        << 134.71;

    QTest::newRow("10.10.2019 10:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(10, 0, 0), eet)
        << coordinateKyiv
        << 28.65
        << 149.95;

    QTest::newRow("10.10.2019 11:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(11, 0, 0), eet)
        << coordinateKyiv
        << 32.15
        << 166.76;

    QTest::newRow("10.10.2019 12:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(12, 0, 0), eet)
        << coordinateKyiv
        << 32.87
        << 184.43;

    QTest::newRow("10.10.2019 13:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(13, 0, 0), eet)
        << coordinateKyiv
        << 30.69
        << 201.79;

    QTest::newRow("10.10.2019 14:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(14, 0, 0), eet)
        << coordinateKyiv
        << 25.92
        << 217.84;

    QTest::newRow("10.10.2019 15:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(15, 0, 0), eet)
        << coordinateKyiv
        << 19.17
        << 232.23;

    QTest::newRow("10.10.2019 16:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(16, 0, 0), eet)
        << coordinateKyiv
        << 11.03
        << 245.15;

    QTest::newRow("10.10.2019 17:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(17, 0, 0), eet)
        << coordinateKyiv
        << 2.20
        << 257.08;

    QTest::newRow("10.10.2019 18:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(18, 0, 0), eet)
        << coordinateKyiv
        << -7.51
        << 268.61;

    QTest::newRow("10.10.2019 19:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(19, 0, 0), eet)
        << coordinateKyiv
        << -17.05
        << 280.36;

    QTest::newRow("10.10.2019 20:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(20, 0, 0), eet)
        << coordinateKyiv
        << -26.21
        << 293.03;

    QTest::newRow("10.10.2019 21:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(21, 0, 0), eet)
        << coordinateKyiv
        << -34.47
        << 307.44;

    QTest::newRow("10.10.2019 22:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(22, 0, 0), eet)
        << coordinateKyiv
        << -41.14
        << 324.35;

    QTest::newRow("10.10.2019 23:00:00 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(23, 0, 0), eet)
        << coordinateKyiv
        << -45.33
        << 344.04;

    QTest::newRow("10.10.2019 23:59:59 (EET)")
        << QDateTime(QDate(2019, 10, 10), QTime(23, 59, 59), eet)
        << coordinateKyiv
        << -46.22
        << 5.43;

    QTest::newRow("10.10.2019 01:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(1, 0, 0), pst)
        << coordinateSeattle
        << -46.79
        << 23.41;

    QTest::newRow("10.10.2019 02:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(2, 0, 0), pst)
        << coordinateSeattle
        << -41.27
        << 42.74;

    QTest::newRow("10.10.2019 03:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(3, 0, 0), pst)
        << coordinateSeattle
        << -33.44
        << 58.75;

    QTest::newRow("10.10.2019 04:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(4, 0, 0), pst)
        << coordinateSeattle
        << -24.26
        << 72.19;

    QTest::newRow("10.10.2019 05:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(5, 0, 0), pst)
        << coordinateSeattle
        << -14.38
        << 84.04;

    QTest::newRow("10.10.2019 06:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(6, 0, 0), pst)
        << coordinateSeattle
        << -4.24
        << 95.17;

    QTest::newRow("10.10.2019 07:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(7, 0, 0), pst)
        << coordinateSeattle
        << 5.75
        << 106.31;

    QTest::newRow("10.10.2019 08:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(8, 0, 0), pst)
        << coordinateSeattle
        << 15.0
        << 118.13;

    QTest::newRow("10.10.2019 09:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(9, 0, 0), pst)
        << coordinateSeattle
        << 23.28
        << 131.24;

    QTest::newRow("10.10.2019 10:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(10, 0, 0), pst)
        << coordinateSeattle
        << 29.93
        << 146.16;

    QTest::newRow("10.10.2019 11:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(11, 0, 0), pst)
        << coordinateSeattle
        << 34.26
        << 163.01;

    QTest::newRow("10.10.2019 12:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(12, 0, 0), pst)
        << coordinateSeattle
        << 35.64
        << 181.13;

    QTest::newRow("10.10.2019 13:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(13, 0, 0), pst)
        << coordinateSeattle
        << 33.84
        << 199.14;

    QTest::newRow("10.10.2019 14:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(14, 0, 0), pst)
        << coordinateSeattle
        << 29.16
        << 215.75;

    QTest::newRow("10.10.2019 15:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(15, 0, 0), pst)
        << coordinateSeattle
        << 22.25
        << 230.41;

    QTest::newRow("10.10.2019 16:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(16, 0, 0), pst)
        << coordinateSeattle
        << 13.8
        << 243.3;

    QTest::newRow("10.10.2019 17:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(17, 0, 0), pst)
        << coordinateSeattle
        << 4.46
        << 254.98;

    QTest::newRow("10.10.2019 18:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(18, 0, 0), pst)
        << coordinateSeattle
        << -5.63
        << 266.05;

    QTest::newRow("10.10.2019 19:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(19, 0, 0), pst)
        << coordinateSeattle
        << -15.78
        << 277.21;

    QTest::newRow("10.10.2019 20:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(20, 0, 0), pst)
        << coordinateSeattle
        << -25.63
        << 289.18;

    QTest::newRow("10.10.2019 21:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(21, 0, 0), pst)
        << coordinateSeattle
        << -34.73
        << 302.86;

    QTest::newRow("10.10.2019 22:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(22, 0, 0), pst)
        << coordinateSeattle
        << -42.37
        << 319.26;

    QTest::newRow("10.10.2019 23:00:00 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(23, 0, 0), pst)
        << coordinateSeattle
        << -47.59
        << 339.06;

    QTest::newRow("10.10.2019 23:59:59 (PST)")
        << QDateTime(QDate(2019, 10, 10), QTime(23, 59, 59), pst)
        << coordinateSeattle
        << -49.33
        << 1.45;

    QTest::newRow("10.10.2019 01:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(1, 0, 0), nzdt)
        << coordinateAuckland
        << -46.85
        << 183.05;

    QTest::newRow("10.10.2019 02:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(2, 0, 0), nzdt)
        << coordinateAuckland
        << -45.22
        << 161.62;

    QTest::newRow("10.10.2019 03:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(3, 0, 0), nzdt)
        << coordinateAuckland
        << -39.58
        << 142.86;

    QTest::newRow("10.10.2019 04:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(4, 0, 0), nzdt)
        << coordinateAuckland
        << -31.1
        << 127.77;

    QTest::newRow("10.10.2019 05:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(5, 0, 0), nzdt)
        << coordinateAuckland
        << -20.87
        << 115.67;

    QTest::newRow("10.10.2019 06:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(6, 0, 0), nzdt)
        << coordinateAuckland
        << -9.61
        << 105.49;

    QTest::newRow("10.10.2019 07:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(7, 0, 0), nzdt)
        << coordinateAuckland
        << 2.42
        << 96.33;

    QTest::newRow("10.10.2019 08:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(8, 0, 0), nzdt)
        << coordinateAuckland
        << 14.21
        << 87.37;

    QTest::newRow("10.10.2019 09:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(9, 0, 0), nzdt)
        << coordinateAuckland
        << 26.08
        << 77.78;

    QTest::newRow("10.10.2019 10:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(10, 0, 0), nzdt)
        << coordinateAuckland
        << 37.5
        << 66.48;

    QTest::newRow("10.10.2019 11:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(11, 0, 0), nzdt)
        << coordinateAuckland
        << 47.83
        << 51.78;

    QTest::newRow("10.10.2019 12:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(12, 0, 0), nzdt)
        << coordinateAuckland
        << 55.85
        << 31.28;

    QTest::newRow("10.10.2019 13:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(13, 0, 0), nzdt)
        << coordinateAuckland
        << 59.56
        << 4.04;

    QTest::newRow("10.10.2019 14:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(14, 0, 0), nzdt)
        << coordinateAuckland
        << 57.42
        << 335.6;

    QTest::newRow("10.10.2019 15:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(15, 0, 0), nzdt)
        << coordinateAuckland
        << 50.38
        << 313.12;

    QTest::newRow("10.10.2019 16:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(16, 0, 0), nzdt)
        << coordinateAuckland
        << 40.55
        << 297.04;

    QTest::newRow("10.10.2019 17:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(17, 0, 0), nzdt)
        << coordinateAuckland
        << 29.37
        << 284.98;

    QTest::newRow("10.10.2019 18:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(18, 0, 0), nzdt)
        << coordinateAuckland
        << 17.58
        << 275.03;

    QTest::newRow("10.10.2019 19:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(19, 0, 0), nzdt)
        << coordinateAuckland
        << 5.7
        << 265.97;

    QTest::newRow("10.10.2019 20:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(20, 0, 0), nzdt)
        << coordinateAuckland
        << -6.25
        << 256.92;

    QTest::newRow("10.10.2019 21:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(21, 0, 0), nzdt)
        << coordinateAuckland
        << -17.69
        << 247.09;

    QTest::newRow("10.10.2019 22:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(22, 0, 0), nzdt)
        << coordinateAuckland
        << -28.22
        << 235.6;

    QTest::newRow("10.10.2019 23:00:00 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(23, 0, 0), nzdt)
        << coordinateAuckland
        << -37.22
        << 221.43;

    QTest::newRow("10.10.2019 23:59:59 (NZDT)")
        << QDateTime(QDate(2019, 10, 10), QTime(23, 59, 59), nzdt)
        << coordinateAuckland
        << -43.7
        << 203.73;
}

void SunPositionTest::location()
{
    QFETCH(QDateTime, dateTime);
    QFETCH(QGeoCoordinate, location);
    QFETCH(qreal, elevation);
    QFETCH(qreal, azimuth);

    const SunPosition position(dateTime, location);

    QVERIFY(std::abs(position.elevation() - elevation) < s_precision);
    QVERIFY(std::abs(position.azimuth() - azimuth) < s_precision);
}

QTEST_MAIN(SunPositionTest)
#include "sunpositiontest.moc"
