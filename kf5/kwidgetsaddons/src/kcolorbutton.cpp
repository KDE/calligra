/*  This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)
    Copyright (C) 1999 Cristian Tibirna (ctibirna@kde.org)

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
    Boston, MA 02110-1301, USA.
*/

#include "kcolorbutton.h"

#include <QtCore/QPointer>
#include <QPainter>
#include <qdrawutil.h>
#include <QApplication>
#include <QColorDialog>
#include <QClipboard>
#include <QMimeData>
#include <QDrag>
#include <QStyle>
#include <QMouseEvent>
#include <QStyleOptionButton>

class KColorButton::KColorButtonPrivate
{
public:
    KColorButtonPrivate(KColorButton *q);

    void _k_chooseColor();
    void _k_colorChosen();

    KColorButton *q;
    QColor m_defaultColor;
    bool m_bdefaultColor : 1;
    bool m_alphaChannel : 1;

    QColor col;
    QPoint mPos;

    QPointer<QColorDialog> dialogPtr;

    void initStyleOption(QStyleOptionButton* opt) const;    
};

/////////////////////////////////////////////////////////////////////
// Functions duplicated from KColorMimeData
// Should be kept in sync
void _k_populateMimeData(QMimeData *mimeData, const QColor &color)
{
    mimeData->setColorData(color);
    mimeData->setText(color.name());
}

bool _k_canDecode(const QMimeData *mimeData)
{
    if (mimeData->hasColor())
        return true;
    if (mimeData->hasText()) {
        const QString colorName=mimeData->text();
        if ((colorName.length() >= 4) && (colorName[0] == QLatin1Char('#')))
            return true;
    }
    return false;
}

QColor _k_fromMimeData(const QMimeData *mimeData)
{
    if (mimeData->hasColor())
        return mimeData->colorData().value<QColor>();
    if (_k_canDecode(mimeData))
        return QColor(mimeData->text());
    return QColor();
}


QDrag *_k_createDrag(const QColor &color, QObject *dragsource)
{
    QDrag *drag = new QDrag(dragsource);
    QMimeData *mime = new QMimeData;
    _k_populateMimeData(mime, color);
    drag->setMimeData(mime);
    QPixmap colorpix( 25, 20 );
    colorpix.fill( color );
    QPainter p( &colorpix );
    p.setPen( Qt::black );
    p.drawRect(0,0,24,19);
    p.end();
    drag->setPixmap(colorpix);
    drag->setHotSpot(QPoint(-5,-7));
    return drag;
}
/////////////////////////////////////////////////////////////////////


KColorButton::KColorButtonPrivate::KColorButtonPrivate(KColorButton *q)
    : q(q)
{
  m_bdefaultColor = false;
  m_alphaChannel = false;
  q->setAcceptDrops(true);

  connect(q, SIGNAL(clicked()), q, SLOT(_k_chooseColor()));
}

KColorButton::KColorButton( QWidget *parent )
  : QPushButton( parent )
  , d( new KColorButtonPrivate(this) )
{
}

KColorButton::KColorButton( const QColor &c, QWidget *parent )
  : QPushButton( parent )
  , d( new KColorButtonPrivate(this) )
{
  d->col = c;
}

KColorButton::KColorButton( const QColor &c, const QColor &defaultColor, QWidget *parent )
  : QPushButton( parent )
  , d( new KColorButtonPrivate(this) )
{
  d->col = c;
  setDefaultColor(defaultColor);
}

KColorButton::~KColorButton()
{
  delete d;
}

QColor KColorButton::color() const
{
  return d->col;
}

void KColorButton::setColor( const QColor &c )
{
  if ( d->col != c ) {
    d->col = c;
    update();
    emit changed( d->col );
  }
}

void KColorButton::setAlphaChannelEnabled( bool alpha )
{
    d->m_alphaChannel = alpha;
}

bool KColorButton::isAlphaChannelEnabled() const
{
    return d->m_alphaChannel;
}

QColor KColorButton::defaultColor() const
{
  return d->m_defaultColor;
}

void KColorButton::setDefaultColor( const QColor &c )
{
  d->m_bdefaultColor = c.isValid();
  d->m_defaultColor = c;
}

void KColorButton::KColorButtonPrivate::initStyleOption(QStyleOptionButton* opt) const
{
    opt->initFrom(q);
    opt->state |= q->isDown() ? QStyle::State_Sunken : QStyle::State_Raised;
    opt->features = QStyleOptionButton::None;
    if (q->isDefault())
      opt->features |= QStyleOptionButton::DefaultButton;
    opt->text.clear();
    opt->icon = QIcon();
}

