/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include <kcommand.h>
#include <KoGlobal.h>
#include "kivio_command.h"
#include "kivio_page.h"
#include "kivio_map.h"
#include "kivio_doc.h"
#include "kivio_layer.h"
#include "kivio_group_stencil.h"

KivioChangePageNameCommand::KivioChangePageNameCommand(const QString &_name,  const QString & _oldPageName, const QString & _newPageName, KivioPage *_page)
    : KNamedCommand( _name ),
      oldPageName(_oldPageName ),
      newPageName(_newPageName ),
      m_page( _page )
{

}

KivioChangePageNameCommand::~KivioChangePageNameCommand()
{
}

void KivioChangePageNameCommand::execute()
{
    m_page->setPageName( newPageName );
}

void KivioChangePageNameCommand::unexecute()
{
    m_page->setPageName( oldPageName);
}


KivioHidePageCommand::KivioHidePageCommand(const QString &_name, KivioPage *_page)
    : KNamedCommand( _name ),
      m_page( _page )
{
}

KivioHidePageCommand::~KivioHidePageCommand()
{
}

void KivioHidePageCommand::execute()
{
    m_page->setHidePage( true );
}

void KivioHidePageCommand::unexecute()
{
    m_page->setHidePage( false );
}

KivioShowPageCommand::KivioShowPageCommand( const QString &_name, KivioPage *_page)
    : KivioHidePageCommand( _name, _page)
{
}


KivioAddPageCommand::KivioAddPageCommand(const QString &_name, KivioPage *_page)
    : KNamedCommand( _name ),
      m_page( _page )
{
}

KivioAddPageCommand::~KivioAddPageCommand()
{
}

void KivioAddPageCommand::execute()
{
    m_page->map()->insertPage( m_page );
    m_page->doc()->insertPage( m_page );
}

void KivioAddPageCommand::unexecute()
{
    m_page->map()->takePage( m_page );
    m_page->doc()->takePage( m_page );
}

KivioRemovePageCommand::KivioRemovePageCommand(const QString &_name, KivioPage *_page)
    : KNamedCommand( _name ),
      m_page( _page )
{
}

KivioRemovePageCommand::~KivioRemovePageCommand()
{
}

void KivioRemovePageCommand::execute()
{
    m_page->map()->takePage( m_page );
    m_page->doc()->takePage( m_page );
}

void KivioRemovePageCommand::unexecute()
{
    m_page->map()->insertPage( m_page );
    m_page->doc()->insertPage( m_page );

}

KivioAddStencilCommand::KivioAddStencilCommand(const QString &_name, KivioPage *_page,  KivioLayer * _layer, KivioStencil *_stencil )
    : KNamedCommand( _name ),
      m_page( _page ),
      m_layer( _layer),
      m_stencil( _stencil)
{
}

KivioAddStencilCommand::~KivioAddStencilCommand()
{
}

void KivioAddStencilCommand::execute()
{
    m_layer->insertStencil( m_stencil );
    m_page->doc()->updateView(m_page);
    m_stencil->unselect();
    m_stencil->searchForConnections(m_page, 4.0); //FIXME: The threshold should probably be zoomed....
    m_page->doc()->slotSelectionChanged();
}

void KivioAddStencilCommand::unexecute()
{
    m_layer->takeStencilFromList( m_stencil );
    m_page->doc()->updateView(m_page);
    m_stencil->unselect();
    m_page->doc()->slotSelectionChanged();
}

KivioRemoveStencilCommand::KivioRemoveStencilCommand(const QString &_name, KivioPage *_page,  KivioLayer * _layer, KivioStencil *_stencil )
    : KivioAddStencilCommand(_name, _page, _layer, _stencil )
{
}

KivioChangeStencilTextCommand::KivioChangeStencilTextCommand(const QString& _name, KivioStencil* _stencil,
    const QString& _oldText, const QString& _newText, KivioPage* _page, const QString& textBoxName)
    : KNamedCommand( _name ),
      m_stencil(_stencil),
      oldText(_oldText),
      newText(_newText),
      m_page(_page),
      m_textBoxName(textBoxName)
{
}

