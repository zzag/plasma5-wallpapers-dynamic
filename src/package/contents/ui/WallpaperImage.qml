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

import QtQuick 2.5

Item {
    id: root

    /**
     * The image being displayed in the bottom layer.
     */
    property url bottomLayer

    /**
     * The image being displayed in the top layer.
     */
    property url topLayer

    /**
     * The blend factor between the bottom layer and the top layer.
     *
     * The blend factor varies between 0 and 1. 0 means that only the bottom
     * layer is visible; 1 means that only the top layer is visible.
     *
     * The default value is 0.
     */
    property real blendFactor: 0

    /**
     * Set this property to define what happens when the source image has a
     * different size than the item.
     *
     * Defaults to @c Image.Stretch.
     */
    property int fillMode: Image.Stretch

    /**
     * The actual width and height of the loaded image.
     *
     * This property sets the actual number of pixels stored for the loaded
     * image so that large images do not use more memory than necessary.
     */
    property size sourceSize: undefined

    /**
     * This property holds the status of image loading.
     */
    readonly property int status: {
        if (bottom.status == Image.Error || top.status == Image.Error)
            return Image.Error;
        if (bottom.status == Image.Loading || top.status == Image.Loading)
            return Image.Loading;
        if (bottom.status == Image.Ready && top.status == Image.Ready)
            return Image.Ready;
        return Image.Null;
    }

    Image {
        id: bottom
        anchors.fill: parent
        asynchronous: true
        autoTransform: true
        cache: false
        fillMode: root.fillMode
        source: root.bottomLayer
        sourceSize: root.sourceSize
    }

    Image {
        id: top
        anchors.fill: parent
        asynchronous: true
        autoTransform: true
        cache: false
        fillMode: root.fillMode
        opacity: root.blendFactor
        source: root.topLayer
        sourceSize: root.sourceSize
    }

    Behavior on blendFactor {
        NumberAnimation { duration: 300 }
    }
}