void KColorButton::paintEvent( QPaintEvent* )
{
  QPainter painter(this);
  QStyle *style = QWidget::style();

  //First, we need to draw the bevel.
  QStyleOptionButton butOpt;
  d->initStyleOption(&butOpt);
  style->drawControl( QStyle::CE_PushButtonBevel, &butOpt, &painter, this );

  //OK, now we can muck around with drawing out pretty little color box
  //First, sort out where it goes
  QRect labelRect = style->subElementRect( QStyle::SE_PushButtonContents,
      &butOpt, this );
  int shift = style->pixelMetric( QStyle::PM_ButtonMargin, &butOpt, this ) / 2;
  labelRect.adjust(shift, shift, -shift, -shift);
  int x, y, w, h;
  labelRect.getRect(&x, &y, &w, &h);

  if (isChecked() || isDown()) {
    x += style->pixelMetric( QStyle::PM_ButtonShiftHorizontal, &butOpt, this );
    y += style->pixelMetric( QStyle::PM_ButtonShiftVertical, &butOpt, this );
  }

  QColor fillCol = isEnabled() ? d->col : palette().color(backgroundRole());
  qDrawShadePanel( &painter, x, y, w, h, palette(), true, 1, NULL);
  if ( fillCol.isValid() ) {
    const QRect rect(x+1, y+1, w-2, h-2);
    if (fillCol.alpha() < 255) {
        QPixmap chessboardPattern(16, 16);
        QPainter patternPainter(&chessboardPattern);
        patternPainter.fillRect(0, 0, 8, 8, Qt::black);
        patternPainter.fillRect(8, 8, 8, 8, Qt::black);
        patternPainter.fillRect(0, 8, 8, 8, Qt::white);
        patternPainter.fillRect(8, 0, 8, 8, Qt::white);
        patternPainter.end();
        painter.fillRect(rect, QBrush(chessboardPattern));
    }
    painter.fillRect(rect, fillCol);
  }

  if ( hasFocus() ) {
    QRect focusRect = style->subElementRect( QStyle::SE_PushButtonFocusRect, &butOpt, this );
    QStyleOptionFocusRect focusOpt;
    focusOpt.init(this);
    focusOpt.rect            = focusRect;
    focusOpt.backgroundColor = palette().background().color();
    style->drawPrimitive( QStyle::PE_FrameFocusRect, &focusOpt, &painter, this );
  }
}

QSize KColorButton::sizeHint() const
{
    QStyleOptionButton opt;
    d->initStyleOption(&opt);
    return style()->sizeFromContents(QStyle::CT_PushButton, &opt, QSize(40, 15), this).
	  	expandedTo(QApplication::globalStrut());
}

QSize KColorButton::minimumSizeHint() const
{
    QStyleOptionButton opt;
    d->initStyleOption(&opt);
    return style()->sizeFromContents(QStyle::CT_PushButton, &opt, QSize(3, 3), this).
	  	expandedTo(QApplication::globalStrut());
}

void KColorButton::dragEnterEvent( QDragEnterEvent *event)
{
  event->setAccepted(_k_canDecode(event->mimeData()) && isEnabled());
}

void KColorButton::dropEvent( QDropEvent *event)
{
  QColor c = _k_fromMimeData(event->mimeData());
  if (c.isValid()) {
    setColor(c);
  }
}

void KColorButton::keyPressEvent( QKeyEvent *e )
{
  int key = e->key() | e->modifiers();

  if (QKeySequence::keyBindings(QKeySequence::Copy).contains(key)) {
    QMimeData *mime=new QMimeData;
    _k_populateMimeData(mime, color());
    QApplication::clipboard()->setMimeData( mime, QClipboard::Clipboard );
  } else if (QKeySequence::keyBindings(QKeySequence::Paste).contains(key)) {
    QColor color = _k_fromMimeData(QApplication::clipboard()->mimeData(QClipboard::Clipboard));
    setColor( color );
  }
  else
    QPushButton::keyPressEvent( e );
}

void KColorButton::mousePressEvent( QMouseEvent *e)
{
  d->mPos = e->pos();
  QPushButton::mousePressEvent(e);
}

void KColorButton::mouseMoveEvent( QMouseEvent *e)
{
  if( (e->buttons() & Qt::LeftButton) &&
    (e->pos()-d->mPos).manhattanLength() > QApplication::startDragDistance() )
  {
    _k_createDrag(color(), this)->start();
    setDown(false);
  }
}

void KColorButton::KColorButtonPrivate::_k_chooseColor()
{
    QColorDialog *dialog = dialogPtr.data();
    if (dialog) {
        dialog->show();
        dialog->raise();
        dialog->activateWindow();
        return;
    }

    dialog = new QColorDialog(q);
    dialog->setCurrentColor(q->color());
    dialog->setOption(QColorDialog::ShowAlphaChannel, m_alphaChannel);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, SIGNAL(accepted()), q, SLOT(_k_colorChosen()));
    dialogPtr = dialog;
    dialog->show();
}

void KColorButton::KColorButtonPrivate::_k_colorChosen()
{
    QColorDialog *dialog = dialogPtr.data();
    if (!dialog) {
        return;
    }

    if (dialog->selectedColor().isValid()) {
        q->setColor(dialog->selectedColor());
    } else if (m_bdefaultColor) {
        q->setColor(m_defaultColor);
    }
}

#include "moc_kcolorbutton.cpp"
