/*
 * Copyright (C) 2017 Canonical, Ltd.
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

#include "screenadaptormodel.h"

// mirserver
#include "screenadaptor.h"
#include "logging.h"

// Qt
#include <QGuiApplication>
#include <QScreen>

#define DEBUG_MSG qCDebug(QTMIR_SCREENS).nospace() << "Screens[" << (void*)this <<"]::" << __func__

ScreenAdaptorModel::ScreenAdaptorModel(QObject *parent)
    : qtmir::ScreenModel(parent)
{
    if (qGuiApp->platformName() != QLatin1String("mirserver")) {
        qCritical("Not using 'mirserver' QPA plugin. Using Screens may produce unknown results.");
    }

    connect(qGuiApp, &QGuiApplication::screenAdded, this, &ScreenAdaptorModel::onScreenAdded);
    connect(qGuiApp, &QGuiApplication::screenRemoved, this, &ScreenAdaptorModel::onScreenRemoved);

    Q_FOREACH(QScreen* screen, QGuiApplication::screens()) {
        m_screenList.push_back(new ScreenAdaptor(screen));
    }
    DEBUG_MSG << "(" << m_screenList << ")";
}

ScreenAdaptorModel::~ScreenAdaptorModel()
{
    qDeleteAll(m_screenList);
    m_screenList.clear();
}

void ScreenAdaptorModel::onScreenAdded(QScreen *qscreen)
{
    Q_FOREACH(auto screen, m_screenList) {
        auto adaptor = qobject_cast<ScreenAdaptor*>(screen);
        if (adaptor && adaptor->qscreen() == qscreen) return;
    }
    DEBUG_MSG << "(screen=" << qscreen << ")";

    auto adaptor(new ScreenAdaptor(qscreen));
    m_screenList.push_back(adaptor);
    Q_EMIT screenAdded(adaptor);
}

void ScreenAdaptorModel::onScreenRemoved(QScreen *qscreen)
{
    DEBUG_MSG << "(screen=" << qscreen << ")";

    int index = 0;
    QMutableVectorIterator<qtmir::Screen*> iter(m_screenList);
    while(iter.hasNext()) {
        auto adaptor = qobject_cast<ScreenAdaptor*>(iter.next());
        if (adaptor && adaptor->qscreen() == qscreen) {
            Q_EMIT screenRemoved(adaptor);

            iter.remove();
            adaptor->deleteLater();
            break;
        }
        index++;
    }
}
