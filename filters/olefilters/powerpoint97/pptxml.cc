/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION
*/

#include <pptdoc.h>
#include <pptxml.h>
#include <qdom.h>
#include <qobject.h>

PptXml::PptXml(
    const myFile &mainStream,
    const myFile &currentUser):
        PptDoc(
            mainStream,
            currentUser)
{
    QDomElement doc, paper, borders, *table;

    m_root = new QDomDocument("DOC");
    m_root->appendChild(
        m_root->createProcessingInstruction("xml", "version=\"1.0\" encoding =\"UTF-8\""));

    doc = m_root->createElement("DOC");
    doc.setAttribute("editor", "Powerpoint import filter");
    doc.setAttribute("mime", "application/x-kpresenter");
    doc.setAttribute("syntaxVersion", "2");
    m_root->appendChild(doc);

    paper = m_root->createElement("PAPER");
    paper.setAttribute("format", "5");
    doc.appendChild(paper);

    borders = m_root->createElement("PAPERBORDERS");
    borders.setAttribute("left", 20);
    borders.setAttribute("top", 20);
    borders.setAttribute("right", 20);
    borders.setAttribute("bottom", 20);
    paper.appendChild(borders);
}

PptXml::~PptXml()
{
}

const bool PptXml::convert()
{
    if (!m_isConverted)
        m_success = parse();
    return m_success;
}

void PptXml::gotSlide(
    const Slide &slide)
{
    kdDebug(s_area) << "Slide title: " << slide.title << endl;
    kdDebug(s_area) << "Slide body: " << slide.body.join("\n") << endl;
}
