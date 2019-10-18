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
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0 as QtControls
import QtQuick.Controls 2.3 as QtControls2

import org.kde.kcm 1.1 as KCM
import org.kde.kirigami 2.5 as Kirigami

import com.github.zzag.private.wallpaper 1.0

ColumnLayout {
    id: root

    property int cfg_FillMode
    property int cfg_UpdateInterval
    property string cfg_WallpaperId

    property alias cfg_Latitude: latitudeSpinbox.value
    property alias cfg_Longitude: longitudeSpinbox.value

    Kirigami.FormLayout {
        twinFormLayouts: parentLayout

        QtControls2.ComboBox {
            id: positioningComboBox
            Kirigami.FormData.label: i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Fill Mode:")
            model: [
                {
                    "label": i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Scaled and Cropped"),
                    "fillMode": Image.PreserveAspectCrop
                },
                {
                    "label": i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Scaled"),
                    "fillMode": Image.Stretch
                },
                {
                    "label": i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Scaled, Keep Proportions"),
                    "fillMode": Image.PreserveAspectFit
                },
                {
                    "label": i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Centered"),
                    "fillMode": Image.Pad
                },
                {
                    "label": i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Tiled"),
                    "fillMode": Image.Tile
                }
            ]

            textRole: "label"
            onCurrentIndexChanged: cfg_FillMode = model[currentIndex]["fillMode"]

            Component.onCompleted: {
                for (var i = 0; i < model.length; i++) {
                    if (model[i]["fillMode"] == wallpaper.configuration.FillMode) {
                        positioningComboBox.currentIndex = i;
                        break;
                    }
                }
            }
        }

        QtControls.SpinBox {
            id: latitudeSpinbox
            Kirigami.FormData.label: i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Latitude:")
            decimals: 2
            minimumValue: -90
            maximumValue: 90
        }

        QtControls.SpinBox {
            id: longitudeSpinbox
            Kirigami.FormData.label: i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Longitude:")
            decimals: 2
            minimumValue: -180
            maximumValue: 180
        }

        QtControls.SpinBox {
            id: updateIntervalSpinbox
            Kirigami.FormData.label: i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Update Every:")
            maximumValue: 59
            minimumValue: 1
            suffix: i18ndp("plasma_wallpaper_com.github.zzag.wallpaper", " min", " min", value)
            onValueChanged: cfg_UpdateInterval = value * 60000
            Component.onCompleted: value = wallpaper.configuration.UpdateInterval / 60000
        }
    }

    KCM.GridView {
        id: wallpapersGrid

        Layout.fillWidth: true
        Layout.fillHeight: true

        view.currentIndex: wallpapersModel.indexOf(cfg_WallpaperId)
        view.model: wallpapersModel
        view.delegate: KCM.GridDelegate {
            hoverEnabled: true
            text: model.name
            toolTip: model.name

            thumbnail: Image {
                anchors.fill: parent
                source: model.previewUrl
            }

            onClicked: {
                cfg_WallpaperId = model.id;
                wallpapersGrid.forceActiveFocus();
            }
        }
    }

    WallpapersModel {
        id: wallpapersModel
    }
}
