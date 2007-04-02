/* This file is part of the KDE project
 Copyright (C) 2002-2004 Alexander Dymo <adymo@mksat.net>

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
#ifndef COMMAND_H
#define COMMAND_H

#include <k3command.h>
#include "commdefs.h"
//Added by qt3to4:
#include <Q3ValueList>

/*    void slotAddDetailFooter();
    void slotAddDetail();
    void slotAddDetailHeader();
    void slotAddPageFooter();
    void slotAddPageHeader();
    void slotAddReportFooter();
    void slotAddReportHeader();
    void slotAddItemLine();
    void slotAddItemCalculated();
    void slotAddItemSpecial();
    void slotAddItemField();
    void slotAddItemLabel();*/

//class KudesignerDoc;

namespace Kudesigner
{

class DetailFooter;
class Detail;
class DetailHeader;
class PageFooter;
class PageHeader;
class ReportFooter;
class ReportHeader;
class Band;
class ReportItem;
class View;
class Canvas;
class Box;

class AddDetailFooterCommand: public K3NamedCommand
{
public:
    AddDetailFooterCommand( int level, Canvas *doc );

    virtual void execute();
    virtual void unexecute();

private:
    int m_level;
    Canvas *m_doc;
    DetailFooter *m_section;
};

class AddDetailCommand: public K3NamedCommand
{
public:
    AddDetailCommand( int level, Canvas *doc );

    virtual void execute();
    virtual void unexecute();

private:
    int m_level;
    Canvas *m_doc;
    Detail *m_section;
};

class AddDetailHeaderCommand: public K3NamedCommand
{
public:
    AddDetailHeaderCommand( int level, Canvas *doc );

    virtual void execute();
    virtual void unexecute();

private:
    int m_level;
    Canvas *m_doc;
    DetailHeader *m_section;
};

class AddPageFooterCommand: public K3NamedCommand
{
public:
    AddPageFooterCommand( Canvas *doc );

    virtual void execute();
    virtual void unexecute();

private:
    Canvas *m_doc;
    PageFooter *m_section;
};

class AddPageHeaderCommand: public K3NamedCommand
{
public:
    AddPageHeaderCommand( Canvas *doc );

    virtual void execute();
    virtual void unexecute();

private:
    Canvas *m_doc;
    PageHeader *m_section;
};

class AddReportFooterCommand: public K3NamedCommand
{
public:
    AddReportFooterCommand( Canvas *doc );

    virtual void execute();
    virtual void unexecute();

private:
    Canvas *m_doc;
    ReportFooter *m_section;
};

class AddReportHeaderCommand: public K3NamedCommand
{
public:
    AddReportHeaderCommand( Canvas *doc );

    virtual void execute();
    virtual void unexecute();

private:
    Canvas *m_doc;
    ReportHeader *m_section;
};

class AddReportItemCommand: public K3NamedCommand
{
public:
    AddReportItemCommand( Canvas *doc, View *rc, int x, int y, RttiValues section, int sectionLevel = -1 );

    virtual void execute();
    virtual void unexecute();

private:
    int m_rtti;
    Canvas *m_doc;
    View *m_rc;
    int m_x;
    int m_y;
    ReportItem *m_item;
    RttiValues m_section;
    int m_sectionLevel;
};

class DeleteReportItemsCommand: public K3NamedCommand
{
public:
    DeleteReportItemsCommand( Canvas *doc, Q3ValueList<Box*>& items );
    virtual void execute();
    virtual void unexecute();

private:
    Canvas *m_doc;
    Q3ValueList<Box*> m_items;
};

}

#endif
