import QtQuick 2.7
import QtQuick.Window 2.2
import Qt.labs.folderlistmodel 2.11

import "./utils.js" as Utils

Item {
    id: root

    readonly property int updateInterval: wallpaper.configuration.UpdateInterval * 1000
    readonly property double latitude: wallpaper.configuration.Latitude
    readonly property double longitude: wallpaper.configuration.Longitude
    readonly property string dayPhotosFolder: wallpaper.configuration.DayPhotosFolder
    readonly property string nightPhotosFolder: wallpaper.configuration.NightPhotosFolder
    readonly property int fillMode: wallpaper.configuration.FillMode
    readonly property size sourceSize: Qt.size(root.width * Screen.devicePixelRatio, root.height * Screen.devicePixelRatio)

    onLatitudeChanged: update();
    onLongitudeChanged: update();

    onFillModeChanged: {
        fromImage.fillMode = fillMode;
        toImage.fillMode = fillMode;
    }

    onSourceSizeChanged: {
        fromImage.sourceSize = sourceSize;
        toImage.sourceSize = sourceSize;
    }

    FolderListModel {
        id: dayPhotos
        folder: dayPhotosFolder
        nameFilters: ["*.jpeg", "*.jpg", "*.png"]
        showDirs: false
        sortField: FolderListModel.Name
        onStatusChanged: update();
    }

    FolderListModel {
        id: nightPhotos
        folder: nightPhotosFolder
        nameFilters: ["*.jpeg", "*.jpg", "*.png"]
        showDirs: false
        sortField: FolderListModel.Name
        onStatusChanged: update();
    }

    Image {
        id: fromImage
        anchors.fill: parent
        fillMode: fillMode
        z: 0
    }

    Image {
        id: toImage
        anchors.fill: parent
        fillMode: fillMode
        opacity: 0
        z: 1
    }

    Timer {
        id: timer
        interval: updateInterval
        running: true
        repeat: true
        onTriggered: update()
    }

    function updateDay(now, timeInfo) {
        var progress = (now.getTime() - timeInfo.sunrise.getTime()) /
            (timeInfo.dusk.getTime() - timeInfo.sunrise.getTime());
        var idx = progress * dayPhotos.count;

        var fromIdx = Math.floor(idx);
        var fromSource = dayPhotos.get(fromIdx, "fileURL");

        var toSource = (fromIdx + 1 < dayPhotos.count)
            ? dayPhotos.get(fromIdx + 1, "fileURL")
            : nightPhotos.get(0, "fileURL");

        fromImage.source = fromSource;
        toImage.source = toSource;
        toImage.opacity = idx - Math.floor(idx);
    }

    function updateNight(now, timeInfo) {
        var fromSource;
        var toSource;
        var progress;

        if (now.getTime() < timeInfo.dawn.getTime()) {
            var lastNightPhoto = nightPhotos.get(nightPhotos.count - 1, "fileURL");
            fromSource = lastNightPhoto;
            toSource = lastNightPhoto;
            progress = 1.0;
        }

        // Dawn - sunrise transition.
        else if (now.getTime() < timeInfo.sunrise.getTime()) {
            fromSource = nightPhotos.get(nightPhotos.count - 1, "fileURL");
            toSource = dayPhotos.get(0, "fileURL");
            progress = (now.getTime() - timeInfo.dawn.getTime()) /
                (timeInfo.sunrise.getTime() - timeInfo.dawn.getTime());
        }

        else if (timeInfo.dusk.getTime() <= now.getTime()) {
            var startOfDay = new Date(timeInfo.dusk);
            startOfDay.setHours(0, 0, 0, 0);
            var nightLength = 86400000 - (timeInfo.dusk.getTime() - startOfDay.getTime());
            var idx = nightPhotos.count * (now.getTime() - timeInfo.dusk.getTime()) / nightLength;

            var fromIdx = Math.floor(idx);
            fromSource = nightPhotos.get(fromIdx, "fileURL");

            if (nightPhotos.count <= fromIdx + 1) {
                toSource = fromSource;
                progress = 1.0;
            } else {
                toSource = nightPhotos.get(fromIdx + 1, "fileURL");
                progress = idx - Math.floor(idx);
            }
        }

        else {
            var lastNightPhoto = nightPhotos.get(nightPhotos.count - 1, "fileURL");
            fromSource = lastNightPhoto;
            toSource = lastNightPhoto;
            progress = 1.0;
        }

        fromImage.source = fromSource;
        toImage.source = toSource;
        toImage.opacity = progress;
    }

    function update() {
        if (dayPhotos.status != FolderListModel.Ready) {
            return;
        }

        if (nightPhotos.status != FolderListModel.Ready) {
            return;
        }

        // If a folder with day photos or night photos doesn't have
        // images, do not show anything.
        if (dayPhotos.count == 0 || nightPhotos.count == 0) {
            fromImage.source = "";
            toImage.source = "";
            toImage.opacity = 0;
            return;
        }

        var now = new Date();
        var timeInfo = Utils.calcSunriseSunset(now, latitude, longitude);

        if (timeInfo.sunrise.getTime() <= now.getTime()
                && now.getTime() < timeInfo.dusk.getTime()) {
            updateDay(now, timeInfo);
        } else {
            updateNight(now, timeInfo);
        }
    }
}