KivioChangeStencilTextCommand::~KivioChangeStencilTextCommand()
{
}

void KivioChangeStencilTextCommand::execute()
{
  if(m_textBoxName.isEmpty()) {
    m_stencil->setText(newText);
  } else {
    m_stencil->setText(newText, m_textBoxName);
  }

  m_page->doc()->updateView(m_page);
}

void KivioChangeStencilTextCommand::unexecute()
{
  if(m_textBoxName.isEmpty()) {
    m_stencil->setText(oldText);
  } else {
    m_stencil->setText(oldText, m_textBoxName);
  }

  m_page->doc()->updateView(m_page);
}


KivioAddLayerCommand::KivioAddLayerCommand( const QString &_name, KivioPage *_page, KivioLayer * _layer, int _pos )
    :KNamedCommand( _name ),
     m_page( _page ),
     m_layer( _layer),
     layerPos(_pos)
{
}

KivioAddLayerCommand::~KivioAddLayerCommand()
{
}

void KivioAddLayerCommand::execute()
{
    m_page->insertLayer( layerPos-1, m_layer );
    m_page->doc()->updateView(m_page);
    m_page->doc()->resetLayerPanel();

}

void KivioAddLayerCommand::unexecute()
{
    m_page->takeLayer( m_layer );
    m_page->doc()->updateView(m_page);
    m_page->doc()->resetLayerPanel();
}


KivioRemoveLayerCommand::KivioRemoveLayerCommand( const QString &_name, KivioPage *_page, KivioLayer * _layer, int _pos )
    :KivioAddLayerCommand( _name, _page, _layer, _pos )
{
}

KivioRenameLayerCommand::KivioRenameLayerCommand( const QString &_name, KivioLayer * _layer, const QString & _oldName, const QString & _newName)
    :KNamedCommand( _name ),
     m_layer( _layer ),
     oldName( _oldName),
     newName( _newName)
{
}

KivioRenameLayerCommand::~KivioRenameLayerCommand()
{
}

void KivioRenameLayerCommand::execute()
{
    m_layer->setName(newName);
    m_layer->page()->doc()->resetLayerPanel();
}

void KivioRenameLayerCommand::unexecute()
{
    m_layer->setName(oldName);
    m_layer->page()->doc()->resetLayerPanel();
}

KivioResizeStencilCommand::KivioResizeStencilCommand( const QString &_name, KivioStencil *_stencil, KoRect _initSize, KoRect _endSize, KivioPage *_page)
    :KNamedCommand( _name ),
     m_stencil( _stencil),
     initSize( _initSize),
     endSize( _endSize ),
     m_page( _page)
{
}

KivioResizeStencilCommand::~KivioResizeStencilCommand()
{
}

void KivioResizeStencilCommand::execute()
{
    m_stencil->setDimensions( endSize.width(), endSize.height() );
    m_stencil->setPosition( endSize.x(), endSize.y() );
    m_page->doc()->updateView(m_page);
}

void KivioResizeStencilCommand::unexecute()
{
    m_stencil->setDimensions( initSize.width(), initSize.height() );
    m_stencil->setPosition( initSize.x(), initSize.y() );
    m_page->doc()->updateView(m_page);
}


KivioMoveStencilCommand::KivioMoveStencilCommand( const QString &_name, KivioStencil *_stencil, KoRect _initSize, KoRect _endSize, KivioPage *_page)
    :KNamedCommand( _name ),
     m_stencil( _stencil),
     initSize( _initSize),
     endSize( _endSize ),
     m_page( _page)
{
}

KivioMoveStencilCommand::~KivioMoveStencilCommand()
{
}

void KivioMoveStencilCommand::execute()
{
    m_stencil->setDimensions( endSize.width(), endSize.height() );
    m_stencil->setPosition( endSize.x(), endSize.y() );
    m_page->doc()->updateView(m_page);
}

