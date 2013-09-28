/* This file is part of the KDE libraries
   Copyright (C) 2000 David Faure <faure@kde.org>
   Copyright (C) 2007 Olivier Goffart <ogoffart at kde.org>
   Copyright (C) 2013 Jarosław Staniek <staniek@kde.org>

   Based on kpasswordwidget.cpp from kdelibs

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "KexiPasswordWidget.h"

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QTextDocument>
#include <QTimer>
#include <QPalette>

#include <kcombobox.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <khbox.h>
#include <kdebug.h>
#include <kconfiggroup.h>
#include <ktitlewidget.h>

#include "ui_KexiPasswordWidget.h"

/** @internal */
class KexiPasswordWidget::KexiPasswordWidgetPrivate
{
public:
    KexiPasswordWidgetPrivate(KexiPasswordWidget *q)
        : q(q),
          userEditCombo(0),
          pixmapLabel(0),
          commentRow(0)
    {}

    void activated( const QString& userName );

    void updateFields();
    void init();

    KexiPasswordWidget *q;
    KexiPasswordWidgetFlags m_flags;
    Ui_KexiPasswordWidget ui;
    QMap<QString,QString> knownLogins;
    KComboBox* userEditCombo;
    QLabel* pixmapLabel;
    unsigned int commentRow;
};

KexiPasswordWidget::KexiPasswordWidget(QWidget* parent ,
                                       const KexiPasswordWidgetFlags& flags)
   : QWidget(parent), d(new KexiPasswordWidgetPrivate(this))
{
    d->m_flags = flags;
    d->init();
}

KexiPasswordWidget::~KexiPasswordWidget()
{
    delete d;
}

static void setLineEditReadOnly(KLineEdit *edit, bool readOnly)
{
    QPalette p(edit->parentWidget()->palette());
    p.setColor(QPalette::Base, Qt::transparent);

    edit->setReadOnly(readOnly);
    edit->setFrame(!readOnly);
    edit->setPalette(readOnly ? p : edit->parentWidget()->palette());
    edit->setFocusPolicy(readOnly ? Qt::NoFocus : Qt::StrongFocus);
    QFont f = edit->font();
    f.setBold(readOnly);
    edit->setFont(f);
}

void KexiPasswordWidget::KexiPasswordWidgetPrivate::updateFields()
{
    if (q->anonymousMode()) {
        ui.userEdit->setEnabled( false );
        ui.nameEdit->setEnabled( false );
        ui.domainEdit->setEnabled( false );
        ui.passEdit->setEnabled( false );
        ui.keepCheckBox->setEnabled( false );
    }
    else {
        setLineEditReadOnly(ui.userEdit, m_flags & KexiPasswordWidget::UsernameReadOnly);
        setLineEditReadOnly(ui.nameEdit, m_flags & KexiPasswordWidget::DatabaseNameReadOnly);
        setLineEditReadOnly(ui.domainEdit, m_flags & KexiPasswordWidget::DomainReadOnly);

        ui.passEdit->setEnabled( true );
        ui.keepCheckBox->setEnabled( true );
    }
}

void KexiPasswordWidget::KexiPasswordWidgetPrivate::init()
{
    ui.setupUi( q );
    ui.errorMessage->setHidden(true);

    // Row 4: Username field
    if ( m_flags & KexiPasswordWidget::ShowUsernameLine ) {
        ui.userEdit->setFocus();
        QObject::connect( ui.userEdit, SIGNAL(returnPressed()), ui.passEdit, SLOT(setFocus()) );
        q->setFocusProxy(ui.userEdit);
    }
    else {
        ui.userNameLabel->hide();
        ui.userEdit->hide();
        ui.passEdit->setFocus();
        q->setFocusProxy(ui.passEdit);
    }

    if (!(m_flags & KexiPasswordWidget::ShowAnonymousLoginCheckBox)) {
        ui.anonymousCheckBox->hide();
        ui.anonymousLabel->hide();
    }
    else {
        QObject::connect( ui.anonymousCheckBox, SIGNAL(stateChanged(int)), q, SLOT(updateFields()) );
    }

    if (!(m_flags & KexiPasswordWidget::ShowDatabaseNameLine )) {
        q->showDatabaseName(false);
    }

    if (!(m_flags & KexiPasswordWidget::ShowDomainLine )) {
        ui.domainLabel->hide();
        ui.domainEdit->hide();
    }

    if (!(m_flags & KexiPasswordWidget::ShowKeepPassword)) {
        ui.keepCheckBox->hide();
        ui.keepCheckBoxLabel->hide();
    }

    updateFields();

    QRect desktop = KGlobalSettings::desktopGeometry(q->topLevelWidget());
    q->setMinimumWidth(qMin(1000, qMax(q->sizeHint().width(), desktop.width() / 4)));
    if ( ( m_flags & KexiPasswordWidget::ShowIcon ) ) {
        q->setPixmap(KIcon("dialog-password").pixmap(KIconLoader::SizeHuge));
    }

    QObject::connect(ui.userEdit, SIGNAL(returnPressed()), q, SIGNAL(returnPressed()));
    QObject::connect(ui.domainEdit, SIGNAL(returnPressed()), q, SIGNAL(returnPressed()));
    QObject::connect(ui.passEdit, SIGNAL(returnPressed()), q, SIGNAL(returnPressed()));
}

