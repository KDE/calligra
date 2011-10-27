/*
 *  Exporter
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef ANIMATOR_EXPORTER_H
#define ANIMATOR_EXPORTER_H

#include <QObject>

#include <KoFilterManager.h>
#include <KoCanvasObserverBase.h>

#include <kis_doc2.h>

#include "animator_manager.h"

class AnimatorExporter : public QObject, KoCanvasObserverBase
{
    Q_OBJECT
    
public:
    AnimatorExporter(AnimatorManager* manager);
    virtual ~AnimatorExporter();
    
public:
    virtual void setPrefix(const QString& prefix);
    virtual void setSuffix(const QString& suffix);
    virtual void setNumberWidth(int n);
    
    virtual const QString getName(int i);
    
public:
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void unsetCanvas();
    
public slots:
    virtual void exportFrame(int num);
    virtual void exportAll();
    
protected:
    virtual void setDocument(KisDoc2* document);
    
private:
    AnimatorManager* m_manager;
    
private:
    QString m_dir;
    QString m_prefix;
    QString m_suffix;
    int m_leadingZeros;
    
private:
    KisDoc2* m_document;
    KoFilterManager* m_filter_manager;
};

#endif // ANIMATOR_EXPORTER_H
