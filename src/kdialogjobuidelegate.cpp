/* This file is part of the KDE libraries
    Copyright (C) 2000 Stephan Kulow <coolo@kde.org>
                       David Faure <faure@kde.org>
    Copyright (C) 2006,2007 Kevin Ottens <ervin@kde.org>

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

#include "kdialogjobuidelegate.h"

#include <QWidget>
#include <QQueue>
#include <QPointer>

#include <kmessagebox.h>
#include <kjob.h>
#include <kjobwidgets.h>

#include <config-kjobwidgets.h>
#if HAVE_X11
#include <QX11Info>
#endif

struct MessageBoxData {
    QWidget *widget;
    KMessageBox::DialogType type;
    QString msg;
};

class Q_DECL_HIDDEN KDialogJobUiDelegate::Private : public QObject
{
    Q_OBJECT
public:
    explicit Private(QObject *parent = nullptr);
    virtual ~Private();
    void queuedMessageBox(QWidget *widget, KMessageBox::DialogType type, const QString &msg);

    QWidget *window;

public Q_SLOTS:
    void next();

private:
    bool running;
    QQueue<QSharedPointer<MessageBoxData> > queue;
};

KDialogJobUiDelegate::Private::Private(QObject *parent)
    : QObject(parent)
    , window(nullptr)
    , running(false)
{
}

KDialogJobUiDelegate::Private::~Private()
{
    queue.clear();
}

void KDialogJobUiDelegate::Private::next()
{
    if (queue.isEmpty()) {
        running = false;
        return;
    }

    QSharedPointer<MessageBoxData> data = queue.dequeue();

    //kmessagebox starts a new event loop which can cause this to get deleted
    //https://bugs.kde.org/show_bug.cgi?id=356321#c16
    QPointer<KDialogJobUiDelegate::Private> thisGuard(this);
    KMessageBox::messageBox(data->widget, data->type, data->msg);

    if (!thisGuard) {
        return;
    }

    QMetaObject::invokeMethod(this, "next", Qt::QueuedConnection);
}

void KDialogJobUiDelegate::Private::queuedMessageBox(QWidget *widget, KMessageBox::DialogType type, const QString &msg)
{
    QSharedPointer<MessageBoxData> data(new MessageBoxData);
    data->type = type;
    data->widget = widget;
    data->msg = msg;

    queue.enqueue(data);

    if (!running) {
        running = true;
        QMetaObject::invokeMethod(this, "next", Qt::QueuedConnection);
    }
}

KDialogJobUiDelegate::KDialogJobUiDelegate()
    : d(new KDialogJobUiDelegate::Private)
{
}

KDialogJobUiDelegate::~KDialogJobUiDelegate()
{
    delete d;
}

bool KDialogJobUiDelegate::setJob(KJob *job)
{
    bool ret = KJobUiDelegate::setJob(job);
#if HAVE_X11
    if (ret) {
        unsigned long time = QX11Info::appUserTime();
        KJobWidgets::updateUserTimestamp(job, time);
    }
#endif
    return ret;
}

void KDialogJobUiDelegate::setWindow(QWidget *window)
{
    if (job()) {
        KJobWidgets::setWindow(job(), window);
    }
    d->window = window;
}

QWidget *KDialogJobUiDelegate::window() const
{
    if (d->window) {
        return d->window;
    } else if (job()) {
        return KJobWidgets::window(job());
    }
    return nullptr;
}

void KDialogJobUiDelegate::updateUserTimestamp(unsigned long time)
{
    KJobWidgets::updateUserTimestamp(job(), time);
}

unsigned long KDialogJobUiDelegate::userTimestamp() const
{
    return KJobWidgets::userTimestamp(job());
}

void KDialogJobUiDelegate::showErrorMessage()
{
    if (job()->error() != KJob::KilledJobError) {
        d->queuedMessageBox(window(), KMessageBox::Error, job()->errorString());
    }
}

void KDialogJobUiDelegate::slotWarning(KJob * /*job*/, const QString &plain, const QString &/*rich*/)
{
    if (isAutoWarningHandlingEnabled()) {
        d->queuedMessageBox(window(), KMessageBox::Information, plain);
    }
}

#include "kdialogjobuidelegate.moc"