void KexiPasswordWidget::setPixmap(const QPixmap &pixmap)
{
    if ( !d->pixmapLabel )
    {
        d->pixmapLabel = new QLabel(this);
        d->pixmapLabel->setAlignment( Qt::AlignLeft | Qt::AlignTop );
        d->ui.hboxLayout->insertWidget( 0, d->pixmapLabel );
    }

    d->pixmapLabel->setPixmap( pixmap );
}

QPixmap KexiPasswordWidget::pixmap() const
{
    if ( !d->pixmapLabel ) {
        return QPixmap();
    }

    return *d->pixmapLabel->pixmap();
}


void KexiPasswordWidget::setUsername(const QString& user)
{
    d->ui.userEdit->setText(user);
    if ( user.isEmpty() )
        return;

    d->activated(user);
    if ( d->ui.userEdit->isVisibleTo( this ) )
    {
        d->ui.passEdit->setFocus();
    }
}


QString KexiPasswordWidget::username() const
{
    return d->ui.userEdit->text();
}

QString KexiPasswordWidget::password() const
{
    return d->ui.passEdit->text();
}

void KexiPasswordWidget::setDomain(const QString& domain)
{
    d->ui.domainEdit->setText(domain);
}

QString KexiPasswordWidget::domain() const
{
    return d->ui.domainEdit->text();
}

void KexiPasswordWidget::setAnonymousMode(bool anonymous)
{
    d->ui.anonymousCheckBox->setChecked( anonymous );
}

bool KexiPasswordWidget::anonymousMode() const
{
    return d->ui.anonymousCheckBox->isChecked();
}


void KexiPasswordWidget::setKeepPassword( bool b )
{
    d->ui.keepCheckBox->setChecked( b );
}

bool KexiPasswordWidget::keepPassword() const
{
    return d->ui.keepCheckBox->isChecked();
}

void KexiPasswordWidget::addCommentLine( const QString& label,
                                      const QString& comment )
{
    int gridMarginLeft, gridMarginTop, gridMarginRight, gridMarginBottom;
    d->ui.formLayout->getContentsMargins(&gridMarginLeft, &gridMarginTop, &gridMarginRight, &gridMarginBottom);

    int spacing = d->ui.formLayout->horizontalSpacing();
    if (spacing < 0) {
        // same inter-column spacing for all rows, see comment in qformlayout.cpp
        spacing = style()->combinedLayoutSpacing(QSizePolicy::Label, QSizePolicy::LineEdit, Qt::Horizontal, 0, this);
    }

    QLabel* c = new QLabel(comment, this);
    c->setWordWrap(true);

    d->ui.formLayout->insertRow(d->commentRow, label, c);
    ++d->commentRow;

    // cycle through column 0 widgets and see the max width so we can set the minimum height of
    // column 2 wordwrapable labels
    int firstColumnWidth = 0;
    for (int i = 0; i < d->ui.formLayout->rowCount(); ++i) {
        QLayoutItem *li = d->ui.formLayout->itemAt(i, QFormLayout::LabelRole);
        if (li) {
            QWidget *w = li->widget();
            if (w && !w->isHidden()) {
                firstColumnWidth = qMax(firstColumnWidth, w->sizeHint().width());
            }
        }
    }
    for (int i = 0; i < d->ui.formLayout->rowCount(); ++i) {
        QLayoutItem *li = d->ui.formLayout->itemAt(i, QFormLayout::FieldRole);
        if (li) {
            QLabel *l = qobject_cast<QLabel*>(li->widget());
            if (l && l->wordWrap()) {
                int w = sizeHint().width() - firstColumnWidth - ( 2 * KDialog::marginHint() )
                        - gridMarginLeft - gridMarginRight - spacing;
                l->setMinimumSize( w, l->heightForWidth(w) );
            }
        }
    }
}

