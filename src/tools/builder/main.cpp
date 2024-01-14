/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>

#include <KDynamicWallpaperWriter>
#include <KLocalizedString>
#include <KSolarDynamicWallpaperMetaData>

#include "dynamicwallpapermanifest.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("kdynamicwallpaperbuilder");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineOption outputOption(QStringLiteral("output"));
    outputOption.setDescription(i18n("Write output to <file>"));
    outputOption.setValueName(QStringLiteral("file"));

    QCommandLineOption speedOption(QStringLiteral("speed"));
    speedOption.setDescription(i18n("Encoding speed, 0 - slowest, 10 - fastest"));
    speedOption.setValueName(QStringLiteral("speed"));

    QCommandLineOption maxThreadsOption(QStringLiteral("max-threads"));
    maxThreadsOption.setDescription(i18n("Maximum number of threads that can be used when encoding a wallpaper"));
    maxThreadsOption.setValueName(QStringLiteral("max-threads"));

    QCommandLineOption codecOption(QStringLiteral("codec"));
    codecOption.setDescription(i18n("Codec to use (aom|rav1e|svt)"));
    codecOption.setValueName(QStringLiteral("codec"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("json", i18n("Manifest file to use"));
    parser.addOption(outputOption);
    parser.addOption(maxThreadsOption);
    parser.addOption(speedOption);
    parser.addOption(codecOption);
    parser.process(app);

    if (parser.positionalArguments().count() != 1)
        parser.showHelp(-1);

    DynamicWallpaperManifest manifest(parser.positionalArguments().first());
    if (manifest.hasError()) {
        if (manifest.hasError())
            qWarning() << qPrintable(manifest.errorString());
        return -1;
    }

    KDynamicWallpaperWriter writer;
    writer.setImages(manifest.images());
    writer.setMetaData(manifest.metaData());

    if (parser.isSet(maxThreadsOption)) {
        bool ok;
        if (const int threadCount = parser.value(maxThreadsOption).toInt(&ok); ok) {
            writer.setMaxThreadCount(threadCount);
        } else {
            parser.showHelp(-1);
        }
    }

    if (parser.isSet(speedOption)) {
        bool ok;
        if (const int speed = parser.value(speedOption).toInt(&ok); ok) {
            writer.setSpeed(speed);
        } else {
            parser.showHelp(-1);
        }
    }

    if (parser.isSet(codecOption)) {
        if (!writer.setCodecName(parser.value(codecOption))) {
            qWarning() << qPrintable(writer.errorString());
            return -1;
        }
    }

    QString targetFileName = parser.value(outputOption);
    if (targetFileName.isEmpty())
        targetFileName = QStringLiteral("wallpaper.avif");

    if (!writer.flush(targetFileName)) {
        qWarning() << writer.errorString();
        QFile::remove(targetFileName);
        return -1;
    }

    return 0;
}
