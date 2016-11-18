/*
 * Copyright (C) 2013-2016 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Qt
#include <QObject>
#include <QDebug>

// local
#include "qmirserver.h"
#include "qmirserver_p.h"


QMirServer::QMirServer(int &argc, char **argv, QObject *parent)
    : QObject(parent)
    , d_ptr(new QMirServerPrivate(argc, argv))
{
    Q_D(QMirServer);

    d->serverThread = new MirServerThread(d);

    connect(d->serverThread, &MirServerThread::stopped, this, &QMirServer::stopped);
}

QMirServer::~QMirServer()
{
    stop();
}

QSharedPointer<QMirServer> QMirServer::create(int &argc, char **argv)
{
    static QWeakPointer<QMirServer> server;
    if (server.isNull()) {
        QSharedPointer<QMirServer> newServer(new QMirServer(argc, argv));
        server = newServer.toWeakRef();
        return newServer;
    }
    return server.toStrongRef();
}

void QMirServer::start()
{
    Q_D(QMirServer);

    if (d->serverThread->isRunning()) {
        return;
    }

    d->serverThread->start(QThread::TimeCriticalPriority);

    if (!d->serverThread->waitForMirStartup())
    {
        qFatal("ERROR: QMirServer - Mir failed to start"); // will core dump
    }
    Q_EMIT started();
}

void QMirServer::stop()
{
    Q_D(QMirServer);

    if (d->serverThread->isRunning()) {
        d->stop();
        if (!d->serverThread->wait(10000)) {
            // do something to indicate fail during shutdown
            qCritical() << "ERROR: QMirServer - Mir failed to shut down correctly, terminating it";
            d->serverThread->terminate();
        }
    }
}

bool QMirServer::isRunning() const
{
    Q_D(const QMirServer);
    return d->serverThread->isRunning();
}

QSharedPointer<ScreensModel> QMirServer::screensModel() const
{
    Q_D(const QMirServer);
    return d->screensModel;
}

QPlatformOpenGLContext *QMirServer::createPlatformOpenGLContext(QOpenGLContext *context) const
{
    Q_D(const QMirServer);
    return d->createPlatformOpenGLContext(context);
}

void *QMirServer::nativeResourceForIntegration(const QByteArray &resource) const
{
    Q_D(const QMirServer);
    void *result = nullptr;

    if (resource == "SessionAuthorizer")
        result = d->theApplicationAuthorizer().get();
    else if (resource == "AppNotifier")
        result = d->appNotifier();
    else if (resource == "PromptSessionListener")
        result = d->promptSessionListener();
    else if (resource == "WindowController")
        result = d->windowController();
    else if (resource == "WindowModelNotifier")
        result = d->windowModelNotifier();
    else if (resource == "ScreensController")
        result = d->screensController.data();

    return result;
}

std::shared_ptr<qtmir::PromptSessionManager> QMirServer::thePromptSessionManager() const
{
    Q_D(const QMirServer);
    return d->promptSessionManager();
}

void QMirServer::wrapDisplayConfigurationPolicy(miral::BasicSetDisplayConfigurationPolicy const& setDisplayConfigurationPolicy)
{
    qDebug() << "OVERRIDE wrapDisplayConfigurationPolicy";

    Q_D(QMirServer);
    d->m_displayConfigurationPolicy = setDisplayConfigurationPolicy;
}

void QMirServer::overrideSessionAuthorizer(miral::BasicSetApplicationAuthorizer const& setApplicationAuthorizer)
{
    qDebug() << "OVERRIDE overrideSessionAuthorizer";

    Q_D(QMirServer);
    d->m_sessionAuthorizer = setApplicationAuthorizer;
}
