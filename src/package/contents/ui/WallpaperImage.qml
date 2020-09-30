/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.5

Item {
    id: root

    /*!
     * The image being displayed in the bottom layer.
     */
    property url bottomLayer

    /*!
     * The image being displayed in the top layer.
     */
    property url topLayer

    /*!
     * The blend factor between the bottom layer and the top layer.
     *
     * The blend factor varies between 0 and 1. 0 means that only the bottom
     * layer is visible; 1 means that only the top layer is visible.
     *
     * The default value is 0.
     */
    property real blendFactor: 0

    /*!
     * Set this property to define what happens when the source image has a
     * different size than the item.
     *
     * Defaults to \c Image.Stretch.
     */
    property int fillMode: Image.Stretch

    /*!
     * This property holds the status of image loading.
     */
    readonly property int status: {
        if (bottom.status == Image.Error || top.status == Image.Error)
            return Image.Error;
        if (bottom.status == Image.Loading || top.status == Image.Loading)
            return Image.Loading;
        if (bottom.status == Image.Ready || top.status == Image.Ready)
            return Image.Ready;
        return Image.Null;
    }

    Image {
        id: bottom
        anchors.fill: parent
        asynchronous: true
        autoTransform: true
        fillMode: root.fillMode
        source: root.bottomLayer
    }

    Image {
        id: top
        anchors.fill: parent
        asynchronous: true
        autoTransform: true
        fillMode: root.fillMode
        opacity: root.blendFactor
        source: root.topLayer
    }

    Behavior on blendFactor {
        NumberAnimation {
            duration: wallpaper.configuration.TransitionDuration
        }
    }
}
