/*
    Copyright (C) 2003 Nadeem Hasan <nhasan@kde.org>

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

#include "kfontrequester.h"
#include "fonthelpers_p.h"

#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QFontDatabase>
#include <QFontDialog>
#include <QLocale>

#include <kfontchooser.h>

#include <cmath>

// Determine if the font with given properties is available on the system,
// otherwise find and return the best fitting combination.
static QFont nearestExistingFont (const QFont &font)
{
    QFontDatabase dbase;

    // Initialize font data accoring to given font object.
    QString family = font.family();
    QString style = dbase.styleString(font);
    qreal size = font.pointSizeF();

    // Check if the family exists.
    const QStringList families = dbase.families();
    if (!families.contains(family)) {
        // Chose another family.
        family = families.count() ? families[0] : QLatin1String("fixed");
        // TODO: Try to find nearest match?
    }

    // Check if the family has the requested style.
    // Easiest by piping it through font selection in the database.
    QString retStyle = dbase.styleString(dbase.font(family, style, 10));
    style = retStyle;

    // Check if the family has the requested size.
    // Only for bitmap fonts.
    if (!dbase.isSmoothlyScalable(family, style)) {
        QList<int> sizes = dbase.smoothSizes(family, style);
        if (!sizes.contains(size)) {
            // Find nearest available size.
            int mindiff = 1000;
            int refsize = size;
            Q_FOREACH (int lsize, sizes) {
                int diff = qAbs(refsize - lsize);
                if (mindiff > diff) {
                    mindiff = diff;
                    size = lsize;
                }
            }
        }
    }

    // Select the font with confirmed properties.
    QFont result = dbase.font(family, style, int(size));
    if (dbase.isSmoothlyScalable(family, style) && result.pointSize() == floor(size)) {
        result.setPointSizeF(size);
    }
    return result;
}

class KFontRequester::KFontRequesterPrivate
{
public:
  KFontRequesterPrivate(KFontRequester *q): q(q) {}

  void displaySampleText();
  void setToolTip();

  void _k_buttonClicked();

  KFontRequester *q;
  bool m_onlyFixed;
  QString m_sampleText, m_title;
  QLabel *m_sampleLabel;
  QPushButton *m_button;
  QFont m_selFont;
};

KFontRequester::KFontRequester( QWidget *parent, bool onlyFixed )
    : QWidget( parent ), d(new KFontRequesterPrivate(this))
{
  d->m_onlyFixed = onlyFixed;

  QHBoxLayout *layout = new QHBoxLayout( this );
  layout->setMargin( 0 );

  d->m_sampleLabel = new QLabel( this );
  d->m_button = new QPushButton( tr( "Choose..." ), this );

  d->m_sampleLabel->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  setFocusProxy( d->m_button );

  layout->addWidget( d->m_sampleLabel, 1 );
  layout->addWidget( d->m_button );

  connect( d->m_button, SIGNAL(clicked()), SLOT(_k_buttonClicked()) );

  d->displaySampleText();
  d->setToolTip();
}

KFontRequester::~KFontRequester()
{
  delete d;
}

QFont KFontRequester::font() const
{
  return d->m_selFont;
}

bool KFontRequester::isFixedOnly() const
{
  return d->m_onlyFixed;
}

QString KFontRequester::sampleText() const
{
  return d->m_sampleText;
}

QString KFontRequester::title() const
{
  return d->m_title;
}

QLabel *KFontRequester::label() const
{
  return d->m_sampleLabel;
}

QPushButton *KFontRequester::button() const
{
  return d->m_button;
}

void KFontRequester::setFont( const QFont &font, bool onlyFixed )
{
  d->m_selFont = nearestExistingFont(font);
  d->m_onlyFixed = onlyFixed;

  d->displaySampleText();
  emit fontSelected( d->m_selFont );
}

void KFontRequester::setSampleText( const QString &text )
{
  d->m_sampleText = text;
  d->displaySampleText();
}

void KFontRequester::setTitle( const QString &title )
{
  d->m_title = title;
  d->setToolTip();
}

void KFontRequester::KFontRequesterPrivate::_k_buttonClicked()
{
    KFontChooser::DisplayFlags flags = KFontChooser::NoDisplayFlags;
    if ( m_onlyFixed ) {
        flags |= KFontChooser::FixedFontsOnly;
    }
    bool ok= false;
    QFont font = QFontDialog::getFont( &ok, m_selFont, q->parentWidget() );

    if ( ok )
    {
        m_selFont = font;
        displaySampleText();
        emit q->fontSelected( m_selFont );
    }
}

void KFontRequester::KFontRequesterPrivate::displaySampleText()
{
  m_sampleLabel->setFont( m_selFont );

  qreal size = m_selFont.pointSizeF();
  if(size == -1)
    size = m_selFont.pixelSize();

  if ( m_sampleText.isEmpty() ) {
    QString family = translateFontName(m_selFont.family());
    m_sampleLabel->setText( QString::fromUtf8( "%1 %2" ).arg( family ).arg( size ) );
  }
  else {
    m_sampleLabel->setText( m_sampleText );
  }
}

void KFontRequester::KFontRequesterPrivate::setToolTip()
{
  m_button->setToolTip( tr( "Click to select a font" ) );

  m_sampleLabel->setToolTip( QString() );
  m_sampleLabel->setWhatsThis(QString());

  if ( m_title.isNull() )
  {
    m_sampleLabel->setToolTip( tr( "Preview of the selected font" ) );
    m_sampleLabel->setWhatsThis( tr( "This is a preview of the selected font. You can change it"
         " by clicking the \"Choose...\" button." ) );
  }
  else
  {
    m_sampleLabel->setToolTip( tr( "Preview of the \"%1\" font" ).arg( m_title ) );
    m_sampleLabel->setWhatsThis( tr( "This is a preview of the \"%1\" font. You can change it"
        " by clicking the \"Choose...\" button." ).arg( m_title ) );
  }
}

#include "moc_kfontrequester.cpp"

/* vim: et sw=2 ts=2
*/
