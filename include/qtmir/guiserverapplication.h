/*
 * Copyright (C) 2016 Canonical, Ltd.
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

#ifndef QTMIR_GUISERVERAPPLICATION_H
#define QTMIR_GUISERVERAPPLICATION_H

#include <QtGui/QGuiApplication>

// std
#include <functional>

class QMirServer;

namespace qtmir {

class GuiServerApplication : public QGuiApplication
{
    Q_OBJECT
public:
    explicit GuiServerApplication(int &argc,
                                  char **argv,
                                  std::initializer_list<std::function<void(QMirServer&)>> options);
    ~GuiServerApplication();
};

} // namespace qtmir

#endif // QTMIR_GUISERVERAPPLICATION_H
