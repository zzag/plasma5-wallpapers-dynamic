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
import QtQuick.Controls 2.0

StackView {
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
     */
    property real blendFactor

    /**
     * Set this property to define what happens when the source image has a
     * different size than the item.
     */
    property int fillMode

    /**
     * The actual width and height of the loaded image.
     *
     * This property sets the actual number of pixels stored for the loaded
     * image so that large images do not use more memory than necessary.
     */
    property size sourceSize

    /**
     * This property holds the wallpaper image about to be presented.
     */
    property WallpaperImage __nextItem: null

    /**
     * This property holds the status of image loading.
     */
    readonly property int status: __nextItem ? __nextItem.status : Image.Null

    onBottomLayerChanged: Qt.callLater(reload)
    onTopLayerChanged: Qt.callLater(reload)
    onBlendFactorChanged: Qt.callLater(reblend)
    onFillModeChanged: Qt.callLater(reload)
    onSourceSizeChanged: Qt.callLater(reload)

    Component {
        id: baseImage

        WallpaperImage {
            // We have to manually destroy any item pushed onto a StackView.
            StackView.onRemoved: destroy()
        }
    }

    replaceEnter: Transition {
        OpacityAnimator {
            duration: wallpaper.configuration.TransitionDuration
            from: 0
            to: 1
        }
    }

    replaceExit: Transition {
        PauseAnimation {
            duration: wallpaper.configuration.TransitionDuration
        }
    }

    function __swap() {
        if (root.__nextItem.status == Image.Loading)
            return;

        root.__nextItem.statusChanged.disconnect(root.__swap);

        if (root.__nextItem.status == Image.Error)
            return;

        var operation;
        if (!root.currentItem)
            operation = StackView.Immediate;
        else if (root.currentItem.bottomLayer != bottomLayer)
            operation = StackView.Transition;
        else if (root.currentItem.topLayer != topLayer)
            operation = StackView.Transition;
        else
            operation = StackView.Immediate;

        if (operation == StackView.Transition)
            root.__nextItem.opacity = 0;
        else
            root.__nextItem.opacity = 1;

        root.replace(root.__nextItem, {}, operation);
    }

    function reload() {
        if (root.status == Image.Loading)
            root.__nextItem.statusChanged.disconnect(root.__swap);

        root.__nextItem = baseImage.createObject(root, {
            bottomLayer: bottomLayer,
            topLayer: topLayer,
            blendFactor: blendFactor,
            fillMode: fillMode,
            sourceSize: sourceSize
        });

        root.__nextItem.statusChanged.connect(root.__swap);
    }

    function reblend() {
        if (!root.currentItem)
            return;
        if (root.currentItem.bottomLayer != bottomLayer)
            return;
        if (root.currentItem.topLayer != topLayer)
            return;
        root.currentItem.blendFactor = blendFactor;
    }
}
