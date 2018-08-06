import QtQuick 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0 as QtControls

// for "units"
import org.kde.plasma.core 2.0 as PlasmaCore


ColumnLayout {
    id: root
    property int cfg_FillMode
    property alias cfg_Latitude: latitudeSpinbox.value
    property alias cfg_Longitude: longitudeSpinbox.value
    property alias cfg_UpdateInterval: updateIntervalSpinbox.value
    property alias cfg_DayPhotosFolder: dayPhotosFolderTextField.text
    property alias cfg_NightPhotosFolder: nightPhotosFolderTextField.text

    Row {
        spacing: units.largeSpacing / 2

        QtControls.Label {
            anchors.verticalCenter: positioningComboBox.verticalCenter
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            text: "Positioning:"
        }

        QtControls.ComboBox {
            id: positioningComboBox
            property int textLength: 24
            width: theme.mSize(theme.defaultFont).width * textLength
            model: [
                {
                    "label": "Scaled and Cropped",
                    "fillMode": Image.PreserveAspectCrop
                },
                {
                    "label": "Scaled",
                    "fillMode": Image.Stretch
                },
                {
                    "label": "Scaled, Keep Proportions",
                    "fillMode": Image.PreserveAspectFit
                },
                {
                    "label": "Centered",
                    "fillMode": Image.Pad
                },
                {
                    "label": "Tiled",
                    "fillMode": Image.Tile
                }
            ]

            textRole: "label"
            onCurrentIndexChanged: cfg_FillMode = model[currentIndex]["fillMode"]
            Component.onCompleted: setMethod();

            function setMethod() {
                for (var i = 0; i < model.length; i++) {
                    if (model[i]["fillMode"] == wallpaper.configuration.FillMode) {
                        positioningComboBox.currentIndex = i;
                    }
                }
            }
        }
    }

    Row {
        spacing: units.largeSpacing / 2

        QtControls.Label {
            anchors.verticalCenter: latitudeSpinbox.verticalCenter
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            text: "Latitude:"
        }

        QtControls.SpinBox {
            id: latitudeSpinbox
            property int textLength: 24
            width: theme.mSize(theme.defaultFont).width * textLength
            decimals: 2
            minimumValue: -360
            maximumValue: 360
        }
    }

    Row {
        spacing: units.largeSpacing / 2

        QtControls.Label {
            anchors.verticalCenter: longitudeSpinbox.verticalCenter
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            text: "Longitude:"
        }

        QtControls.SpinBox {
            id: longitudeSpinbox
            property int textLength: 24
            width: theme.mSize(theme.defaultFont).width * textLength
            decimals: 2
            minimumValue: -360
            maximumValue: 360
        }
    }

    Row {
        spacing: units.largeSpacing / 2

        QtControls.Label {
            anchors.verticalCenter: updateIntervalSpinbox.verticalCenter
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            text: "Update Interval:"
        }

        QtControls.SpinBox {
            id: updateIntervalSpinbox
            property int textLength: 24
            width: theme.mSize(theme.defaultFont).width * textLength
            minimumValue: 1
            maximumValue: 3600
            stepSize: 5
            suffix: " seconds"
        }
    }

    Row {
        spacing: units.largeSpacing / 2

        QtControls.Label {
            anchors.verticalCenter: dayPhotosFolderTextField.verticalCenter
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            text: "Day Photos Folder:"
        }

        QtControls.TextField {
            id: dayPhotosFolderTextField
            property int textLength: 24
            width: theme.mSize(theme.defaultFont).width * textLength
        }

        QtControls.Button {
            text: "Browse"
            anchors.verticalCenter: dayPhotosFolderTextField.verticalCenter
            onClicked: dayPhotosFileDialog.open()
        }

        FileDialog {
            id: dayPhotosFileDialog
            title: "Choose a folder"
            selectExisting: true
            selectMultiple: false
            selectFolder: true
            onAccepted: dayPhotosFolderTextField.text = dayPhotosFileDialog.fileUrl;
        }
    }

    Row {
        spacing: units.largeSpacing / 2

        QtControls.Label {
            anchors.verticalCenter: nightPhotosFolderTextField.verticalCenter
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            text: "Night Photos Folder:"
        }

        QtControls.TextField {
            id: nightPhotosFolderTextField
            property int textLength: 24
            width: theme.mSize(theme.defaultFont).width * textLength
        }

        QtControls.Button {
            text: "Browse"
            anchors.verticalCenter: nightPhotosFolderTextField.verticalCenter
            onClicked: nightPhotosFileDialog.open()
        }

        FileDialog {
            id: nightPhotosFileDialog
            title: "Choose a folder"
            selectExisting: true
            selectMultiple: false
            selectFolder: true
            onAccepted: nightPhotosFolderTextField.text = nightPhotosFileDialog.fileUrl;
        }
    }

    Item { // tighten layout
        Layout.fillHeight: true
    }
}
