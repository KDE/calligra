/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <tktoolbarbutton.h>

#include <qimage.h>
#include <qtimer.h>
#include <qdrawutil.h>
#include <qtooltip.h>
#include <qbitmap.h>
#include <qpopupmenu.h>

#include <kapp.h>
#include <kglobal.h>
#include <kstyle.h>
#include <kglobalsettings.h>
#include <kiconeffect.h>
#include <kiconloader.h>

// needed to get our instance
#include <kmainwindow.h>

// Delay in ms before delayed popup pops up
#define POPUP_DELAY 500

class TKToolBarButton::TKToolBarButtonPrivate
{
public:
  TKToolBarButtonPrivate()
  {
    m_iconMode     = TK::IconOnly;
    m_isPopup      = false;
    m_isToggle     = false;
    m_isOn         = false;
    m_isRaised     = false;
    m_autoRaised   = true;

    m_text         = QString::null;
    m_iconName     = QString::null;
    m_arrowPressed = false;
    m_delayTimer   = 0L;
    m_popup        = 0L;

    m_disabledIconName = QString::null;
    m_defaultIconName  = QString::null;

    m_instance = KGlobal::instance();
  }

  ~TKToolBarButtonPrivate()
  {
    delete m_delayTimer;
    m_delayTimer = 0;
  }

  bool    m_isPopup;
  bool    m_isToggle;
  bool    m_isOn;
  bool    m_isRaised;
  bool    m_autoRaised;
  bool    m_arrowPressed;

  QString m_text;
  QString m_iconName;
  QString m_disabledIconName;
  QString m_defaultIconName;

  TK::IconMode m_iconMode;

  QTimer     *m_delayTimer;
  QPopupMenu *m_popup;

  KInstance  *m_instance;
};

TKToolBarButton::TKToolBarButton( const QString& icon, const QString& txt,
                                        QWidget* parent, const char* name,
                                        KInstance *instance )
: QButton(parent,name)
{
  d = new TKToolBarButtonPrivate;
  d->m_text = txt;
  d->m_instance = instance;

  setFocusPolicy( NoFocus );

  connect(this, SIGNAL(clicked()), SLOT(slotClicked()) );
  connect(this, SIGNAL(pressed()), SLOT(slotPressed()) );
  connect(this, SIGNAL(released()), SLOT(slotReleased()) );

  installEventFilter(this);

  setIcon(icon);
  modeChange();
}

TKToolBarButton::TKToolBarButton( const QPixmap& pixmap, const QString& txt, QWidget* parent, const char* name )
: QButton(parent,name )
{
  d = new TKToolBarButtonPrivate;
  d->m_text = txt;

  setFocusPolicy( NoFocus );

  connect(this, SIGNAL(clicked()), SLOT(slotClicked()) );
  connect(this, SIGNAL(pressed()), SLOT(slotPressed()) );
  connect(this, SIGNAL(released()), SLOT(slotReleased()) );

  installEventFilter(this);

  setPixmap(pixmap);
  modeChange();
}

TKToolBarButton::~TKToolBarButton()
{
  delete d;
}

QString TKToolBarButton::text()
{
  return d->m_text;
}

void TKToolBarButton::modeChange()
{
  QToolTip::add(this,d->m_text);

  int border = 3;
  int w = 2*border;
  int h = 2*border;

  if (pixmap()) {
    w += pixmap()->width();
    h = QMAX(h,pixmap()->height()+2*border);
    if (d->m_iconMode == TK::IconAndText && !d->m_text.isEmpty())
      w += border;
  }

  if ((d->m_iconMode==TK::IconAndText||d->m_iconMode==TK::TextOnly) && !d->m_text.isEmpty())
  {
    QFont tmp_font = KGlobalSettings::toolBarFont();
    QFontMetrics fm(tmp_font);

    h = QMAX(h,fm.lineSpacing()+2*border);
    w += fm.width(d->m_text);
  }

  if (d->m_popup && !d->m_isToggle)
    w += 11;

  QSize size(w,h);
  setMinimumSize(size);

  updateGeometry();
}

void TKToolBarButton::setEnabled( bool enabled )
{
  if (isEnabled()==enabled)
    return;

  QButton::setPixmap( (enabled ? defaultPixmap : disabledPixmap) );
  QButton::setEnabled( enabled );
}

void TKToolBarButton::setText( const QString& text)
{
  d->m_text = text;
  repaint(false);
}

void TKToolBarButton::setIcon( const QString& icon )
{
  d->m_iconName = icon;
  int iconSize = 16;

  setPixmap( BarIcon(icon, iconSize, KIcon::ActiveState, d->m_instance), false );
  setDisabledPixmap( BarIcon(icon, iconSize, KIcon::DisabledState, d->m_instance) );
  setDefaultPixmap( BarIcon(icon, iconSize, KIcon::DefaultState, d->m_instance) );
}

void TKToolBarButton::setDisabledIcon( const QString &icon )
{
  d->m_disabledIconName = icon;
  int iconSize = 16;
  setDisabledPixmap( BarIcon(icon, iconSize, KIcon::DisabledState, d->m_instance) );
}

