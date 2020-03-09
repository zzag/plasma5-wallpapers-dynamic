/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.5
import QtQuick.Window 2.2
import QtPositioning 5.12

import org.kde.kirigami 2.10 as Kirigami
import com.github.zzag.plasma.wallpapers.dynamic 1.0

Item {
    id: root

    PositionSource {
        id: automaticLocationProvider
        active: wallpaper.configuration.AutoDetectLocation
    }

    Location {
        id: manualLocationProvider
        coordinate {
            latitude: wallpaper.configuration.ManualLatitude
            longitude: wallpaper.configuration.ManualLongitude
        }
    }

    WallpaperView {
        id: view
        anchors.fill: parent
        blendFactor: handler.blendFactor
        bottomLayer: handler.bottomLayer
        fillMode: wallpaper.configuration.FillMode
        topLayer: handler.topLayer
        visible: handler.status == DynamicWallpaperHandler.Ready
        onStatusChanged: if (status != Image.Loading) {
            wallpaper.loading = false;
        }
    }

    Rectangle {
        anchors.fill: parent
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        color: Kirigami.Theme.backgroundColor
        visible: handler.status == DynamicWallpaperHandler.Error

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 24
            horizontalAlignment: Text.AlignHCenter
            text: handler.errorString
            wrapMode: Text.Wrap
        }
    }

    DynamicWallpaperHandler {
        id: handler
        location: {
            if (wallpaper.configuration.AutoDetectLocation)
                return automaticLocationProvider.position.coordinate;
            return manualLocationProvider.coordinate;
        }
        source: wallpaper.configuration.Image
        onStatusChanged: if (status == DynamicWallpaperHandler.Error) {
            wallpaper.loading = false;
        }
    }

    SystemClockMonitor {
        active: handler.status == DynamicWallpaperHandler.Ready
        onSystemClockChanged: handler.scheduleUpdate()
    }

    Timer {
        interval: wallpaper.configuration.UpdateInterval
        repeat: true
        running: handler.status == DynamicWallpaperHandler.Ready
        onTriggered: handler.scheduleUpdate()
    }

    Component.onCompleted: {
        wallpaper.loading = handler.status == DynamicWallpaperHandler.Ready;
    }
}
