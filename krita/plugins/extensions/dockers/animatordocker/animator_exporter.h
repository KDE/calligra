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

#include "kis_doc2.h"
#include "KoFilterManager.h"

#include "animator_model.h"

class AnimatorExporter : public QObject
{
    Q_OBJECT
    
public:
    AnimatorExporter(AnimatorModel* model, KisDoc2* document);
    virtual ~AnimatorExporter();
    
public:
    void setPrefix(QString prefix);
    void setSuffix(QString suffix);
    void setNumberWidth(int n);
    
    const QString& getName(int i);
    
    void setDocument(KisDoc2* document);
    
public slots:
    void exportAll();
    
    
private:
    AnimatorModel* m_model;
    
    QString m_prefix;
    QString m_suffix;
    
    int m_leading_zeros_number;
    
    
    KisDoc2* m_document;
    
    QString m_dir;
    
    
//     KisImage* m_image;
    KoFilterManager* m_filter_manager;
};

#endif // ANIMATOR_EXPORTER_H
