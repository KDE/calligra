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

#include <KoFilterManager.h>

#include "animator_exporter.h"

#include <iostream>

AnimatorExporter::AnimatorExporter(AnimatorModel* model, KisDoc2* document)
{
    m_model = model;
    setDocument(document);
    
//     m_filter_manager = 0;
    
    setPrefix(QString(""));
    setSuffix(QString(".png"));
    setNumberWidth(4);
}

void AnimatorExporter::setDocument(KisDoc2* document)
{
    if (document != m_document)
    {
        m_document = document;
        m_filter_manager = new KoFilterManager(m_document);
    }
}

AnimatorExporter::~AnimatorExporter()
{

}

void AnimatorExporter::exportFrame(int num)
{
    m_model->setFrame(num);
    QByteArray* mime = new QByteArray("image/png");
    KoFilter::ConversionStatus status =  m_filter_manager->exportDocument(getName(num), *mime);
}

void AnimatorExporter::exportAll()
{
//     m_document->exportDocument();
    
//     if (!m_filter_manager)
//         m_filter_manager = new KoFilterManager(m_document);
    
    m_model->disableOnion();
    
    for (int i = 0; i < m_model->columnCount(); ++i)
    {
        exportFrame(i);
    }
    
    m_model->enableOnion();
}

void AnimatorExporter::setPrefix(QString prefix)
{
    m_prefix = prefix;
}

void AnimatorExporter::setSuffix(QString suffix)
{
    m_suffix = suffix;
}

const QString& AnimatorExporter::getName(int i)
{
    QString *t = new QString;
    QString num = QString::number(i);
    
    m_dir = m_document->url().directory();
    
    setPrefix(m_document->url().fileName());
    
    *t = m_dir + QString("/") + m_prefix + num.rightJustified(m_leading_zeros_number, '0') + m_suffix;          // May be there are some functions in KUrl?
    
    return *t;
}

void AnimatorExporter::setNumberWidth(int n)
{
    if (n > 0)
    {
        m_leading_zeros_number = n;
    }
}
