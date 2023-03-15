/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtCore
import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls as QtControls2
import QtPositioning

import org.kde.kcm as KCM
import org.kde.kirigami 2.10 as Kirigami
import com.github.zzag.plasma.wallpapers.dynamic

ColumnLayout {
    id: root

    property int cfg_FillMode
    property string cfg_Image
    property alias cfg_UpdateInterval: updateIntervalSpinBox.value
    property alias cfg_AutoDetectLocation: autoDetectLocationCheckBox.checked
    property alias cfg_ManualLatitude: latitudeSpinBox.value
    property alias cfg_ManualLongitude: longitudeSpinBox.value

    function saveConfig() {
        wallpapersModel.purge();
    }

    Kirigami.FormLayout {
        twinFormLayouts: parentLayout

        QtControls2.ComboBox {
            id: positioningComboBox
            Kirigami.FormData.label: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Fill Mode:")
            textRole: "text"
            model: [
                {
                    "text": i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Scaled and Cropped"),
                    "value": Image.PreserveAspectCrop
                },
                {
                    "text": i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Scaled"),
                    "value": Image.Stretch
                },
                {
                    "text": i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Scaled, Keep Proportions"),
                    "value": Image.PreserveAspectFit
                },
                {
                    "text": i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Centered"),
                    "value": Image.Pad
                },
                {
                    "text": i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Tiled"),
                    "value": Image.Tile
                }
            ]

            onActivated: cfg_FillMode = model[currentIndex]["value"]

            Component.onCompleted: {
                for (var i = 0; i < model.length; i++) {
                    if (model[i]["value"] === wallpaper.configuration.FillMode) {
                        positioningComboBox.currentIndex = i;
                        break;
                    }
                }
            }
        }

        QtControls2.CheckBox {
            id: autoDetectLocationCheckBox
            text: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Automatically detect location")
        }

        DecimalSpinBox {
            Kirigami.FormData.label: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Latitude:")
            enabled: !autoDetectLocationCheckBox.checked
            visible: autoDetectLocationCheckBox.checked
            decimals: 2
            from: -90
            to: 90
            value: automaticLocationProvider.position.coordinate.latitude
        }

        DecimalSpinBox {
            Kirigami.FormData.label: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Longitude:")
            enabled: !autoDetectLocationCheckBox.checked
            visible: autoDetectLocationCheckBox.checked
            decimals: 2
            from: -180
            to: 180
            value: automaticLocationProvider.position.coordinate.longitude
        }

        DecimalSpinBox {
            id: latitudeSpinBox
            Kirigami.FormData.label: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Latitude:")
            decimals: 2
            from: -90
            to: 90
            visible: !autoDetectLocationCheckBox.checked
        }

        DecimalSpinBox {
            id: longitudeSpinBox
            Kirigami.FormData.label: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Longitude:")
            decimals: 2
            from: -180
            to: 180
            visible: !autoDetectLocationCheckBox.checked
        }

        QtControls2.SpinBox {
            id: updateIntervalSpinBox
            Kirigami.FormData.label: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Update Every:")
            to: minutesToMilliseconds(360)
            from: minutesToMilliseconds(1)
            stepSize: minutesToMilliseconds(1)

            function millisecondsToMinutes(milliseconds) {
                return milliseconds / 60000;
            }
            function minutesToMilliseconds(minutes) {
                return minutes * 60000;
            }

            valueFromText: function(text, locale) {
                return minutesToMilliseconds(Number.fromLocaleString(locale, text));
            }
            textFromValue: function(value, locale) {
                const minutes = millisecondsToMinutes(value);
                return minutes.toLocaleString(locale, 'f', 0) +
                        i18ndp("plasma_wallpaper_com.github.zzag.dynamic", " minute", " minutes", minutes);
            }
        }
    }

    Kirigami.InlineMessage {
        id: errorContainer
        Layout.fillWidth: true
        showCloseButton: true
        type: Kirigami.MessageType.Error
        visible: false
    }

    KCM.GridView {
        id: wallpapersGrid
        Layout.fillWidth: true
        Layout.fillHeight: true

        function resetCurrentIndex() {
            view.currentIndex = wallpapersModel.find(cfg_Image);
        }

        view.model: wallpapersModel
        view.delegate: KCM.GridDelegate {
            hoverEnabled: true
            opacity: model.zombie ? 0.5 : 1
            text: model.name
            toolTip: {
                if (model.author && model.license)
                    return i18ndc("plasma_wallpaper_com.github.zzag.dynamic", "<image> by <author> (<license>)", "By %1 (%2)", model.author, model.license);
                if (model.license)
                    return i18ndc("plasma_wallpaper_com.github.zzag.dynamic", "<image> (<license>)", "%1 (%2)", model.name, model.license);
                return model.name;
            }
            actions: [
                Kirigami.Action {
                    icon.name: "document-open-folder"
                    tooltip: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Open Containing Folder")
                    onTriggered: Qt.openUrlExternally(model.folder)
                },
                Kirigami.Action {
                    icon.name: "edit-undo"
                    tooltip: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Restore Wallpaper")
                    visible: model.zombie
                    onTriggered: wallpapersModel.unscheduleRemove(wallpapersModel.modelIndex(index))
                },
                Kirigami.Action {
                    icon.name: "edit-delete"
                    tooltip: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Remove Wallpaper")
                    visible: !model.zombie && model.removable
                    onTriggered: wallpapersModel.scheduleRemove(wallpapersModel.modelIndex(index))
                }
            ]
            thumbnail: Image {
                anchors.fill: parent
                fillMode: cfg_FillMode
                source: model.preview
            }
            onClicked: {
                cfg_Image = model.image;
                wallpapersGrid.forceActiveFocus();
            }
        }

        Connections {
            target: wallpapersModel
            onRowsInserted: Qt.callLater(wallpapersGrid.resetCurrentIndex)
            onRowsRemoved: Qt.callLater(wallpapersGrid.resetCurrentIndex)
        }
        Connections {
            target: root
            onCfg_ImageChanged: Qt.callLater(wallpapersGrid.resetCurrentIndex)
        }
    }

    Loader {
        id: wallpaperDialogLoader
        active: false
        sourceComponent: FileDialog {
            title: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Open Wallpaper")
            currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
            nameFilters: [i18nd("plasma_wallpaper_com.github.zzag.dynamic", "AVIF Image Files (*.avif)")]
            onAccepted: {
                wallpapersModel.add(selectedFile);
                wallpaperDialogLoader.active = false;
            }
            onRejected: {
                wallpaperDialogLoader.active = false;
            }
            Component.onCompleted: open()
        }
    }

    RowLayout {
        Layout.alignment: Qt.AlignRight

        QtControls2.Button {
            icon.name: "list-add"
            text: i18nd("plasma_wallpaper_com.github.zzag.dynamic", "Add Wallpaper...")
            onClicked: wallpaperDialogLoader.active = true
        }
    }

    PositionSource {
        id: automaticLocationProvider
        active: autoDetectLocationCheckBox.checked
    }

    DynamicWallpaperModel {
        id: wallpapersModel
    }

    function showErrorMessage(message) {
        errorContainer.text = message;
        errorContainer.visible = true;
    }

    Component.onCompleted: {
        wallpapersModel.errorOccurred.connect(showErrorMessage);
        wallpapersModel.reload()
    }
}
