/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2003 theKompany.com & Dave Marotti,
 *                         Peter Simonsson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "tool_text.h"

#include <qcursor.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <KoPoint.h>
#include <klocale.h>
#include <KoZoomHandler.h>
#include <kinputdialog.h>

#include "kivio_view.h"
#include "kivio_canvas.h"
#include "kivio_page.h"
#include "kivio_doc.h"

#include "kivio_stencil_spawner_set.h"
#include "kivio_stencil_spawner.h"
#include "kivio_custom_drag_data.h"
#include "kivio_layer.h"
#include "kivio_point.h"
#include "kivio_stencil.h"
#include "kivio_factory.h"
#include "kivio_command.h"
#include "kivio_pluginmanager.h"
#include "mousetoolaction.h"
#include "stenciltexteditor.h"


TextTool::TextTool( KivioView* parent ) : Kivio::MouseTool(parent, "Text Mouse Tool")
{
  m_textAction = new Kivio::MouseToolAction( i18n("Text Tool"), "text", Key_F2, actionCollection(), "text" );
  connect(m_textAction, SIGNAL(toggled(bool)), this, SLOT(setActivated(bool)));
  connect(m_textAction, SIGNAL(doubleClicked()), this, SLOT(makePermanent()));

  m_permanent = false;
  m_mode = stmNone;

  QPixmap pix = BarIcon("kivio_text_cursor",KivioFactory::global());
  m_pTextCursor = new QCursor(pix,2,2);
}

TextTool::~TextTool()
{
  delete m_pTextCursor;
}


/**
 * Event delegation
 *
 * @param e The event to be identified and processed
 *
 */
bool TextTool::processEvent(QEvent* e)
{
  switch (e->type())
  {
  case QEvent::MouseButtonPress:
    mousePress( (QMouseEvent*)e );
    return true;
    break;

  case QEvent::MouseButtonRelease:
    mouseRelease( (QMouseEvent*)e );
    return true;
    break;

  case QEvent::MouseMove:
    mouseMove( (QMouseEvent*)e );
    return true;
    break;

  default:
    break;
  }

  return false;
}

void TextTool::setActivated(bool a)
{
  if(a) {
    emit activated(this);
    m_textAction->setChecked(true);
    view()->canvasWidget()->setCursor(*m_pTextCursor);
    m_mode = stmNone;
  } else {
    m_textAction->setChecked(false);
    m_permanent = false;
  }
}

void TextTool::text(QRect r)
{
  // Calculate the start and end clicks in terms of page coordinates
  KoPoint startPoint = view()->canvasWidget()->mapFromScreen( QPoint( r.x(), r.y() ) );
  KoPoint releasePoint = view()->canvasWidget()->mapFromScreen( QPoint( r.x() + r.width(), r.y() + r.height() ) );

  // Calculate the x,y position of the textion box
  float x = startPoint.x() < releasePoint.x() ? startPoint.x() : releasePoint.x();
  float y = startPoint.y() < releasePoint.y() ? startPoint.y() : releasePoint.y();

  // Calculate the w/h of the textion box
  float w = releasePoint.x() - startPoint.x();

  if( w < 0.0 ) {
    w *= -1.0;
  }

  float h = releasePoint.y() - startPoint.y();

  if( h < 0.0 ) {
    h *= -1.0;
  }

  KivioDoc* doc = view()->doc();
  KivioPage* page = view()->activePage();

  KivioStencilSpawner* ss = doc->findInternalStencilSpawner("Dave Marotti - Text");

  if (!ss) {
    return;
  }

  KivioStencil* stencil = ss->newStencil();
  stencil->setType(kstText);
  stencil->setPosition(x,y);
  stencil->setDimensions(w,h);
  stencil->setText("");
  stencil->setTextFont(doc->defaultFont());
  page->unselectAllStencils();
  page->addStencil(stencil);
  page->selectStencil(stencil);

  doc->updateView(page);

  applyToolAction(page->selectedStencils());

  if (stencil->text().isEmpty()) {
    page->deleteSelectedStencils();
    doc->updateView(page);
  }
}

void TextTool::mousePress( QMouseEvent *e )
{
  if(e->button() == LeftButton) {
    KoPoint pagePoint = view()->canvasWidget()->mapFromScreen(e->pos());
    int colType;
    KivioPage *page = view()->activePage();
    KivioStencil* stencil = page->checkForStencil( &pagePoint, &colType, 0.0, false);
    
    if(stencil) {
      applyToolAction(stencil, pagePoint);
    } else if(startRubberBanding(e)) {
      m_mode = stmDrawRubber;
    }
  }
}


