/*****************************************************************

Copyright (c) 2000-2003 Matthias Hoelzer-Kluepfel <mhk@kde.org>
                        Tobias Koenig <tokoe@kde.org>
                        Daniel Molkentin <molkentin@kde.org>
Copyright (c) 2008 Urs Wolfer <uwolfer @ kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include "ktip.h"

#include <QApplication>
#include <QFile>
#include <QCheckBox>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QDesktopWidget>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextBrowser>
#include <QDebug>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <krandom.h>
#include <kseparator.h>
#include <kstandardguiitem.h>

class KTipDatabase::Private
{
  public:
    void loadTips( const QString &tipFile );
    void addTips( const QString &tipFile );

    QStringList tips;
    int currentTip;
};

void KTipDatabase::Private::loadTips( const QString &tipFile )
{
  tips.clear();
  addTips( tipFile );
}

/**
 * If you change something here, please update the script
 * preparetips, which depends on extracting exactly the same
 * text as done here.
 */
void KTipDatabase::Private::addTips( const QString &tipFile )
{
    const QString fileName = QStandardPaths::locate(QStandardPaths::GenericDataLocation, tipFile);

  if ( fileName.isEmpty() ) {
    qDebug() << "KTipDatabase::addTips: can't find '" << tipFile << "' in standard dirs";
    return;
  }

  QFile file( fileName );
  if ( !file.open( QIODevice::ReadOnly ) ) {
    qDebug() << "KTipDatabase::addTips: can't open '" << fileName << "' for reading";
    return;
  }

  QByteArray data = file.readAll();
  QString content = QString::fromUtf8( data.constData(), data.size() );
  const QRegExp rx( "\\n+" );

  int pos = -1;
  while ( ( pos = content.indexOf( "<html>", pos + 1, Qt::CaseInsensitive ) ) != -1 ) {
    /**
     * To make translations work, tip extraction here must exactly
     * match what is done by the preparetips script.
     */
    QString tip = content
           .mid( pos + 6, content.indexOf( "</html>", pos, Qt::CaseInsensitive ) - pos - 6 )
           .replace( rx, "\n" );

    if ( !tip.endsWith( '\n' ) )
      tip += '\n';

    if ( tip.startsWith( '\n' ) )
      tip = tip.mid( 1 );

    if ( tip.isEmpty() ) {
      qDebug() << "Empty tip found! Skipping! " << pos;
      continue;
    }

    tips.append( tip );
  }

  file.close();
}


KTipDatabase::KTipDatabase( const QString &_tipFile )
  : d( new Private )
{
  QString tipFile = _tipFile;

    if (tipFile.isEmpty())
      tipFile = QCoreApplication::applicationName() + "/tips";

  d->loadTips( tipFile );

  if ( !d->tips.isEmpty() )
    d->currentTip = KRandom::random() % d->tips.count();
}

KTipDatabase::KTipDatabase( const QStringList& tipsFiles )
  : d( new Private )
{
  if ( tipsFiles.isEmpty() || ( ( tipsFiles.count() == 1 ) && tipsFiles.first().isEmpty() ) ) {
      d->addTips(QCoreApplication::applicationName() + "/tips");
  } else {
    for ( QStringList::ConstIterator it = tipsFiles.begin(); it != tipsFiles.end(); ++it )
      d->addTips( *it );
  }

  if ( !d->tips.isEmpty() )
    d->currentTip = KRandom::random() % d->tips.count();
}

KTipDatabase::~KTipDatabase()
{
    delete d;
}

void KTipDatabase::nextTip()
{
  if ( d->tips.isEmpty() )
    return;

  d->currentTip += 1;

  if ( d->currentTip >= (int) d->tips.count() )
    d->currentTip = 0;
}

void KTipDatabase::prevTip()
{
  if ( d->tips.isEmpty() )
    return;

  d->currentTip -= 1;

  if ( d->currentTip < 0 )
    d->currentTip = d->tips.count() - 1;
}

QString KTipDatabase::tip() const
{
  if ( d->tips.isEmpty() )
    return QString();

  return d->tips[ d->currentTip ];
}


