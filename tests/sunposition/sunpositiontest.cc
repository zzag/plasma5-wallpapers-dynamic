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

#include "../../src/declarative/SunPosition.h"

const static qreal s_precision = 0.01;

class SunPositionTest : public QObject
{
    Q_OBJECT

private slots:
    void location_data();
    void location();
    void midnight_data();
    void midnight();
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

    QTest::newRow("10 Nov 2019 01:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(1, 0, 0), gmt)
        << coordinateGreenwich
        << -52.35
        << 30.73;

    QTest::newRow("10 Nov 2019 02:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(2, 0, 0), gmt)
        << coordinateGreenwich
        << -46.25
        << 50.74;

    QTest::newRow("10 Nov 2019 03:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(3, 0, 0), gmt)
        << coordinateGreenwich
        << -38.27
        << 66.91;

    QTest::newRow("10 Nov 2019 04:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(4, 0, 0), gmt)
        << coordinateGreenwich
        << -29.32
        << 80.44;

    QTest::newRow("10 Nov 2019 05:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(5, 0, 0), gmt)
        << coordinateGreenwich
        << -20.02
        << 92.47;

    QTest::newRow("10 Nov 2019 06:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(6, 0, 0), gmt)
        << coordinateGreenwich
        << -10.78
        << 103.87;

    QTest::newRow("10 Nov 2019 07:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(7, 0, 0), gmt)
        << coordinateGreenwich
        << -1.87
        << 115.29;

    QTest::newRow("10 Nov 2019 08:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(8, 0, 0), gmt)
        << coordinateGreenwich
        << 6.08
        << 127.22;

    QTest::newRow("10 Nov 2019 09:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(9, 0, 0), gmt)
        << coordinateGreenwich
        << 12.75
        << 140.04;

    QTest::newRow("10 Nov 2019 10:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(10, 0, 0), gmt)
        << coordinateGreenwich
        << 17.81
        << 153.94;

    QTest::newRow("10 Nov 2019 11:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(11, 0, 0), gmt)
        << coordinateGreenwich
        << 20.78
        << 168.8;

    QTest::newRow("10 Nov 2019 12:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(12, 0, 0), gmt)
        << coordinateGreenwich
        << 21.34
        << 184.14;

    QTest::newRow("10 Nov 2019 13:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(13, 0, 0), gmt)
        << coordinateGreenwich
        << 19.43
        << 199.3;

    QTest::newRow("10 Nov 2019 14:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(14, 0, 0), gmt)
        << coordinateGreenwich
        << 15.27
        << 213.66;

    QTest::newRow("10 Nov 2019 15:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(15, 0, 0), gmt)
        << coordinateGreenwich
        << 9.25
        << 226.95;

    QTest::newRow("10 Nov 2019 16:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(16, 0, 0), gmt)
        << coordinateGreenwich
        << 1.99
        << 239.24;

    QTest::newRow("10 Nov 2019 17:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(17, 0, 0), gmt)
        << coordinateGreenwich
        << -6.73
        << 250.82;

    QTest::newRow("10 Nov 2019 18:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(18, 0, 0), gmt)
        << coordinateGreenwich
        << -15.84
        << 262.13;

    QTest::newRow("10 Nov 2019 19:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(19, 0, 0), gmt)
        << coordinateGreenwich
        << -25.17
        << 273.76;

    QTest::newRow("10 Nov 2019 20:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(20, 0, 0), gmt)
        << coordinateGreenwich
        << -34.37
        << 286.45;

    QTest::newRow("10 Nov 2019 21:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(21, 0, 0), gmt)
        << coordinateGreenwich
        << -42.91
        << 301.23;

    QTest::newRow("10 Nov 2019 22:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(22, 0, 0), gmt)
        << coordinateGreenwich
        << -50.06
        << 319.35;

    QTest::newRow("10 Nov 2019 23:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(23, 0, 0), gmt)
        << coordinateGreenwich
        << -54.69
        << 341.67;

    QTest::newRow("10 Nov 2019 23:59:59 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(23, 59, 59), gmt)
        << coordinateGreenwich
        << -55.64
        << 6.82;

    QTest::newRow("10 Nov 2019 01:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(1, 0, 0), eet)
        << coordinateKyiv
        << -53.03
        << 32.2;

    QTest::newRow("10 Nov 2019 02:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(2, 0, 0), eet)
        << coordinateKyiv
        << -46.61
        << 52.19;

    QTest::newRow("10 Nov 2019 03:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(3, 0, 0), eet)
        << coordinateKyiv
        << -38.34
        << 68.15;

    QTest::newRow("10 Nov 2019 04:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(4, 0, 0), eet)
        << coordinateKyiv
        << -29.14
        << 81.42;

    QTest::newRow("10 Nov 2019 05:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(5, 0, 0), eet)
        << coordinateKyiv
        << -19.62
        << 93.23;

    QTest::newRow("10 Nov 2019 06:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(6, 0, 0), eet)
        << coordinateKyiv
        << -10.19
        << 104.43;

    QTest::newRow("10 Nov 2019 07:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(7, 0, 0), eet)
        << coordinateKyiv
        << -1.01
        << 115.7;

    QTest::newRow("10 Nov 2019 08:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(8, 0, 0), eet)
        << coordinateKyiv
        << 6.97
        << 127.55;

    QTest::newRow("10 Nov 2019 09:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(9, 0, 0), eet)
        << coordinateKyiv
        << 13.77
        << 140.35;

    QTest::newRow("10 Nov 2019 10:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(10, 0, 0), eet)
        << coordinateKyiv
        << 18.9
        << 154.29;

    QTest::newRow("10 Nov 2019 11:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(11, 0, 0), eet)
        << coordinateKyiv
        << 21.87
        << 169.25;

    QTest::newRow("10 Nov 2019 12:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(12, 0, 0), eet)
        << coordinateKyiv
        << 22.36
        << 184.72;

    QTest::newRow("10 Nov 2019 13:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(13, 0, 0), eet)
        << coordinateKyiv
        << 20.31
        << 199.94;

    QTest::newRow("10 Nov 2019 14:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(14, 0, 0), eet)
        << coordinateKyiv
        << 15.96
        << 214.31;

    QTest::newRow("10 Nov 2019 15:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(15, 0, 0), eet)
        << coordinateKyiv
        << 9.73
        << 227.53;

    QTest::newRow("10 Nov 2019 16:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(16, 0, 0), eet)
        << coordinateKyiv
        << 2.23
        << 239.69;

    QTest::newRow("10 Nov 2019 17:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(17, 0, 0), eet)
        << coordinateKyiv
        << -6.69
        << 251.11;

    QTest::newRow("10 Nov 2019 18:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(18, 0, 0), eet)
        << coordinateKyiv
        << -16.01
        << 262.25;

    QTest::newRow("10 Nov 2019 19:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(19, 0, 0), eet)
        << coordinateKyiv
        << -25.55
        << 273.71;

    QTest::newRow("10 Nov 2019 20:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(20, 0, 0), eet)
        << coordinateKyiv
        << -34.96
        << 286.26;

    QTest::newRow("10 Nov 2019 21:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(21, 0, 0), eet)
        << coordinateKyiv
        << -43.71
        << 300.98;

    QTest::newRow("10 Nov 2019 22:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(22, 0, 0), eet)
        << coordinateKyiv
        << -51.03
        << 319.26;

    QTest::newRow("10 Nov 2019 23:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(23, 0, 0), eet)
        << coordinateKyiv
        << -55.74
        << 342.08;

    QTest::newRow("10 Nov 2019 23:59:59 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(23, 59, 59), eet)
        << coordinateKyiv
        << -56.6
        << 7.91;

    QTest::newRow("10 Nov 2019 01:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(1, 0, 0), pst)
        << coordinateSeattle
        << -56.54
        << 29.9;

    QTest::newRow("10 Nov 2019 02:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(2, 0, 0), pst)
        << coordinateSeattle
        << -49.94
        << 51.32;

    QTest::newRow("10 Nov 2019 03:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(3, 0, 0), pst)
        << coordinateSeattle
        << -41.24
        << 67.68;

    QTest::newRow("10 Nov 2019 04:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(4, 0, 0), pst)
        << coordinateSeattle
        << -31.52
        << 80.82;

    QTest::newRow("10 Nov 2019 05:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(5, 0, 0), pst)
        << coordinateSeattle
        << -21.45
        << 92.24;

    QTest::newRow("10 Nov 2019 06:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(6, 0, 0), pst)
        << coordinateSeattle
        << -11.43
        << 102.96;

    QTest::newRow("10 Nov 2019 07:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(7, 0, 0), pst)
        << coordinateSeattle
        << -1.71
        << 113.7;

    QTest::newRow("10 Nov 2019 08:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(8, 0, 0), pst)
        << coordinateSeattle
        << 7.03
        << 125.04;

    QTest::newRow("10 Nov 2019 09:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(9, 0, 0), pst)
        << coordinateSeattle
        << 14.56
        << 137.41;

    QTest::newRow("10 Nov 2019 10:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(10, 0, 0), pst)
        << coordinateSeattle
        << 20.44
        << 151.11;

    QTest::newRow("10 Nov 2019 11:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(11, 0, 0), pst)
        << coordinateSeattle
        << 24.12
        << 166.07;

    QTest::newRow("10 Nov 2019 12:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(12, 0, 0), pst)
        << coordinateSeattle
        << 25.18
        << 181.79;

    QTest::newRow("10 Nov 2019 13:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(13, 0, 0), pst)
        << coordinateSeattle
        << 23.48
        << 197.4;

    QTest::newRow("10 Nov 2019 14:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(14, 0, 0), pst)
        << coordinateSeattle
        << 19.24
        << 212.09;

    QTest::newRow("10 Nov 2019 15:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(15, 0, 0), pst)
        << coordinateSeattle
        << 12.92
        << 225.46;

    QTest::newRow("10 Nov 2019 16:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(16, 0, 0), pst)
        << coordinateSeattle
        << 5.09
        << 237.55;

    QTest::newRow("10 Nov 2019 17:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(17, 0, 0), pst)
        << coordinateSeattle
        << -4.01
        << 248.68;

    QTest::newRow("10 Nov 2019 18:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(18, 0, 0), pst)
        << coordinateSeattle
        << -13.78
        << 259.34;

    QTest::newRow("10 Nov 2019 19:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(19, 0, 0), pst)
        << coordinateSeattle
        << -23.85
        << 270.12;

    QTest::newRow("10 Nov 2019 20:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(20, 0, 0), pst)
        << coordinateSeattle
        << -33.91
        << 281.8;

    QTest::newRow("10 Nov 2019 21:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(21, 0, 0), pst)
        << coordinateSeattle
        << -43.49
        << 295.49;

    QTest::newRow("10 Nov 2019 22:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(22, 0, 0), pst)
        << coordinateSeattle
        << -51.87
        << 312.83;

    QTest::newRow("10 Nov 2019 23:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(23, 0, 0), pst)
        << coordinateSeattle
        << -57.83
        << 335.61;

    QTest::newRow("10 Nov 2019 23:59:59 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(23, 59, 59), pst)
        << coordinateSeattle
        << -59.72
        << 3.19;

    QTest::newRow("10 Nov 2019 01:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(1, 0, 0), nzdt)
        << coordinateAuckland
        << -36.29
        << 181.42;

    QTest::newRow("10 Nov 2019 02:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(2, 0, 0), nzdt)
        << coordinateAuckland
        << -34.73
        << 163.87;

    QTest::newRow("10 Nov 2019 03:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(3, 0, 0), nzdt)
        << coordinateAuckland
        << -29.8
        << 147.91;

    QTest::newRow("10 Nov 2019 04:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(4, 0, 0), nzdt)
        << coordinateAuckland
        << -22.23
        << 134.31;

    QTest::newRow("10 Nov 2019 05:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(5, 0, 0), nzdt)
        << coordinateAuckland
        << -12.83
        << 122.92;

    QTest::newRow("10 Nov 2019 06:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(6, 0, 0), nzdt)
        << coordinateAuckland
        << -2.1
        << 113.15;

    QTest::newRow("10 Nov 2019 07:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(7, 0, 0), nzdt)
        << coordinateAuckland
        << 9.21
        << 104.37;

    QTest::newRow("10 Nov 2019 08:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(8, 0, 0), nzdt)
        << coordinateAuckland
        << 20.97
        << 95.92;

    QTest::newRow("10 Nov 2019 09:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(9, 0, 0), nzdt)
        << coordinateAuckland
        << 32.95
        << 87.06;

    QTest::newRow("10 Nov 2019 10:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(10, 0, 0), nzdt)
        << coordinateAuckland
        << 44.82
        << 76.68;

    QTest::newRow("10 Nov 2019 11:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(11, 0, 0), nzdt)
        << coordinateAuckland
        << 56.08
        << 62.6;

    QTest::newRow("10 Nov 2019 12:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(12, 0, 0), nzdt)
        << coordinateAuckland
        << 65.53
        << 40.11;

    QTest::newRow("10 Nov 2019 13:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(13, 0, 0), nzdt)
        << coordinateAuckland
        << 70.13
        << 3.41;

    QTest::newRow("10 Nov 2019 14:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(14, 0, 0), nzdt)
        << coordinateAuckland
        << 66.73
        << 324.78;

    QTest::newRow("10 Nov 2019 15:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(15, 0, 0), nzdt)
        << coordinateAuckland
        << 57.81
        << 300.23;

    QTest::newRow("10 Nov 2019 16:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(16, 0, 0), nzdt)
        << coordinateAuckland
        << 46.73
        << 285.18;

    QTest::newRow("10 Nov 2019 17:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(17, 0, 0), nzdt)
        << coordinateAuckland
        << 34.93
        << 274.38;

    QTest::newRow("10 Nov 2019 18:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(18, 0, 0), nzdt)
        << coordinateAuckland
        << 22.96
        << 265.35;

    QTest::newRow("10 Nov 2019 19:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(19, 0, 0), nzdt)
        << coordinateAuckland
        << 11.16
        << 256.89;

    QTest::newRow("10 Nov 2019 20:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(20, 0, 0), nzdt)
        << coordinateAuckland
        << 0.18
        << 248.19;

    QTest::newRow("10 Nov 2019 21:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(21, 0, 0), nzdt)
        << coordinateAuckland
        << -11.05
        << 238.61;

    QTest::newRow("10 Nov 2019 22:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(22, 0, 0), nzdt)
        << coordinateAuckland
        << -20.65
        << 227.52;

    QTest::newRow("10 Nov 2019 23:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(23, 0, 0), nzdt)
        << coordinateAuckland
        << -28.53
        << 214.33;

    QTest::newRow("10 Nov 2019 23:59:59 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(23, 59, 59), nzdt)
        << coordinateAuckland
        << -33.9
        << 198.76;
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

void SunPositionTest::midnight_data()
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

    QTest::newRow("10 Nov 2019 01:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(1, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 02:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(2, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 03:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(3, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 04:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(4, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 05:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(5, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 06:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(6, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 07:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(7, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 08:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(8, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 09:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(9, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 10:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(10, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 11:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(11, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 12:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(12, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 13:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(13, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 14:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(14, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 15:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(15, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 16:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(16, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 17:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(17, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 18:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(18, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 19:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(19, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 20:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(20, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 21:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(21, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 22:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(22, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 23:00:00 (GMT)")
        << QDateTime(QDate(2019, 11, 10), QTime(23, 0, 0), gmt)
        << coordinateGreenwich
        << -55.79
        << 0.0;

    QTest::newRow("10 Nov 2019 02:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(2, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 03:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(3, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 04:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(4, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 05:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(5, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 06:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(6, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 07:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(7, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 08:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(8, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 09:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(9, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 10:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(10, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 11:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(11, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 12:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(12, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 13:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(13, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 14:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(14, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 15:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(15, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 16:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(16, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 17:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(17, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 18:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(18, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 19:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(19, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 20:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(21, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 21:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(21, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 22:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(22, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("10 Nov 2019 23:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 10), QTime(23, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("11 Nov 2019 00:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 11), QTime(0, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("11 Nov 2019 01:00:00 (EET)")
        << QDateTime(QDate(2019, 11, 11), QTime(1, 0, 0), eet)
        << coordinateKyiv
        << -56.79
        << 0.0;

    QTest::newRow("09 Nov 2019 16:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 9), QTime(16, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("09 Nov 2019 17:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 9), QTime(17, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("09 Nov 2019 18:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 9), QTime(18, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("09 Nov 2019 19:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 9), QTime(19, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("09 Nov 2019 20:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 9), QTime(20, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("09 Nov 2019 21:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 9), QTime(21, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("09 Nov 2019 22:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 9), QTime(22, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("09 Nov 2019 23:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 9), QTime(23, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 00:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(0, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 01:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(1, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 02:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(2, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 03:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(3, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 04:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(4, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 05:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(5, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 06:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(6, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 07:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(7, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 08:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(8, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 09:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(9, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 10:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(10, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 11:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(11, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 12:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(12, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 13:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(13, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 14:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(14, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("10 Nov 2019 15:00:00 (PST)")
        << QDateTime(QDate(2019, 11, 10), QTime(15, 0, 0), pst)
        << coordinateSeattle
        << -59.75
        << 0.0;

    QTest::newRow("09 Nov 2019 13:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 9), QTime(13, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("09 Nov 2019 14:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 9), QTime(14, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("09 Nov 2019 15:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 9), QTime(15, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("09 Nov 2019 16:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 9), QTime(16, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("09 Nov 2019 17:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 9), QTime(17, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("09 Nov 2019 18:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 9), QTime(18, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("09 Nov 2019 19:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 9), QTime(19, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("09 Nov 2019 20:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 9), QTime(20, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("09 Nov 2019 21:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 9), QTime(21, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("09 Nov 2019 22:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 9), QTime(22, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("09 Nov 2019 23:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 9), QTime(23, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 00:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(0, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 01:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(1, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 02:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(2, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 03:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(3, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 04:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(4, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 05:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(5, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 06:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(6, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 07:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(7, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 08:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(8, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 09:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(9, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 10:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(10, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 11:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(11, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;

    QTest::newRow("10 Nov 2019 12:00:00 (NZDT)")
        << QDateTime(QDate(2019, 11, 10), QTime(12, 0, 0), nzdt)
        << coordinateAuckland
        << -36.3
        << 180.0;
}

void SunPositionTest::midnight()
{
    QFETCH(QDateTime, dateTime);
    QFETCH(QGeoCoordinate, location);
    QFETCH(qreal, elevation);
    QFETCH(qreal, azimuth);

    const SunPosition position = SunPosition::midnight(dateTime, location);

    QVERIFY(std::abs(position.elevation() - elevation) < s_precision);
    QVERIFY(std::abs(position.azimuth() - azimuth) < s_precision);
}

QTEST_MAIN(SunPositionTest)
#include "sunpositiontest.moc"
