/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "MsooXmlCommentsReader.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>

#define MSOOXML_CURRENT_CLASS MsooXmlCommentsReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

using namespace MSOOXML;

MsooXmlCommentsReaderContext::MsooXmlCommentsReaderContext(QMap<QString, QStringList>& _comments)
        : comments(&_comments)
{
}

class MsooXmlCommentsReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }
    QString pathAndFile;
};

MsooXmlCommentsReader::MsooXmlCommentsReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlReader(writers)
        , m_context(0)
        , d(new Private)
{
    init();
}

MsooXmlCommentsReader::~MsooXmlCommentsReader()
{
    delete d;
}

void MsooXmlCommentsReader::init()
{
}

KoFilter::ConversionStatus MsooXmlCommentsReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<MsooXmlCommentsReaderContext*>(context);
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;    
    return result;
}

KoFilter::ConversionStatus MsooXmlCommentsReader::readInternal()
{
    readNext();
    
    QString commentId, commentAuthor, commentDate, commentText;
    
    while (!atEnd()) {
        
        if (name() == "comment") {
            QXmlStreamAttributes attr = attributes();
            
            if (attr.hasAttribute("w:id")) {
                commentId = attr.value("w:id").toString();
            }
            
            if (attr.hasAttribute("w:author")) {
                commentAuthor = attr.value("w:author").toString();
            }
            
            if (attr.hasAttribute("w:date")) {
                commentDate = attr.value("w:date").toString();
            }
        }
        
        // this could be done better! Text can have formatting and this extracts only pure text
        if (name() == "t" && isStartElement()) {
            readNext();
            if (text().toString() != "")
                commentText += text().toString();
        }
        
        if (name() == "comment" && isEndElement()) {
            QStringList list;
            
            if (commentId != "") {
                
                if (commentDate.endsWith("Z"))
                    commentDate.remove(commentDate.length()-1, 1); 
                
                list << commentAuthor << commentDate << commentText;
                m_context->comments->insert(commentId, list);
                //kDebug() << "MsooXmlCommentsReader::readInternal() id: " <<  commentId << " date: " << commentDate <<  " author: " << commentAuthor <<  " text: " << commentText;
            }
            
            commentId = "";
            commentDate = "";
            commentAuthor = "";
            commentText = "";
        }
                
        readNext();
        
        if (hasError()) {
            break;
        }
    }    
    
    return KoFilter::OK;    
}
