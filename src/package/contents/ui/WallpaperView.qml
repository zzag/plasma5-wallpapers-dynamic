/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.5
import QtQuick.Controls 2.0

StackView {
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
     */
    property real blendFactor

    /*!
     * Set this property to define what happens when the source image has a
     * different size than the item.
     */
    property int fillMode

    /*!
     * The scaled width and height of the full-frame image.
     */
    property size sourceSize

    /*!
     * This property holds the wallpaper image about to be presented.
     */
    property WallpaperImage __nextItem: null

    /*!
     * This property holds the status of image loading.
     */
    readonly property int status: __nextItem ? __nextItem.status : Image.Null

    onBottomLayerChanged: Qt.callLater(reload)
    onTopLayerChanged: Qt.callLater(reload)
    onBlendFactorChanged: Qt.callLater(reblend)
    onFillModeChanged: Qt.callLater(reload)

    Component {
        id: baseImage

        WallpaperImage {
            layer.enabled: root.replaceEnter.running
            StackView.onRemoved: destroy()
        }
    }

    replaceEnter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0
            to: 1
            duration: wallpaper.configuration.TransitionDuration
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
        else if (root.currentItem.bottomLayer !== bottomLayer)
            operation = StackView.Transition;
        else if (root.currentItem.topLayer !== topLayer)
            operation = StackView.Transition;
        else
            operation = StackView.Immediate;

        if (operation === StackView.Transition)
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

        if (root.__nextItem.status == Image.Loading)
            root.__nextItem.statusChanged.connect(root.__swap);
        else
            root.__swap();
    }

    function reblend() {
        if (!root.currentItem)
            return;
        if (root.currentItem.bottomLayer !== bottomLayer)
            return;
        if (root.currentItem.topLayer !== topLayer)
            return;
        root.currentItem.blendFactor = blendFactor;
    }
}