void KivioMoveStencilCommand::unexecute()
{
    m_stencil->setDimensions( initSize.width(), initSize.height() );
    m_stencil->setPosition( initSize.x(), initSize.y() );
    m_page->doc()->updateView(m_page);
}

KivioChangeLayoutCommand::KivioChangeLayoutCommand( const QString &_name, KivioPage *_page, KoPageLayout _oldLayout, KoPageLayout _newLayout)
    :KNamedCommand( _name),
     m_page(_page),
     oldLayout( _oldLayout),
     newLayout( _newLayout)
{
}

KivioChangeLayoutCommand::~KivioChangeLayoutCommand()
{
}

void KivioChangeLayoutCommand::execute()
{
    m_page->setPaperLayout(newLayout);
}

void KivioChangeLayoutCommand::unexecute()
{
    m_page->setPaperLayout(oldLayout);
}

KivioChangeStencilHAlignmentCommand::KivioChangeStencilHAlignmentCommand(const QString& _name, KivioPage* _page,
    KivioStencil* _stencil, int _oldAlign, int _newAlign, const QString& textBoxName)
    : KNamedCommand(_name),
     m_page(_page),
     m_stencil(_stencil),
     oldAlign(_oldAlign),
     newAlign(_newAlign),
     m_textBoxName(textBoxName)
{
}

KivioChangeStencilHAlignmentCommand::~KivioChangeStencilHAlignmentCommand()
{
}

void KivioChangeStencilHAlignmentCommand::execute()
{
  if(m_textBoxName.isEmpty()) {
    m_stencil->setHTextAlign(newAlign);
  } else {
    m_stencil->setHTextAlign(m_textBoxName, newAlign);
  }

  m_page->doc()->updateView(m_page);
  m_page->doc()->slotSelectionChanged();
}

void KivioChangeStencilHAlignmentCommand::unexecute()
{
  if(m_textBoxName.isEmpty()) {
    m_stencil->setHTextAlign(newAlign);
  } else {
    m_stencil->setHTextAlign(m_textBoxName, newAlign);
  }

  m_page->doc()->updateView(m_page);
  m_page->doc()->slotSelectionChanged();
}

KivioChangeStencilVAlignmentCommand::KivioChangeStencilVAlignmentCommand(const QString& _name, KivioPage* _page,
    KivioStencil* _stencil, int _oldAlign, int _newAlign, const QString& textBoxName)
    : KNamedCommand(_name),
     m_page(_page),
     m_stencil(_stencil),
     oldAlign(_oldAlign),
     newAlign(_newAlign),
     m_textBoxName(textBoxName)
{
}

KivioChangeStencilVAlignmentCommand::~KivioChangeStencilVAlignmentCommand()
{
}

void KivioChangeStencilVAlignmentCommand::execute()
{
  if(m_textBoxName.isEmpty()) {
    m_stencil->setVTextAlign(newAlign);
  } else {
    m_stencil->setVTextAlign(m_textBoxName, newAlign);
  }

  m_page->doc()->updateView(m_page);
  m_page->doc()->slotSelectionChanged();
}

void KivioChangeStencilVAlignmentCommand::unexecute()
{
  if(m_textBoxName.isEmpty()) {
    m_stencil->setVTextAlign(oldAlign);
  } else {
    m_stencil->setVTextAlign(m_textBoxName, oldAlign);
  }

  m_page->doc()->updateView(m_page);
  m_page->doc()->slotSelectionChanged();
}


KivioChangeStencilFontCommand::KivioChangeStencilFontCommand(const QString& _name, KivioPage* _page,
    KivioStencil* _stencil, const QFont& _oldFont, const QFont& _newFont, const QString& textBoxName)
    : KNamedCommand(_name),
     m_page(_page),
     m_stencil( _stencil),
     oldFont(_oldFont),
     newFont(_newFont),
     m_textBoxName(textBoxName)
{
}