/**
 * Tests if we should start rubber banding (always returns true).
 */
bool TextTool::startRubberBanding( QMouseEvent *e )
{
  view()->canvasWidget()->startRectDraw( e->pos(), KivioCanvas::Rubber );
  view()->canvasWidget()->repaint();
  m_startPoint = e->pos();

  return true;
}

void TextTool::mouseMove( QMouseEvent * e )
{
  switch( m_mode )
  {
    case stmDrawRubber:
      continueRubberBanding(e);
      break;

    default:
      break;
  }
}

void TextTool::continueRubberBanding( QMouseEvent *e )
{
  view()->canvasWidget()->continueRectDraw( e->pos(), KivioCanvas::Rubber );
}

void TextTool::mouseRelease( QMouseEvent *e )
{
  m_releasePoint = e->pos();

  switch( m_mode )
  {
    case stmDrawRubber:
      endRubberBanding(e);
      break;
  }

  m_mode = stmNone;

  view()->canvasWidget()->repaint();
}

void TextTool::endRubberBanding(QMouseEvent */*e*/)
{
  // End the rubber-band drawing
  view()->canvasWidget()->endRectDraw();
  QRect rect;

  if( m_startPoint != m_releasePoint ) {
    rect = view()->canvasWidget()->rect();
  } else { // Behave a bit more sensible when clicking the canvas...
    rect.setTopLeft(m_startPoint);
    rect.setWidth(view()->zoomHandler()->zoomItX(100));
    rect.setHeight(view()->zoomHandler()->zoomItY(20));
  }

  text(rect);

  if(!m_permanent) {
    view()->pluginManager()->activateDefaultTool();
  }
}

void TextTool::applyToolAction(QPtrList<KivioStencil>* stencils)
{
  if(stencils->isEmpty()) {
    return;
  }

  KivioStencil* stencil = stencils->first();
  bool ok = false;

  while(stencil) {
    if(stencil->hasTextBox()) {
      ok = true;
      break;
    }

    stencil = stencils->next();
  }

  if(!ok) {
    return;
  }

  Kivio::StencilTextEditor editor(i18n("Edit Text"), view(), "StencilTextEditor");
  editor.setFont(stencil->textFont());
  editor.setFontColor(stencil->textColor());
  editor.setBackgroundColor(stencil->bgColor());
  editor.setText(stencil->text());
  editor.setHorizontalAlign(static_cast<Qt::AlignmentFlags>(stencil->hTextAlign()));
  editor.setVerticalAlign(static_cast<Qt::AlignmentFlags>(stencil->vTextAlign()));

  if(editor.exec() == QDialog::Accepted) {
    KMacroCommand* macroCmd = new KMacroCommand(i18n("Change Stencil Text and Formatting"));
    bool changed = false;
    QString text = editor.text();
    QFont font = editor.font();
    QColor textColor = editor.fontColor();
    int halignment = editor.horizontalAlignment();
    int valignment = editor.verticalAlignment();
    bool changeFont = (stencil->textFont() != font);
    bool changeTextColor = (stencil->textColor() != textColor);
    bool changeHAlignment = (stencil->hTextAlign() != halignment);
    bool changeVAlignment = (stencil->vTextAlign() != valignment);


    while( stencil )
    {
      if(stencil->text() != text)
      {
        KivioChangeStencilTextCommand *cmd = new KivioChangeStencilTextCommand(i18n("Change Stencil Text"),
            stencil, stencil->text(), text, view()->activePage());
        stencil->setText(text);
        macroCmd->addCommand(cmd);
        changed = true;
      }

      if(changeFont && (stencil->textFont() != font)) {
        KivioChangeStencilFontCommand* cmd = new KivioChangeStencilFontCommand(i18n("Change Stencil Font"),
            view()->activePage(), stencil, stencil->textFont(), font);
        stencil->setTextFont(font);
        macroCmd->addCommand(cmd);
        changed = true;
      }

      if(changeTextColor && (stencil->textColor() != textColor)) {
        KivioChangeStencilColorCommand* cmd = new KivioChangeStencilColorCommand(i18n("Change Stencil Text Color"),
            view()->activePage(), stencil, stencil->textColor(), textColor, KivioChangeStencilColorCommand::CT_TEXTCOLOR);
        stencil->setTextColor(textColor);
        macroCmd->addCommand(cmd);
        changed = true;
      }

      if(changeHAlignment && (stencil->hTextAlign() != halignment)) {
        KivioChangeStencilHAlignmentCommand* cmd = new KivioChangeStencilHAlignmentCommand(
            i18n("Change Stencil Horizontal Alignment"), view()->activePage(), stencil, stencil->hTextAlign(), halignment);
        stencil->setHTextAlign(halignment);
        macroCmd->addCommand(cmd);
        changed = true;
      }

      if(changeVAlignment && (stencil->vTextAlign() != valignment)) {
        KivioChangeStencilVAlignmentCommand* cmd = new KivioChangeStencilVAlignmentCommand(
            i18n("Change Stencil Vertical Alignment"), view()->activePage(), stencil, stencil->vTextAlign(), valignment);
        stencil->setVTextAlign(valignment);
        macroCmd->addCommand(cmd);
        changed = true;
      }

      stencil = stencils->next();
    }

    if(changed) {
      view()->doc()->addCommand(macroCmd);
      view()->doc()->updateView(view()->activePage());
    } else {
      delete macroCmd;
    }
  }
}