void TKToolBarButton::setDefaultIcon( const QString &icon )
{
  d->m_defaultIconName = icon;
  int iconSize = 16;
  setDefaultPixmap( BarIcon(icon, iconSize, KIcon::DefaultState, d->m_instance) );
}

void TKToolBarButton::setPixmap( const QPixmap &pixmap )
{
  setPixmap( pixmap, true );
}

void TKToolBarButton::setPixmap( const QPixmap &pixmap, bool generate )
{
  activePixmap = pixmap;

  if ( generate )
  {
    makeDefaultPixmap();
    makeDisabledPixmap();
  }
  else
  {
    if (defaultPixmap.isNull())
      defaultPixmap = activePixmap;
    if (disabledPixmap.isNull())
      disabledPixmap = activePixmap;
  }

  QButton::setPixmap( isEnabled() ? defaultPixmap : disabledPixmap );
}

void TKToolBarButton::setDefaultPixmap( const QPixmap &pixmap )
{
  defaultPixmap = pixmap;
  QButton::setPixmap( isEnabled() ? defaultPixmap : disabledPixmap );
}

void TKToolBarButton::setDisabledPixmap( const QPixmap &pixmap )
{
  disabledPixmap = pixmap;
  QButton::setPixmap( isEnabled() ? defaultPixmap : disabledPixmap );
}

void TKToolBarButton::setPopup(QPopupMenu *p)
{
  d->m_popup = p;
  d->m_popup->setFont(KGlobalSettings::toolBarFont());
  p->installEventFilter(this);

  modeChange();
}

QPopupMenu *TKToolBarButton::popup()
{
  return d->m_popup;
}

void TKToolBarButton::setDelayedPopup (QPopupMenu *p, bool toggle )
{
  d->m_isPopup = true;
  setToggle(toggle);

  if (!d->m_delayTimer) {
    d->m_delayTimer = new QTimer(this);
    connect(d->m_delayTimer, SIGNAL(timeout()), this, SLOT(slotDelayTimeout()));
  }

  setPopup(p);
}

void TKToolBarButton::setRaised(bool f)
{
  d->m_isRaised = f;
  repaint(false);
}

void TKToolBarButton::setAutoRaised(bool f)
{
  d->m_autoRaised = f;
}

void TKToolBarButton::leaveEvent(QEvent *)
{
  if (!d->m_isToggle && !(d->m_popup && d->m_popup->isVisible()) ) {
    QButton::setPixmap(isEnabled() ? defaultPixmap : disabledPixmap);
    if (d->m_autoRaised)
      setRaised(false);
  }
}

void TKToolBarButton::enterEvent(QEvent *)
{
  if (!d->m_isToggle) {
    if (isEnabled()) {
      QButton::setPixmap(activePixmap);
      if (d->m_autoRaised)
        setRaised(true);
    } else {
      QButton::setPixmap(disabledPixmap);
    }
    repaint(false);
  }
}

bool TKToolBarButton::eventFilter(QObject *o, QEvent *ev)
{
  if ( o == this )
    if (ev->type() == QEvent::MouseButtonPress && d->m_popup && d->m_isPopup ) {
      if (!d->m_isToggle) {
        int x = mapFromGlobal(QCursor::pos()).x();
        d->m_arrowPressed = x > width() - 12;
      } else {
        d->m_delayTimer->start(POPUP_DELAY);
      }
    }

  if ( o == d->m_popup) {
    switch (ev->type())
    {
      case QEvent::Show:
        on(true);
        return false;
      case QEvent::Hide:
        on(false);
        setDown(false);
        if ( !geometry().contains(parentWidget()->mapFromGlobal(QCursor::pos())) )
          leaveEvent(0L);
        return false;
        break;
      default:
        break;
    }
  }
  return false;
}

