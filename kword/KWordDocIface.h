/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
*/

#ifndef KWORD_DOC_IFACE_H
#define KWORD_DOC_IFACE_H

#include <KoDocumentIface.h>
#include <dcopref.h>

#include <qstring.h>

class KWDocument;

class KWordDocIface : virtual public KoDocumentIface
{
    K_DCOP
public:
    KWordDocIface(  KWDocument *doc_ );

k_dcop:
    virtual int getNumPages();
    virtual int getNumFrameSets();
    virtual unsigned int getColumns();

    //paper
    virtual double ptTopBorder();
    virtual double ptBottomBorder();
    virtual double ptLeftBorder();
    virtual double ptRightBorder();
    virtual double ptPaperHeight();
    virtual double ptPaperWidth();
    virtual double ptColumnWidth();
    virtual double ptColumnSpacing();
    virtual double ptPageTop( int pgNum );

    //configure
    virtual double gridX();
    virtual double gridY();
    virtual void setGridX(double _gridx);
    virtual void setGridY(double _gridy);
    virtual QString getUnitName();
    virtual double getIndentValue();
    virtual void setIndentValue(double _ind);

    virtual int getNbPagePerRow();
    virtual void setNbPagePerRow(int _nb);
    virtual int defaultColumnSpacing();
    virtual void setDefaultColumnSpacing(int _val);
    virtual int maxRecentFiles();

    virtual void setUndoRedoLimit(int _val);

    bool showRuler() const;
    bool dontCheckUpperWord();
    bool dontCheckTitleCase() const;
    bool showdocStruct() const;
    bool viewFrameBorders() const; 

    virtual void recalcAllVariables();
    virtual void recalcVariables(int _var);
    virtual void recalcVariables(const QString &varName);
private:
    KWDocument *doc;

};

#endif
