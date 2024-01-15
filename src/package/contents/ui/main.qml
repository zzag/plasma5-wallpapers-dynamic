/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Window
import QtPositioning

import org.kde.kirigami as Kirigami
import org.kde.plasma.plasmoid
import com.github.zzag.plasma.wallpapers.dynamic

WallpaperItem {
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
        sourceSize: Qt.size(root.width, root.height)
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