void KexiPasswordWidget::showErrorMessage( const QString& message, const ErrorType type )
{
    d->ui.errorMessage->setText( message, KTitleWidget::ErrorMessage );

    QFont bold = font();
    bold.setBold( true );
    switch ( type ) {
        case PasswordError:
            d->ui.passwordLabel->setFont( bold );
            d->ui.passEdit->clear();
            d->ui.passEdit->setFocus();
            break;
        case UsernameError:
            if ( d->ui.userEdit->isVisibleTo( this ) )
            {
                d->ui.userNameLabel->setFont( bold );
                d->ui.userEdit->setFocus();
            }
            break;
        case DomainError:
            if ( d->ui.domainEdit->isVisibleTo( this ) )
            {
                d->ui.domainLabel->setFont( bold );
                d->ui.domainEdit->setFocus();
            }
            break;
        case FatalError:
            d->ui.userNameLabel->setEnabled( false );
            d->ui.userEdit->setEnabled( false );
            d->ui.passwordLabel->setEnabled( false );
            d->ui.passEdit->setEnabled( false );
            d->ui.keepCheckBox->setEnabled( false );
            //enableButton( Ok, false );
            break;
        default:
            break;
    }
    adjustSize();
}

void KexiPasswordWidget::setPrompt(const QString& prompt)
{
    d->ui.prompt->setVisible(!prompt.isEmpty());
    d->ui.prompt->setText( prompt );
    d->ui.prompt->setWordWrap( true );
    d->ui.prompt->setMinimumHeight(
                d->ui.prompt->heightForWidth( width() -  ( 2 * KDialog::marginHint() ) ) );
}

QString KexiPasswordWidget::prompt() const
{
    return d->ui.prompt->text();
}

void KexiPasswordWidget::setPassword(const QString &p)
{
    d->ui.passEdit->setText(p);
}

void KexiPasswordWidget::setUsernameReadOnly(bool readOnly)
{
    setLineEditReadOnly(d->ui.userEdit, readOnly);
    d->m_flags |= KexiPasswordWidget::UsernameReadOnly;
    if (readOnly) {
        d->m_flags ^= KexiPasswordWidget::UsernameReadOnly;
    }

    if ( readOnly && d->ui.userEdit->hasFocus() ) {
        d->ui.passEdit->setFocus();
    }
}

void KexiPasswordWidget::showDatabaseName(bool show)
{
    d->ui.nameLabel->setVisible(show);
    d->ui.nameEdit->setVisible(show);
}

void KexiPasswordWidget::setDatabaseName(const QString& databaseName)
{
    d->ui.nameEdit->setText(databaseName);
}

void KexiPasswordWidget::setDatabaseNameReadOnly(bool readOnly)
{
    setLineEditReadOnly(d->ui.nameEdit, readOnly);
    d->m_flags |= KexiPasswordWidget::DatabaseNameReadOnly;
    if (readOnly) {
        d->m_flags ^= KexiPasswordWidget::DatabaseNameReadOnly;
    }

    if ( readOnly && d->ui.userEdit->hasFocus() ) {
        d->ui.passEdit->setFocus();
    }
}

void KexiPasswordWidget::setKnownLogins( const QMap<QString, QString>& knownLogins )
{
    const int nr = knownLogins.count();
    if ( nr == 0 ) {
        return;
    }

    if ( nr == 1 ) {
        d->ui.userEdit->setText( knownLogins.begin().key() );
        setPassword( knownLogins.begin().value() );
        return;
    }

    Q_ASSERT( !d->ui.userEdit->isReadOnly() );
    if ( !d->userEditCombo ) {
        d->ui.formLayout->removeWidget(d->ui.userEdit);
        delete d->ui.userEdit;
        d->userEditCombo = new KComboBox(true, this);
        d->ui.userEdit = qobject_cast<KLineEdit*>(d->userEditCombo->lineEdit());
//        QSize s = d->userEditCombo->sizeHint();
//        d->ui.userEditCombo->setFixedHeight( s.height() );
//        d->ui.userEditCombo->setMinimumWidth( s.width() );
        d->ui.userNameLabel->setBuddy( d->userEditCombo );
        d->ui.formLayout->setWidget( d->commentRow, QFormLayout::FieldRole, d->userEditCombo );
        setTabOrder( d->ui.userEdit, d->ui.anonymousCheckBox );
        setTabOrder( d->ui.anonymousCheckBox, d->ui.domainEdit );
        setTabOrder( d->ui.domainEdit, d->ui.passEdit );
        setTabOrder( d->ui.passEdit, d->ui.keepCheckBox );
        connect( d->ui.userEdit, SIGNAL(returnPressed()), d->ui.passEdit, SLOT(setFocus()) );
    }

    d->knownLogins = knownLogins;
    d->userEditCombo->addItems( knownLogins.keys() );
    d->userEditCombo->setFocus();

    connect( d->userEditCombo, SIGNAL(activated(QString)),
             this, SLOT(activated(QString)) );
}

void KexiPasswordWidget::KexiPasswordWidgetPrivate::activated( const QString& userName )
{
    QMap<QString, QString>::ConstIterator it = knownLogins.constFind( userName );
    if ( it != knownLogins.constEnd() ) {
        q->setPassword( it.value() );
    }
}

bool KexiPasswordWidget::checkPassword()
{
    return true;
}

#include "KexiPasswordWidget.moc"
