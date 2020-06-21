/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>

#include <KDynamicWallpaperMetaData>
#include <KDynamicWallpaperWriter>
#include <KLocalizedString>

#include "dynamicwallpaperdescriptionreader.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("kdynamicwallpaperbuilder");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineOption qualityOption(QStringLiteral("quality"));
    qualityOption.setDescription(i18n("Quality of the encoded images (from 0 to 100)"));
    qualityOption.setValueName(QStringLiteral("quality"));

    QCommandLineOption outputOption(QStringLiteral("output"));
    outputOption.setDescription(i18n("Write output to <file>"));
    outputOption.setValueName(QStringLiteral("file"));

    QCommandLineOption discardColorProfileOption(QStringLiteral("discard-color-profile"));
    discardColorProfileOption.setDescription(i18n("Discard embedded color profile"));

    QCommandLineOption losslessOption(QStringLiteral("lossless"));
    losslessOption.setDescription(i18n("Use lossless coding"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("json", i18n("Description file to use"));
    parser.addOption(qualityOption);
    parser.addOption(outputOption);
    parser.addOption(discardColorProfileOption);
    parser.addOption(losslessOption);
    parser.process(app);

    if (parser.positionalArguments().count() != 1)
        parser.showHelp(-1);

    KDynamicWallpaperWriter writer;
    writer.setLossless(parser.isSet(losslessOption));
    writer.setCodec(KDynamicWallpaperWriter::HEVC);

    if (parser.isSet(qualityOption)) {
        bool ok;
        const int quality = parser.value(qualityOption).toInt(&ok);
        if (ok)
            writer.setQuality(quality);
        else
            parser.showHelp(-1);
    }

    DynamicWallpaperDescriptionReader reader(parser.positionalArguments().first());

    QString targetFileName = parser.value(outputOption);
    if (targetFileName.isEmpty())
        targetFileName = QStringLiteral("wallpaper.heic");

    if (!writer.begin(targetFileName)) {
        qWarning() << writer.errorString();
        return -1;
    }

    while (!reader.atEnd()) {
        if (!reader.readNext())
            break;

        KDynamicWallpaperWriter::WriteOptions writeOptions;

        if (!parser.isSet(discardColorProfileOption))
            writeOptions |= KDynamicWallpaperWriter::PreserveColorProfile;
        if (reader.isPrimary())
            writeOptions |= KDynamicWallpaperWriter::Primary;

        if (!writer.write(reader.image(), reader.metaData(), writeOptions))
            break;
    }

    writer.end();

    if (reader.hasError() || writer.error() != KDynamicWallpaperWriter::NoError) {
        if (reader.hasError())
            qWarning() << qPrintable(reader.errorString());
        if (writer.error() != KDynamicWallpaperWriter::NoError)
            qWarning() << writer.errorString();
        QFile::remove(targetFileName);
        return -1;
    }

    return 0;
}
