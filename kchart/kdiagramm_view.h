/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#ifndef __kdiagramm_gui_h__
#define __kdiagramm_gui_h__

class KDiagrammView;
class KDiagrammDoc;
class KDiagrammShell;

#include <koFrame.h>
#include <koView.h>
#include <opMenu.h>
#include <opToolBar.h>
#include <koFrame.h>
#include <openparts_ui.h>

#include <qlist.h>
#include <qscrbar.h>
#include <qlabel.h>
#include <qbutton.h>
#include <qpoint.h>

#include "kdiagramm.h"
#include <koDiagramm.h>

/**
 */
class KDiagrammView : public KoDiagrammView,
		    virtual public KoViewIf,
		    virtual public KDiagramm::View_skel
{
    Q_OBJECT
public:
    KDiagrammView( QWidget *_parent, const char *_name, KDiagrammDoc *_doc );
    ~KDiagrammView();

    KDiagrammDoc* doc() { return m_pDoc; }

    /**
     * ToolBar
     */
    void modeLines();
    /**
     * ToolBar
     */
    void modeAreas();
    /**
     * ToolBar
     */
    void modeBars();
    /**
     * ToolBar
     */
    void modeCakes();

    /**
     * MenuBar
     */
    void editData();

    /**
     * MenuBar
     */
    void pageLayout();

    /**
     * MenuBar
     */
    void configChart();

    virtual void cleanUp();

    CORBA::Boolean printDlg();

public slots:
    // Document signals
    void slotUpdateView();

protected:
    // C++
    virtual void init();
    // IDL
    virtual bool event( const char* _event, const CORBA::Any& _value );
    // C++
    virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
    virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );
  //virtual bool mappingEventConfigured( KDiagramm::View::EventConfigured _event );

    virtual void newView();
    virtual void helpUsing();

    OpenPartsUI::ToolBar_var m_vToolBarEdit;
    CORBA::Long m_idButtonEdit_Lines;
    CORBA::Long m_idButtonEdit_Areas;
    CORBA::Long m_idButtonEdit_Bars;
    CORBA::Long m_idButtonEdit_Cakes;

    OpenPartsUI::Menu_var m_vMenuEdit;
    CORBA::Long m_idMenuEdit_Lines;
    CORBA::Long m_idMenuEdit_Areas;
    CORBA::Long m_idMenuEdit_Bars;
    CORBA::Long m_idMenuEdit_Cakes;
    CORBA::Long m_idMenuEdit_Data;
    CORBA::Long m_idMenuEdit_Page;
    CORBA::Long m_idMenuConfig_Chart;

    OpenPartsUI::Menu_var m_vMenuHelp;
    CORBA::Long m_idMenuHelp_About;
    CORBA::Long m_idMenuHelp_Using;

    KDiagrammDoc *m_pDoc;
};

#endif


