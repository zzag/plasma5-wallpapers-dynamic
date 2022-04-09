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

#include <KSolarDynamicWallpaperMetaData>
#include <KDynamicWallpaperWriter>
#include <KLocalizedString>

#include "dynamicwallpaperdescription.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("kdynamicwallpaperbuilder");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineOption outputOption(QStringLiteral("output"));
    outputOption.setDescription(i18n("Write output to <file>"));
    outputOption.setValueName(QStringLiteral("file"));

    QCommandLineOption maxThreadsOption(QStringLiteral("max-threads"));
    maxThreadsOption.setDescription(i18n("Maximum number of threads that can be used during encoding wallpaper"));
    maxThreadsOption.setValueName(QStringLiteral("max-threads"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("json", i18n("Description file to use"));
    parser.addOption(outputOption);
    parser.addOption(maxThreadsOption);
    parser.process(app);

    if (parser.positionalArguments().count() != 1)
        parser.showHelp(-1);

    DynamicWallpaperDescription description(parser.positionalArguments().first());
    if (description.hasError()) {
        if (description.hasError())
            qWarning() << qPrintable(description.errorString());
        return -1;
    }

    KDynamicWallpaperWriter writer;
    writer.setImages(description.images());
    writer.setMetaData(description.metaData());

    if (parser.isSet(maxThreadsOption)) {
        bool ok;
        if (const int threadCount = parser.value(maxThreadsOption).toInt(&ok); ok) {
            writer.setMaxThreadCount(threadCount);
        } else {
            parser.showHelp(-1);
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