class KTipDialog::Private
{
  public:
    Private( KTipDialog *_parent )
      : parent( _parent )
    {
    }
    ~Private()
    {
      delete database;
    }

    void _k_nextTip();
    void _k_prevTip();
    void _k_showOnStart( bool );

    KTipDialog *parent;
    KTipDatabase *database;
    QCheckBox *tipOnStart;
    QTextBrowser *tipText;

    static KTipDialog *mInstance;
};

KTipDialog *KTipDialog::Private::mInstance = 0;

void KTipDialog::Private::_k_prevTip()
{
  database->prevTip();
  tipText->setHtml( QString::fromLatin1( "<html><body>%1</body></html>" )
                  .arg( KTipDialog::tr( database->tip().toUtf8() ) ) );
}

void KTipDialog::Private::_k_nextTip()
{
  database->nextTip();
  tipText->setHtml( QString::fromLatin1( "<html><body>%1</body></html>" )
                  .arg( KTipDialog::tr( database->tip().toUtf8() ) ) );
}

void KTipDialog::Private::_k_showOnStart( bool on )
{
  parent->setShowOnStart( on );
}


KTipDialog::KTipDialog( KTipDatabase *database, QWidget *parent )
  : QDialog( parent ),
    d( new Private( this ) )
{
  setWindowTitle( tr( "Tip of the Day" ) );

  /**
   * Parent is 0L when TipDialog is used as a mainWidget. This should
   * be the case only in ktip, so let's use the ktip layout.
   */
  bool isTipDialog = (parent != 0);

  d->database = database;

  setWindowIcon(QIcon::fromTheme("ktip"));

  QVBoxLayout *mainLayout = new QVBoxLayout( this );
  mainLayout->setMargin( 0 );

  if ( isTipDialog ) {
    QLabel *titleLabel = new QLabel( this );
    titleLabel->setText( tr( "Did you know...?\n" ) );
    titleLabel->setFont(QFont(qApp->font().family(), 20, QFont::Bold));
    titleLabel->setAlignment( Qt::AlignCenter );
    mainLayout->addWidget( titleLabel );
  }

  QHBoxLayout *browserLayout = new QHBoxLayout();
  mainLayout->addLayout( browserLayout );

  d->tipText = new QTextBrowser( this );

  d->tipText->setOpenExternalLinks( true );

  d->tipText->setWordWrapMode( QTextOption::WrapAtWordBoundaryOrAnywhere );

  QStringList paths;
  paths << QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "icons", QStandardPaths::LocateDirectory)
        << QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "kdewizard/pics", QStandardPaths::LocateDirectory);

  d->tipText->setSearchPaths( paths );

  d->tipText->setFrameStyle( QFrame::NoFrame );
  d->tipText->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  QPalette tipPal(d->tipText->palette());
  tipPal.setColor(QPalette::Base, Qt::transparent);
  tipPal.setColor(QPalette::Text, tipPal.color(QPalette::WindowText));
  d->tipText->setPalette(tipPal);

  browserLayout->addWidget( d->tipText );

  QLabel *label = new QLabel( this );
  label->setPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "kconfigwidgets/pics/ktip-bulb.png"));
  label->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
  browserLayout->addWidget( label );

  if ( !isTipDialog ) {
    resize( 520, 280 );
    QSize sh = size();

    QRect rect = QApplication::desktop()->screenGeometry(QCursor::pos());

    move( rect.x() + ( rect.width() - sh.width() ) / 2,
          rect.y() + ( rect.height() - sh.height() ) / 2 );
  }

  KSeparator* sep = new KSeparator( Qt::Horizontal );
  mainLayout->addWidget( sep );

  QHBoxLayout *buttonLayout = new QHBoxLayout();

  mainLayout->addLayout( buttonLayout );

  d->tipOnStart = new QCheckBox( tr( "&Show tips on startup" ) );
  buttonLayout->addWidget( d->tipOnStart, 1 );

  QPushButton *prev = new QPushButton;
  KGuiItem::assign(prev, KStandardGuiItem::back( KStandardGuiItem::UseRTL ));
  prev->setText( tr( "&Previous" ) );
  buttonLayout->addWidget( prev );

  QPushButton *next = new QPushButton;
  KGuiItem::assign(next, KStandardGuiItem::forward( KStandardGuiItem::UseRTL ));
  next->setText( tr( "&Next", "Opposite to Previous" ) );
  buttonLayout->addWidget( next );

  QPushButton *ok = new QPushButton;
  KGuiItem::assign(ok, KStandardGuiItem::close());
  ok->setDefault( true );
  buttonLayout->addWidget( ok );

  KConfigGroup config( KSharedConfig::openConfig(), "TipOfDay" );
  d->tipOnStart->setChecked( config.readEntry( "RunOnStart", true ) );

  connect( next, SIGNAL(clicked()), this, SLOT(_k_nextTip()) );
  connect( prev, SIGNAL(clicked()), this, SLOT(_k_prevTip()) );
  connect( ok, SIGNAL(clicked()), this, SLOT(accept()) );
  connect( d->tipOnStart, SIGNAL(toggled(bool)), this, SLOT(_k_showOnStart(bool)) );

  ok->setFocus();

  d->_k_nextTip();
}

