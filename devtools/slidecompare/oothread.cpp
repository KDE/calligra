/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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
 * Boston, MA 02110-1301, USA.
*/
#include "oothread.h"
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QDateTime>
#include <cstdio>

#define UNX
#define LINUX
#define CPPU_ENV gcc3
#define OSL_DEBUG_LEVEL 0
#include <cppuhelper/bootstrap.hxx>
#include <osl/process.h>
#include <osl/file.hxx>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/util/XCloseable.hpp>
 
using namespace com::sun::star::bridge;
using namespace com::sun::star::container;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;
using namespace cppu;
using namespace rtl;

class OoThread::OOConnection {
public:
    Reference<XComponentContext> rComponentContext;
    Reference<XMultiComponentFactory> xServiceManager;
    Reference<XComponentLoader> xComponentLoader;
    OOConnection() {
        try {
            rComponentContext = Reference<XComponentContext>(bootstrap());
            xServiceManager = Reference<XMultiComponentFactory>(
                rComponentContext->getServiceManager());
            OUString d(
                    RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop"));
            xComponentLoader = Reference<XComponentLoader>(
                xServiceManager->createInstanceWithContext(d,
                     rComponentContext), UNO_QUERY_THROW);
        } catch(Exception &e) {
            OString o = OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
            qDebug() << "Error in OOConnection(): " << o;
        } catch(...) {
            qDebug() << "Unknown error in OOConnection().";
        }
    }
    ~OOConnection() {
        try {
            if (xComponentLoader.get()) {
                Reference<XDesktop>::query(xComponentLoader)->terminate();
            }
        } catch(Exception &e) {
            OString o = OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
            qDebug() << "Error in ~OOConnection(): " << o;
        } catch(...) {
            qDebug() << "Unknown error in ~OOConnection().";
        }
    }
};
 
OoThread::OoThread(QObject* o) :QThread(o), oo(0) {
    qsrand(QDateTime::currentDateTime().toTime_t());
    running = true;
    start();
}
OoThread::~OoThread() {
    stop();
    wait();
}
void OoThread::run() {
    oo = new OOConnection();
    running = oo->xComponentLoader.get() != 0;
    while (running) {
        mutex.lock();
        currentToOdp = currentToPng = Conversion();
        if (!(nextToOdp || nextToPng)) {
            moreWork.wait(&mutex);
        }
        Conversion toOdp;
        Conversion toPng;
        if (nextToOdp) {
            toOdp = currentToOdp = nextToOdp;
            nextToOdp = Conversion();
        } else if (nextToPng) {
            toPng = currentToPng = nextToPng;
            nextToPng = Conversion();
        }
        mutex.unlock();

        if (toOdp) {
            convertToOdp(toOdp);
            emit toOdpDone(toOdp.to);
        } else if (toPng) {
            convertToPng(toPng);
            emit toPngDone(toPng.to);
        }
    }
    delete oo;
}
void OoThread::stop() {
    running = false;
    moreWork.wakeAll();
}
namespace {
    QString tmpname() {
        return QString::number(qrand());
    }
}
QString
OoThread::toOdp(const QString& path) {
    QDir dir(QDir::temp().filePath("slidecompare-" + QDir::home().dirName()));
    dir.mkpath(dir.absolutePath());
    Conversion c;
    c.from = path;
    do {
        c.to = dir.absoluteFilePath(tmpname()+".odp");
    } while(dir.exists(c.to));
    mutex.lock();
    nextToOdp = c;
    moreWork.wakeAll();
    mutex.unlock();
    return c.to;
}
QString
OoThread::toPng(const QString& path, int pngwidth) {
    QDir dir(QDir::temp().filePath("slidecompare-" + QDir::home().dirName()));
    dir.mkpath(dir.absolutePath());
    Conversion c;
    c.from = path;
    c.width = pngwidth;
    do {
        c.to = dir.absoluteFilePath(tmpname() + '/');
    } while(dir.exists(c.to));
    dir.mkdir(c.to);
    mutex.lock();
    nextToPng = c;
    moreWork.wakeAll();
    mutex.unlock();
    return c.to;
}
bool
OoThread::waitingOrBusy(const QString& path) {
    bool b;
    mutex.lock();
    b = nextToOdp.to == path || nextToPng.to == path
        || currentToOdp.to == path || currentToPng.to == path;
    mutex.unlock();
    return b;
}
void
OoThread::convertToOdp(const Conversion& c) {
    QByteArray fromUrl(QUrl::fromLocalFile(c.from).toEncoded());
    QByteArray toUrl(QUrl::fromLocalFile(c.to).toEncoded());

    qDebug() << "converting from '" << fromUrl << "' to '" << toUrl;

    Sequence<PropertyValue> props(1);
    props[0].Name = OUString::createFromAscii("Hidden");
    props[0].Value = Any(true);
    try {
        Reference<XComponent> xComponent
                = oo->xComponentLoader->loadComponentFromURL(
                    OUString::createFromAscii(fromUrl.constData()),
                    OUString::createFromAscii("_default"),
                    0, props);

        Sequence<PropertyValue> htmlProps(2);
        htmlProps[0].Name = OUString::createFromAscii("Overwrite");
        htmlProps[0].Value = Any(true);
        htmlProps[1].Name = OUString::createFromAscii("FilterName");
        htmlProps[1].Value <<= OUString::createFromAscii("impress8");

        Reference<XStorable> store(xComponent, UNO_QUERY);

        if (store.is()) {
            OUString dest_url = OUString::createFromAscii(toUrl.constData());
            store->storeToURL(dest_url, htmlProps);
        }
        Reference<XModifiable>::query(xComponent)->setModified(false);
        Reference<XCloseable>::query(xComponent)->close(true);
        Reference<XComponent>::query(xComponent)->dispose();
    } catch(Exception &e) {
        OString o = OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
        qDebug() << "Error in toOdp: " << o;
    } catch(...) {
        qDebug() << "Unknown error in convertToPng().";
    }
}
void
OoThread::convertToPng(const Conversion& c) {
    QByteArray fromUrl(QUrl::fromLocalFile(
            QFileInfo(c.from).absoluteFilePath()).toEncoded());
    QByteArray toUrl(QUrl::fromLocalFile(
            QFileInfo(c.to).absoluteFilePath()+"/index.html").toEncoded());

    qDebug() << "converting from '" << fromUrl << "' to '" << toUrl << " "
            << c.width;

    Sequence<PropertyValue> props(1);
    props[0].Name = OUString::createFromAscii("Hidden");
    props[0].Value = Any(true);
    try {
        Reference<XComponent> xComponent
                = oo->xComponentLoader->loadComponentFromURL(
                    OUString::createFromAscii(fromUrl.constData()),
                    OUString::createFromAscii("_default"),
                    0, props);

        Sequence<PropertyValue> pngProps(3);
        pngProps[0].Name = OUString::createFromAscii("Format");
        pngProps[0].Value = Any((sal_uInt32)2); // png
        pngProps[1].Name = OUString::createFromAscii("Width");
        pngProps[1].Value = Any((sal_uInt32)c.width);
        pngProps[2].Name = OUString::createFromAscii("PublishMode");
        pngProps[2].Value = Any((sal_uInt32)0); // html

        Sequence<PropertyValue> htmlProps(3);
        htmlProps[0].Name = OUString::createFromAscii("Overwrite");
        htmlProps[0].Value = Any(true);
        htmlProps[1].Name = OUString::createFromAscii("FilterName");
        htmlProps[1].Value <<= OUString::createFromAscii("impress_html_Export");
        htmlProps[2].Name = OUString::createFromAscii("FilterData");
        htmlProps[2].Value <<= pngProps;

        Reference<XStorable> store(xComponent, UNO_QUERY);
        if (store.is()) {
            OUString dest_url = OUString::createFromAscii(toUrl.constData());
            store->storeToURL(dest_url, htmlProps);
        }
        Reference<XModifiable>::query(xComponent)->setModified(false);
        Reference<XCloseable>::query(xComponent)->close(true);
        Reference<XComponent>::query(xComponent)->dispose();
    } catch(Exception &e) {
        OString o = OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
        qDebug() << "Error in toPng: " << o;
    } catch(...) {
        qDebug() << "Unknown error in convertToPng().";
    }
}
