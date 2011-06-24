/* This file is part of the KDE project
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "SvgLoadingContext.h"
#include "SvgGraphicContext.h"
#include "SvgUtil.h"

#include <KDebug>

#include <QtCore/QStack>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

class SvgLoadingContext::Private
{
public:
    Private()
        : zIndex(0)
    {

    }

    ~Private()
    {
        if (! gcStack.isEmpty())
            kWarning() << "the context stack is not empty (current count" << gcStack.size() << ", expected 0)";
        qDeleteAll(gcStack);
        gcStack.clear();
    }
    QStack<SvgGraphicsContext*> gcStack;
    QString initialXmlBaseDir;
    int zIndex;
};

SvgLoadingContext::SvgLoadingContext()
    : d(new Private())
{

}

SvgLoadingContext::~SvgLoadingContext()
{
    delete d;
}

SvgGraphicsContext *SvgLoadingContext::currentGC()
{
    if (d->gcStack.isEmpty())
        return 0;

    return d->gcStack.top();
}

SvgGraphicsContext *SvgLoadingContext::pushGraphicsContext(const KoXmlElement &element, bool inherit)
{
    SvgGraphicsContext *gc = new SvgGraphicsContext;

    // copy data from current context
    if (! d->gcStack.isEmpty() && inherit)
        *gc = *(d->gcStack.top());

    gc->filterId.clear(); // filters are not inherited
    gc->clipPathId.clear(); // clip paths are not inherited
    gc->display = true; // display is not inherited
    gc->opacity = 1.0; // opacity is not inherited

    if (!element.isNull()) {
        if (element.hasAttribute("transform")) {
            QTransform mat = SvgUtil::parseTransform(element.attribute("transform"));
            gc->matrix = mat * gc->matrix;
        }
        if (element.hasAttribute("xml:base"))
            gc->xmlBaseDir = element.attribute("xml:base");
        if (element.hasAttribute("xml:space"))
            gc->preserveWhitespace = element.attribute("xml:space") == "preserve";
    }

    d->gcStack.push(gc);

    return gc;
}

void SvgLoadingContext::popGraphicsContext()
{
    delete(d->gcStack.pop());
}

void SvgLoadingContext::setInitialXmlBaseDir(const QString &baseDir)
{
    d->initialXmlBaseDir = baseDir;
}

QString SvgLoadingContext::xmlBaseDir()
{
    SvgGraphicsContext *gc = currentGC();
    return (gc && !gc->xmlBaseDir.isEmpty()) ? gc->xmlBaseDir : d->initialXmlBaseDir;
}

QString SvgLoadingContext::absoluteFilePath(const QString &href)
{
    QFileInfo info(href);
    if (! info.isRelative())
        return href;

    SvgGraphicsContext *gc = currentGC();
    if (!gc)
        return d->initialXmlBaseDir;

    QString baseDir = d->initialXmlBaseDir;
    if (! gc->xmlBaseDir.isEmpty())
        baseDir = absoluteFilePath(gc->xmlBaseDir);

    QFileInfo pathInfo(QFileInfo(baseDir).filePath());

    QString relFile = href;
    while (relFile.startsWith(QLatin1String("../"))) {
        relFile = relFile.mid(3);
        pathInfo.setFile(pathInfo.dir(), QString());
    }

    QString absFile = pathInfo.absolutePath() + '/' + relFile;

    return absFile;
}

int SvgLoadingContext::nextZIndex()
{
    return d->zIndex++;
}