void TextTool::applyToolAction(KivioStencil* stencil, const KoPoint& pos)
{
  if(!stencil) {
    return;
  }

  QString name = stencil->getTextBoxName(pos);

  if(name.isEmpty()) {
    return;
  }

  Kivio::StencilTextEditor editor(i18n("Edit Text"), view(), "StencilTextEditor");
  editor.setFont(stencil->textFont(name));
  editor.setFontColor(stencil->textColor(name));
  editor.setBackgroundColor(stencil->bgColor());
  editor.setText(stencil->text(name));
  editor.setHorizontalAlign(static_cast<Qt::AlignmentFlags>(stencil->hTextAlign(name)));
  editor.setVerticalAlign(static_cast<Qt::AlignmentFlags>(stencil->vTextAlign(name)));

  if(editor.exec() == QDialog::Accepted) {
    KMacroCommand* macroCmd = new KMacroCommand(i18n("Change Stencil Text and Formatting"));
    bool changed = false;
    QString text = editor.text();

    if(stencil->text(name) != text)
    {
      KivioChangeStencilTextCommand *cmd = new KivioChangeStencilTextCommand(i18n("Change Stencil Text"),
          stencil, stencil->text(name), text, view()->activePage(), name);
      stencil->setText(text, name);
      macroCmd->addCommand(cmd);
      changed = true;
    }

    QFont font = editor.font();

    if(stencil->textFont(name) != font) {
      KivioChangeStencilFontCommand* cmd = new KivioChangeStencilFontCommand(i18n("Change Stencil Font"),
          view()->activePage(), stencil, stencil->textFont(name), font, name);
      stencil->setTextFont(name, font);
      macroCmd->addCommand(cmd);
      changed = true;
    }

    QColor textColor = editor.fontColor();

    if(stencil->textColor(name) != textColor) {
      KivioChangeStencilColorCommand* cmd = new KivioChangeStencilColorCommand(i18n("Change Stencil Text Color"),
          view()->activePage(), stencil, stencil->textColor(name), textColor,
          KivioChangeStencilColorCommand::CT_TEXTCOLOR, name);
      stencil->setTextColor(name, textColor);
      macroCmd->addCommand(cmd);
      changed = true;
    }

    int halignment = editor.horizontalAlignment();

    if(stencil->hTextAlign(name) != halignment) {
      KivioChangeStencilHAlignmentCommand* cmd = new KivioChangeStencilHAlignmentCommand(
          i18n("Change Stencil Horizontal Alignment"), view()->activePage(), stencil,
          stencil->hTextAlign(name), halignment, name);
      stencil->setHTextAlign(name, halignment);
      macroCmd->addCommand(cmd);
      changed = true;
    }

    int valignment = editor.verticalAlignment();

    if(stencil->vTextAlign(name) != valignment) {
      KivioChangeStencilVAlignmentCommand* cmd = new KivioChangeStencilVAlignmentCommand(
          i18n("Change Stencil Vertical Alignment"), view()->activePage(), stencil,
      stencil->vTextAlign(name), valignment, name);
      stencil->setVTextAlign(name, valignment);
      macroCmd->addCommand(cmd);
      changed = true;
    }

    if(changed) {
      view()->doc()->addCommand(macroCmd);
      view()->doc()->updateView(view()->activePage());
    } else {
      delete macroCmd;
    }
  }
}

void TextTool::makePermanent()
{
  m_permanent = true;
}

#include "tool_text.moc"
