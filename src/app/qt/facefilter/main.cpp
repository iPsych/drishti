/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Copyright (C) 2015 Ruslan Baratov
** Coypright (C) 2016 David Hirvonen
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Multimedia module.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <cassert> // assert

#include <QGuiApplication>
#include <QQuickView>
#include <QQuickItem>
#include <QtPlugin> // Q_IMPORT_PLUGIN
#include <QQmlExtensionPlugin>
#include <QtOpenGL/QGLFormat>
#include <QFile>
#include <QTextStream>
#include <QDirIterator>

#include "QMLCameraManager.h"
#include "VideoFilterRunnable.hpp"
#include "VideoFilter.hpp"
#include "InfoFilter.hpp"
#include "QTRenderGL.hpp"
#include "FrameHandler.h"
#include "QtStream.h"

#include "drishti/core/Logger.h"
#include "drishti/core/drishti_core.h"

#include "nlohmann/json.hpp" // nlohman-json

#include <iostream>

#if defined(Q_OS_OSX)
Q_IMPORT_PLUGIN(QtQuick2Plugin);
Q_IMPORT_PLUGIN(QMultimediaDeclarativeModule);
#endif

// Utilities
static void printResources();
static nlohmann::json loadJSON(spdlog::logger &logger);

#if defined(Q_OS_IOS)
extern "C" int qtmn(int argc, char** argv)
#else
int main(int argc, char **argv)
#endif
{
#ifdef Q_OS_WIN // avoid ANGLE on Windows
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
#endif
    
    // ###### Instantiate logger ########
    auto logger = drishti::core::Logger::create("facefilter");
    logger->info() << "Start";
    
    printResources();

    // JSON configuration
    auto json = loadJSON(*logger);
    
    QGuiApplication app(argc, argv);

    qmlRegisterType<VideoFilter>("facefilter.test", 1, 0, "VideoFilter");
    qmlRegisterType<InfoFilter>("facefilter.test", 1, 0, "InfoFilter");
    qmlRegisterType<QTRenderGL>("OpenGLUnderQML", 1, 0, "QTRenderGL");

#if defined(Q_OS_OSX)
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2Plugin().instance())->registerTypes("QtQuick");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QMultimediaDeclarativeModule().instance())->registerTypes("QtMultimedia");
#endif

    QQuickView view;
    view.setSource(QUrl("qrc:///main.qml"));
    view.setResizeMode( QQuickView::SizeRootObjectToView );

#if defined(Q_OS_OSX)
    // This had been tested with GLFW + ogles_gpgpu before
    //OpenGL version: 2.1 NVIDIA-10.4.2 310.41.35f01
    //GLSL version: 1.20
    QSurfaceFormat format;
    format.setVersion(2, 1);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    view.setFormat(format);

    logger->info() << "OpenGL Versions Supported: " << QGLFormat::openGLVersionFlags();
#endif

    // Default camera on iOS is not setting good parameters by default
    QQuickItem* root = view.rootObject();
    
    QObject * qmlVideoOutput = root->findChild<QObject*>("VideoOutput");
    assert(qmlVideoOutput);
    
    auto qmlCameraManager = QMLCameraManager::create(root, logger);
    (void) qmlCameraManager->configure();
    
    // ### Display the device/camera name:
    logger->info() << "device: " << qmlCameraManager->getDeviceName();
    logger->info() << "description: " << qmlCameraManager->getDescription();
    
    auto frameHandlers = FrameHandlerManager::get(&json);
    if(frameHandlers && qmlCameraManager)
    {
        frameHandlers->setOrientation(qmlCameraManager->getOrientation());
        frameHandlers->setSize(qmlCameraManager->getSize());
    }

    view.showFullScreen();

    return app.exec();
}


static void printResources()
{
    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        qDebug() << it.next();
    }
}

static nlohmann::json loadJSON(spdlog::logger &logger)
{
    nlohmann::json json;
    
    QFile inputFile(":/facefilter.json");
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        logger.error() << "Can't open file";
        return EXIT_FAILURE;
    }
    
    QTextStream in(&inputFile);
    std::stringstream stream;
    stream <<  in.readAll().toStdString();
    stream >> json;
    
    return json;
}


