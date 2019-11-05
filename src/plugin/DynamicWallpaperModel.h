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

#pragma once

// Own
#include "SunPath.h"

// Qt
#include <QDateTime>
#include <QGeoCoordinate>
#include <QUrl>
#include <QVector>

// std
#include <memory>

class DynamicWallpaperPackage;

class DynamicWallpaperModel
{
public:
    explicit DynamicWallpaperModel(std::shared_ptr<DynamicWallpaperPackage> wallpaper);
    virtual ~DynamicWallpaperModel();

    /**
     * Returns whether the model is no longer actual and must be destroyed.
     *
     * Default implementation returns @c false.
     */
    virtual bool isExpired() const;

    /**
     * Returns whether the dynamic wallpaper model is valid.
     *
     * Default implementation returns @c true.
     */
    virtual bool isValid() const;

    /**
     * Returns the text with the description why the model is invalid.
     *
     * Default implementation returns empty string.
     */
    virtual QString errorText() const;

    /**
     * Returns the path to image that has to be displayed in the bottom layer.
     */
    QUrl bottomLayer() const;

    /**
     * Returns the path to image that has to be displayed in the top layer.
     */
    QUrl topLayer() const;

    /**
     * Returns the blend factor between the bottom and the top layer.
     *
     * A value of 0.0 is equivalent to displaying only the bottom layer.
     *
     * A value of 1.0 is equivalent to displaying only the top layer.
     */
    qreal blendFactor() const;

    /**
     * Updates the internal state of the dynamic wallpaper model.
     */
    virtual void update() = 0;

    /**
     * Returns the dynamic wallpaper package associated with this model.
     */
    DynamicWallpaperPackage *wallpaper() const;

protected:
    struct Knot
    {
        bool operator<(const Knot &other) const { return time < other.time; }
        bool operator<=(const Knot &other) const { return time <= other.time; }

        qreal time;
        QUrl url;
    };

    Knot currentBottomKnot() const;
    Knot currentTopKnot() const;

    QVector<Knot> m_knots;
    qreal m_time = 0;

private:
    std::shared_ptr<DynamicWallpaperPackage> m_wallpaper;
};

class SolarDynamicWallpaperModel : public DynamicWallpaperModel
{
public:
    SolarDynamicWallpaperModel(std::shared_ptr<DynamicWallpaperPackage> wallpaper, const QGeoCoordinate &location);

    bool isExpired() const override;
    bool isValid() const override;
    QString errorText() const override;
    void update() override;

private:
    SunPath m_sunPath;
    QDateTime m_dateTime;
    QGeoCoordinate m_location;
};

class TimedDynamicWallpaperModel : public DynamicWallpaperModel
{
public:
    explicit TimedDynamicWallpaperModel(std::shared_ptr<DynamicWallpaperPackage> wallpaper);

    void update() override;
};