KivioChangeStencilFontCommand::~KivioChangeStencilFontCommand()
{
}

void KivioChangeStencilFontCommand::execute()
{
  if(m_textBoxName.isEmpty()) {
    m_stencil->setTextFont(newFont);
  } else {
    m_stencil->setTextFont(m_textBoxName, newFont);
  }

  m_page->doc()->updateView(m_page);
  m_page->doc()->slotSelectionChanged();
}

void KivioChangeStencilFontCommand::unexecute()
{
  if(m_textBoxName.isEmpty()) {
    m_stencil->setTextFont(oldFont);
  } else {
    m_stencil->setTextFont(m_textBoxName, oldFont);
  }

  m_page->doc()->updateView(m_page);
  m_page->doc()->slotSelectionChanged();
}

KivioChangeStencilColorCommand::KivioChangeStencilColorCommand(const QString& _name, KivioPage* _page,
    KivioStencil* _stencil, const QColor& _oldColor,  const QColor& _newColor, ColorType _type, const QString& textBoxName)
    : KNamedCommand(_name),
     m_page(_page),
     m_stencil(_stencil),
     oldColor(_oldColor),
     newColor(_newColor),
     type(_type),
     m_textBoxName(textBoxName)
{
}

KivioChangeStencilColorCommand::~KivioChangeStencilColorCommand()
{
}

void KivioChangeStencilColorCommand::execute()
{
  switch( type ) {
    case CT_TEXTCOLOR:
      if(m_textBoxName.isEmpty()) {
        m_stencil->setTextColor(newColor);
      } else {
        m_stencil->setTextColor(m_textBoxName, newColor);
      }
      break;
    case CT_BGCOLOR:
      m_stencil->setBGColor(newColor);
      break;
    case CT_FGCOLOR:
      m_stencil->setFGColor(newColor);
      break;
  }

  m_page->doc()->updateView(m_page);
  m_page->doc()->slotSelectionChanged();
}

void KivioChangeStencilColorCommand::unexecute()
{
  switch( type ) {
    case CT_TEXTCOLOR:
      if(m_textBoxName.isEmpty()) {
        m_stencil->setTextColor(oldColor);
      } else {
        m_stencil->setTextColor(m_textBoxName, oldColor);
      }
      break;
    case CT_BGCOLOR:
      m_stencil->setBGColor(oldColor);
      break;
    case CT_FGCOLOR:
      m_stencil->setFGColor(oldColor);
      break;
  }

  m_page->doc()->updateView(m_page);
  m_page->doc()->slotSelectionChanged();
}


KivioChangeLineWidthCommand::KivioChangeLineWidthCommand( const QString &_name,
    KivioPage *_page, KivioStencil * _stencil, double _oldValue, double _newValue)
    :KNamedCommand( _name),
    m_page(_page),
    m_stencil( _stencil ),
    oldValue( _oldValue),
    newValue( _newValue)
{
}

KivioChangeLineWidthCommand::~KivioChangeLineWidthCommand()
{
}

void KivioChangeLineWidthCommand::execute()
{
    m_stencil->setLineWidth( newValue );
    m_page->doc()->updateView(m_page);
    m_page->doc()->slotSelectionChanged();
}

void KivioChangeLineWidthCommand::unexecute()
{
    m_stencil->setLineWidth( oldValue );
    m_page->doc()->updateView(m_page);
    m_page->doc()->slotSelectionChanged();
}

KivioChangeRotationCommand::KivioChangeRotationCommand( const QString &_name,
    KivioPage *_page, KivioStencil * _stencil, int _oldValue, int _newValue)
    :KNamedCommand( _name),
    m_page(_page),
    m_stencil( _stencil ),
    oldValue( _oldValue),
    newValue( _newValue)
{
}

KivioChangeRotationCommand::~KivioChangeRotationCommand()
{
}