void TKToolBarButton::drawButton( QPainter* p )
{
#define DRAW_PIXMAP_AND_TEXT \
  int x = 3;\
  if (pixmap()) {\
    style().drawItem( p, x, 0, pixmap()->width(), height(), AlignCenter, colorGroup(), isEnabled(), pixmap(), QString::null );\
    if (d->m_iconMode==TK::IconAndText && !d->m_text.isEmpty()) {\
      x += pixmap()->width() + 3;\
    }\
  }\
  if ((d->m_iconMode==TK::IconAndText||d->m_iconMode==TK::TextOnly) && !d->m_text.isEmpty()) {\
    QFontMetrics fm(ref_font);\
    style().drawItem( p, x, 0, fm.width(d->m_text), height(), AlignCenter, colorGroup(), isEnabled(), 0, d->m_text );\
  }

  const char* arrow[] = {
  "7 4 2 1",
  "# c Black",
  ". c None",
  "#######",
  ".#####.",
  "..###..",
  "...#..."};
  QPixmap arrow_pix(arrow);

  KStyle::KToolButtonType iconType = d->m_iconMode != TK::IconOnly ? KStyle::IconTextRight : KStyle::Icon;
  QFont ref_font(KGlobalSettings::toolBarFont());
  bool f = d->m_isOn || isDown();

  if (d->m_popup && !d->m_isToggle)
  {
    if (d->m_isPopup)
    {
      if (kapp->kstyle()) {
        kapp->kstyle()->drawKToolBarButton(p, 0, 0, width()-12, height(),
          isEnabled()? colorGroup() : palette().disabled(), f && !d->m_arrowPressed,
          d->m_isRaised, isEnabled(), false, iconType, d->m_text,
          pixmap(), &ref_font, this);
        kapp->kstyle()->drawKToolBarButton(p, width()-13, 0, 13, height(),
          isEnabled()? colorGroup() : palette().disabled(), f && d->m_arrowPressed,
          d->m_isRaised, isEnabled(), false, KStyle::Icon, "",
          &arrow_pix, &ref_font, this);
      } else {
        style().drawToolButton( p, 0, 0, width()-12, height(), isEnabled() ? colorGroup() : palette().disabled(), f && !d->m_arrowPressed );

        style().drawToolButton( p, width()-13, 0, 13, height(), isEnabled() ? colorGroup() : palette().disabled(), f && d->m_arrowPressed );
        style().drawItem( p, width()-13, 0, 13, height(), AlignCenter, colorGroup(), isEnabled(), &arrow_pix, QString::null );
        DRAW_PIXMAP_AND_TEXT
      }
    } else {
      if (kapp->kstyle()) {
        kapp->kstyle()->drawKToolBarButton(p, 0, 0, width(), height(),
          isEnabled()? colorGroup() : palette().disabled(), f,
          d->m_isRaised, isEnabled(), false, KStyle::IconTextRight, d->m_iconMode != TK::IconOnly ? d->m_text:QString(""),
          pixmap(), &ref_font, this);
      } else {
        style().drawToolButton( p, 0, 0, width(), height(), isEnabled() ? colorGroup() : palette().disabled(), f );
        DRAW_PIXMAP_AND_TEXT
      }
      int z = f ? 1:0;
      p->drawPixmap(width()-11+z,(height()-4)/2+z ,arrow_pix);
    }
  } else {
    if (kapp->kstyle()) {
      kapp->kstyle()->drawKToolBarButton(p, 0, 0, width(), height(),
        isEnabled()? colorGroup() : palette().disabled(), f,
        d->m_isRaised, isEnabled(), d->m_popup, iconType, d->m_text,
        pixmap(), &ref_font, this);
    } else {
      style().drawToolButton( p, 0, 0, width(), height(), isEnabled() ? colorGroup() : palette().disabled(), f );
      DRAW_PIXMAP_AND_TEXT
    }
  }
}

void TKToolBarButton::paletteChange(const QPalette &)
{
  makeDisabledPixmap();
  if ( !isEnabled() )
    QButton::setPixmap( disabledPixmap );
  else
    QButton::setPixmap( defaultPixmap );
  repaint(false);
}

void TKToolBarButton::makeDefaultPixmap()
{
  if (activePixmap.isNull())
    return;

  KIconEffect effect;
  defaultPixmap = effect.apply(activePixmap, KIcon::Toolbar, KIcon::DefaultState);
}

void TKToolBarButton::makeDisabledPixmap()
{
  if (activePixmap.isNull())
    return;

  KIconEffect effect;
  disabledPixmap = effect.apply(activePixmap, KIcon::Toolbar, KIcon::DisabledState);
}

void TKToolBarButton::showMenu()
{
  QPoint p ( mapToGlobal( QPoint( 0, 0 ) ) );
  if ( p.y() + height() + d->m_popup->height() > KApplication::desktop()->height() )
      p.setY( p.y() - d->m_popup->height() );
  else
      p.setY( p.y() + height( ));

  d->m_popup->popup(p);
}

void TKToolBarButton::slotDelayTimeout()
{
  d->m_delayTimer->stop();
  showMenu();
}

void TKToolBarButton::slotClicked()
{
  if (d->m_popup && !d->m_isPopup)
    showMenu();
  else
    emit buttonClicked();
}

void TKToolBarButton::slotPressed()
{
  if (d->m_popup) {
    if (d->m_isPopup) {
      if (d->m_arrowPressed)
        showMenu();
    } else {
      showMenu();
    }
  } else {
    emit buttonPressed();
  }
}

void TKToolBarButton::slotReleased()
{
  if (d->m_popup && d->m_isPopup)
    d->m_delayTimer->stop();

  emit buttonReleased();
}

void TKToolBarButton::slotToggled()
{
  emit buttonToggled();
}

void TKToolBarButton::on(bool flag)
{
  d->m_isOn = flag;
  repaint();
}

void TKToolBarButton::toggle()
{
  on(!d->m_isOn);
}

void TKToolBarButton::setToggle(bool flag)
{
  d->m_isToggle = flag;
  if (flag == true)
    connect(this, SIGNAL(toggled(bool)), this, SLOT(slotToggled()));
  else
    disconnect(this, SIGNAL(toggled(bool)), this, SLOT(slotToggled()));

  modeChange();
  repaint();
}

void TKToolBarButton::setIconMode( TK::IconMode m )
{
  d->m_iconMode = m;
  modeChange();
  repaint();
}

#include <tktoolbarbutton.moc>
