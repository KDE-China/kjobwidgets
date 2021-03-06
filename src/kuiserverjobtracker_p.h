/* This file is part of the KDE libraries
    Copyright (C) 2006-2008 Rafael Fernández López <ereslibre@kde.org>
                            Kevin Ottens <ervin@kde.org>
    Copyright (C) 2000 Matej Koss <koss@miesto.sk>
                       David Faure <faure@kde.org>
                       Stephan Kulow <coolo@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KUISERVERJOBTRACKER_P_H
#define KUISERVERJOBTRACKER_P_H

#include <QMap>

#include "jobviewserverinterface.h"

class Q_DECL_HIDDEN KSharedUiServerProxy : public QObject
{
    Q_OBJECT

public:
    KSharedUiServerProxy();
    ~KSharedUiServerProxy();

    org::kde::JobViewServer &uiserver();

private:
    org::kde::JobViewServer m_uiserver;
};

#endif