void KivioChangeRotationCommand::execute()
{
    m_stencil->setRotation( newValue );
    m_page->doc()->updateView(m_page);
    m_page->doc()->slotSelectionChanged();
}

void KivioChangeRotationCommand::unexecute()
{
    m_stencil->setRotation( oldValue );
    m_page->doc()->updateView(m_page);
    m_page->doc()->slotSelectionChanged();
}


KivioChangeLineStyleCommand::KivioChangeLineStyleCommand( const QString &_name,
    KivioPage *_page, KivioStencil * _stencil, int _oldValue, int _newValue)
    :KNamedCommand( _name),
    m_page(_page),
    m_stencil( _stencil ),
    oldValue( _oldValue),
    newValue( _newValue)
{
}

KivioChangeLineStyleCommand::~KivioChangeLineStyleCommand()
{
}

void KivioChangeLineStyleCommand::execute()
{
    m_stencil->setLinePattern( newValue );
    m_page->doc()->updateView(m_page);
    m_page->doc()->slotSelectionChanged();
}

void KivioChangeLineStyleCommand::unexecute()
{
    m_stencil->setLinePattern( oldValue );
    m_page->doc()->updateView(m_page);
    m_page->doc()->slotSelectionChanged();
}

KivioChangeBeginEndArrowCommand::KivioChangeBeginEndArrowCommand( const QString &_name, KivioPage *_page, KivioStencil * _stencil, int _oldArrow,  int _newArrow, bool _beginArrow)
    :KNamedCommand( _name),
     m_page(_page),
     m_stencil( _stencil ),
     oldArrow( _oldArrow),
     newArrow( _newArrow),
     beginArrow(_beginArrow)
{
}

KivioChangeBeginEndArrowCommand::~KivioChangeBeginEndArrowCommand()
{
}

void KivioChangeBeginEndArrowCommand::execute()
{
    if (beginArrow)
        m_stencil->setStartAHType(newArrow);
    else
        m_stencil->setEndAHType(newArrow);
    m_page->doc()->updateView(m_page);
    m_page->doc()->slotSelectionChanged();
}

void KivioChangeBeginEndArrowCommand::unexecute()
{
    if (beginArrow)
        m_stencil->setStartAHType(oldArrow);
    else
        m_stencil->setEndAHType(oldArrow);
    m_page->doc()->updateView(m_page);
    m_page->doc()->slotSelectionChanged();
}


KivioChangeBeginEndSizeArrowCommand::KivioChangeBeginEndSizeArrowCommand( const QString &_name, KivioPage *_page, KivioStencil * _stencil, float _oldH,float _oldW, float _newH,float _newW, bool _beginArrow)
    :KNamedCommand( _name),
     m_page(_page),
     m_stencil( _stencil ),
     oldWidth( _oldW),
     oldLength( _oldH),
     newWidth( _newW),
     newLength( _newH),
     beginArrow(_beginArrow)
{
}

KivioChangeBeginEndSizeArrowCommand::~KivioChangeBeginEndSizeArrowCommand()
{
}

void KivioChangeBeginEndSizeArrowCommand::execute()
{
    if (beginArrow)
    {
        m_stencil->setStartAHWidth(newWidth);
        m_stencil->setStartAHLength(newLength);
    }
    else
    {
        m_stencil->setEndAHWidth(newWidth);
        m_stencil->setEndAHLength(newLength);
    }
    m_page->doc()->updateView(m_page);
    m_page->doc()->slotSelectionChanged();
}

void KivioChangeBeginEndSizeArrowCommand::unexecute()
{
    if (beginArrow)
    {
        m_stencil->setStartAHWidth(oldWidth);
        m_stencil->setStartAHLength(oldLength);
    }
    else
    {
        m_stencil->setEndAHWidth(oldWidth);
        m_stencil->setEndAHLength(oldLength);
    }
    m_page->doc()->updateView(m_page);
    m_page->doc()->slotSelectionChanged();
}

