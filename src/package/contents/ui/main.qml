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

import QtQuick 2.1

import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import org.kde.plasma.core 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import com.github.zzag.private.wallpaper 1.1

Item {
    id: root

    readonly property int fillMode: wallpaper.configuration.FillMode
    readonly property double latitude: wallpaper.configuration.Latitude
    readonly property double longitude: wallpaper.configuration.Longitude
    readonly property size sourceSize: Qt.size(root.width * Screen.devicePixelRatio, root.height * Screen.devicePixelRatio)
    readonly property int updateInterval: wallpaper.configuration.UpdateInterval
    readonly property string wallpaperId: wallpaper.configuration.WallpaperId

    onFillModeChanged: wallpaperView.fillMode = fillMode
    onLatitudeChanged: dynamicWallpaper.latitude = latitude
    onLongitudeChanged: dynamicWallpaper.longitude = longitude
    onSourceSizeChanged: wallpaperView.sourceSize = sourceSize
    onUpdateIntervalChanged: timer.interval = updateInterval
    onWallpaperIdChanged: dynamicWallpaper.wallpaperId = wallpaperId

    WallpaperView {
        id: wallpaperView
        anchors.fill: parent
        blendFactor: dynamicWallpaper.blendFactor
        bottomLayer: dynamicWallpaper.bottomLayer
        fillMode: root.fillMode
        sourceSize: root.sourceSize
        topLayer: dynamicWallpaper.topLayer
        visible: dynamicWallpaper.status == DynamicWallpaper.Ok
    }

    Rectangle {
        anchors.fill: parent
        color: "#707070"
        visible: dynamicWallpaper.status == DynamicWallpaper.Error

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            color: "white"
            font.pointSize: 24
            horizontalAlignment: Text.AlignHCenter
            text: dynamicWallpaper.error
            wrapMode: Text.Wrap
        }
    }

    DynamicWallpaper {
        id: dynamicWallpaper
        latitude: latitude
        longitude: longitude
        wallpaperId: wallpaperId
    }

    DateTimeWatcher {
        active: dynamicWallpaper.status == DynamicWallpaper.Ok
        onDateTimeChanged: dynamicWallpaper.update()
    }

    Timer {
        id: timer
        interval: updateInterval
        repeat: true
        running: dynamicWallpaper.status == DynamicWallpaper.Ok
        onTriggered: dynamicWallpaper.update()
    }
}
