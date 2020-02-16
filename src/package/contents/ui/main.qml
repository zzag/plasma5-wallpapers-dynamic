/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.1
import QtQuick.Window 2.2
import QtPositioning 5.9

import org.kde.kirigami 2.5 as Kirigami

import com.github.zzag.private.wallpaper 1.3

Item {
    id: root

    PositionSource {
        id: automaticLocationProvider
        active: wallpaper.configuration.AutoDetectLocation
    }

    Location {
        id: manualLocationProvider
        coordinate {
            latitude: wallpaper.configuration.Latitude
            longitude: wallpaper.configuration.Longitude
        }
    }

    WallpaperView {
        id: wallpaperView
        anchors.fill: parent
        blendFactor: dynamicWallpaperHandler.blendFactor
        bottomLayer: dynamicWallpaperHandler.bottomLayer
        fillMode: wallpaper.configuration.FillMode
        sourceSize: Qt.size(root.width * Screen.devicePixelRatio, root.height * Screen.devicePixelRatio)
        topLayer: dynamicWallpaperHandler.topLayer
        visible: dynamicWallpaperHandler.status == DynamicWallpaperHandler.Ok
        onStatusChanged: if (status != Image.Loading) {
            wallpaper.loading = false;
        }
    }

    Rectangle {
        anchors.fill: parent
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        color: Kirigami.Theme.backgroundColor
        visible: dynamicWallpaperHandler.status == DynamicWallpaperHandler.Error

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 24
            horizontalAlignment: Text.AlignHCenter
            text: dynamicWallpaperHandler.error
            wrapMode: Text.Wrap
        }
    }

    DynamicWallpaperHandler {
        id: dynamicWallpaperHandler
        location: {
            if (wallpaper.configuration.AutoDetectLocation)
                return automaticLocationProvider.position.coordinate;
            return manualLocationProvider.coordinate;
        }
        wallpaperId: wallpaper.configuration.WallpaperId
        onStatusChanged: if (status != DynamicWallpaperHandler.Ok) {
            wallpaper.loading = false;
        }
    }

    ClockSkewNotifier {
        active: dynamicWallpaperHandler.status == DynamicWallpaperHandler.Ok
        onClockSkewed: dynamicWallpaperHandler.update()
    }

    Timer {
        id: timer
        interval: wallpaper.configuration.UpdateInterval
        repeat: true
        running: dynamicWallpaperHandler.status == DynamicWallpaperHandler.Ok
        onTriggered: dynamicWallpaperHandler.update()
    }

    Component.onCompleted: {
        // Delay KSplash until the dynamic wallpaper is loaded.
        wallpaper.loading = true;
    }
}