KivioChangeStencilProtectCommand::KivioChangeStencilProtectCommand( const QString &_name, KivioPage *_page, KivioStencil * _stencil, bool _state, KVP_TYPE _type)
    :KNamedCommand( _name),
     m_page(_page),
     m_stencil( _stencil ),
     type( _type),
     m_bValue( _state)
{
}

void KivioChangeStencilProtectCommand::execute()
{
    changeValue( m_bValue );
}

void KivioChangeStencilProtectCommand::unexecute()
{
    changeValue( !m_bValue );
}

void KivioChangeStencilProtectCommand::changeValue( bool b )
{
    switch( type )
    {
    case KV_POSX:
	 m_stencil->protection()->setBit( kpX, b );
        break;
    case KV_POSY:
	 m_stencil->protection()->setBit( kpY, b );
        break;
    case KV_WIDTH:
	 m_stencil->protection()->setBit( kpWidth, b );
        break;
    case KV_HEIGHT:
        m_stencil->protection()->setBit( kpHeight, b );
        break;
    case KV_ASPECT:
        m_stencil->protection()->setBit( kpAspect, b );
        break;
    case KV_DELETE:
	 m_stencil->protection()->setBit( kpDeletion, b);
        break;
    default:
        break;
    }

    m_page->doc()->updateProtectPanelCheckBox();
    m_page->doc()->updateView(m_page);
}

KivioAddConnectorTargetCommand::KivioAddConnectorTargetCommand(const QString& name, KivioPage* page,
    KivioStencil* stencil, const KoPoint& targetPoint) : KNamedCommand(name)
{
  m_page = page;
  m_stencil = stencil;
  m_targetPoint = targetPoint;
}
void KivioAddConnectorTargetCommand::execute()
{
  m_stencil->addConnectorTarget(m_targetPoint);
  m_page->doc()->updateView(m_page);
}

void KivioAddConnectorTargetCommand::unexecute()
{
  m_stencil->removeConnectorTarget(m_targetPoint);
  m_page->doc()->updateView(m_page);
}


KivioCustomDragCommand::KivioCustomDragCommand(const QString& name, KivioPage* page, KivioStencil* stencil,
                                               int customID, const KoPoint& originalPoint, const KoPoint& newPoint)
  : KNamedCommand(name)
{
  m_page = page;
  m_stencil = stencil;
  m_customID = customID;
  m_originalPoint = originalPoint;
  m_newPoint = newPoint;
}

void KivioCustomDragCommand::execute()
{
  m_stencil->setCustomIDPoint(m_customID, m_newPoint, m_page);
  m_page->doc()->updateView(m_page);
}

void KivioCustomDragCommand::unexecute()
{
  m_stencil->setCustomIDPoint(m_customID, m_originalPoint, m_page);
  m_page->doc()->updateView(m_page);
}


KivioGroupCommand::KivioGroupCommand(const QString& name, KivioPage* page, KivioLayer* layer,
                                     KivioGroupStencil* group)
  : KNamedCommand(name)
{
  m_page = page;
  m_layer = layer;
  m_groupStencil = group;
}

void KivioGroupCommand::execute()
{
  QPtrList<KivioStencil>* list = m_groupStencil->groupList();
  QPtrListIterator<KivioStencil> it(*list);
  KivioStencil* stencil = 0;

  for(; (stencil = it.current()) != 0; ++it) {
    m_layer->takeStencil(stencil);
  }

  m_groupStencil->unselect();
  m_layer->addStencil(m_groupStencil);
  m_page->doc()->updateView(m_page);
}

void KivioGroupCommand::unexecute()
{
  QPtrList<KivioStencil>* list = m_groupStencil->groupList();
  QPtrListIterator<KivioStencil> it(*list);
  KivioStencil* stencil = 0;

  for(; (stencil = it.current()) != 0; ++it) {
    stencil->unselect();
    m_layer->addStencil(stencil);
  }

  m_layer->takeStencil(m_groupStencil);
  m_page->doc()->updateView(m_page);
}