KTipDialog::~KTipDialog()
{
  if ( Private::mInstance == this )
    Private::mInstance = 0L;
  delete d;
}

/**
 * use the one with a parent window as parameter instead of this one
 * or you will get focus problems
 */
void KTipDialog::showTip( const QString &tipFile, bool force )
{
  showTip( 0, tipFile, force );
}

void KTipDialog::showTip( QWidget *parent, const QString &tipFile, bool force )
{
  showMultiTip( parent, QStringList( tipFile ), force );
}

void KTipDialog::showMultiTip( QWidget *parent, const QStringList &tipFiles, bool force )
{
  KConfigGroup configGroup( KSharedConfig::openConfig(), "TipOfDay" );

  const bool runOnStart = configGroup.readEntry( "RunOnStart", true );

  if ( !force ) {
    if ( !runOnStart )
      return;

    // showing the tooltips on startup suggests the tooltip
    // will be shown *each time* on startup, not $random days later
    // TODO either remove or uncomment this code, but make the situation clear
    /*bool hasLastShown = configGroup.hasKey( "TipLastShown" );
    if ( hasLastShown ) {
      const int oneDay = 24 * 60 * 60;
      QDateTime lastShown = configGroup.readEntry( "TipLastShown", QDateTime() );

      // Show tip roughly once a week
      if ( lastShown.secsTo( QDateTime::currentDateTime() ) < (oneDay + (KRandom::random() % (10 * oneDay))) )
        return;
    }

    configGroup.writeEntry( "TipLastShown", QDateTime::currentDateTime() );

    if ( !hasLastShown )
      return; // Don't show tip on first start*/
  }

  if ( !Private::mInstance )
    Private::mInstance = new KTipDialog( new KTipDatabase( tipFiles ), parent );
  else
      // The application might have changed the RunOnStart option in its own
      // configuration dialog, so we should update the checkbox.
      Private::mInstance->d->tipOnStart->setChecked( runOnStart );

  Private::mInstance->show();
  Private::mInstance->raise();
}

void KTipDialog::setShowOnStart( bool on )
{
  KConfigGroup config( KSharedConfig::openConfig(), "TipOfDay" );
  config.writeEntry( "RunOnStart", on );
}

bool KTipDialog::eventFilter( QObject *object, QEvent *event )
{
  if ( object == d->tipText && event->type() == QEvent::KeyPress &&
       (((QKeyEvent *)event)->key() == Qt::Key_Return ||
       ((QKeyEvent *)event)->key() == Qt::Key_Space ))
    accept();

  /**
   * If the user presses Return or Space, we close the dialog as if the
   * default button was pressed even if the QTextBrowser has the keyboard
   * focus. This could have the bad side-effect that the user cannot use the
   * keyboard to open urls in the QTextBrowser, so we just let it handle
   * the key event _additionally_. (Antonio)
   */

  return QWidget::eventFilter( object, event );
}


#include "moc_ktip.cpp"
