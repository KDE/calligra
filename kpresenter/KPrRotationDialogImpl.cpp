// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include <qtoolbutton.h>
#include <qslider.h>
#include <q3groupbox.h>
#include <QLayout>
#include <q3buttongroup.h>
#include <qobject.h>
#include <qevent.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <Q3Frame>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <knuminput.h>
#include <kvbox.h>

#include "KPrRotationDialogImpl.h"
#include "rotationpropertyui.h"
#include "KPrTextPreview.h"

KPrRotationDialogImpl::KPrRotationDialogImpl( QWidget *parent, const char* name )
: KDialog( parent )
, m_dialog( new RotationPropertyUI( this, name ) )
{
    setCaption(i18n( "Rotation"));
    setButtons(Ok|Cancel|Apply);
    enableButtonSeparator(true);

    noSignals = false;
    m_preview = new KPrTextPreview( m_dialog->previewPanel );
    Q3HBoxLayout *lay = new Q3HBoxLayout( m_dialog->previewPanel, m_dialog->previewPanel->lineWidth(), 0 );
    lay->addWidget( m_preview );

    Q3HBoxLayout *hbox = new Q3HBoxLayout(m_dialog->angleFrame);
    m_angleGroup = new KPrCircleGroup(m_dialog->angleFrame);
    hbox->addWidget(m_angleGroup);

    // Draw the circle of checkboxes.
    Q3GridLayout *circleLayout = new Q3GridLayout(m_angleGroup, 4, 5);
    circleLayout->addItem(new QSpacerItem ( 1, 1 , QSizePolicy::MinimumExpanding ), 0, 0);
    circleLayout->addItem(new QSpacerItem ( 1, 1 , QSizePolicy::MinimumExpanding ), 0, 5);
    KPrCircleToggle *r0 = new KPrCircleToggle(m_angleGroup, "tm", 0);
    KPrCircleToggle *r45 = new KPrCircleToggle(m_angleGroup, "tr", 45);
    KPrCircleToggle *r90 = new KPrCircleToggle(m_angleGroup, "mr", 90);
    KPrCircleToggle *r135 = new KPrCircleToggle(m_angleGroup, "br", 135);
    KPrCircleToggle *r180 = new KPrCircleToggle(m_angleGroup, "bm", 180);
    KPrCircleToggle *r225 = new KPrCircleToggle(m_angleGroup, "bl", -135);
    KPrCircleToggle *r270 = new KPrCircleToggle(m_angleGroup, "ml", -90);
    KPrCircleToggle *r315 = new KPrCircleToggle(m_angleGroup, "tl", -45);
    circleLayout->addWidget(r0, 0, 2);
    circleLayout->addWidget(r180, 2, 2);
    circleLayout->addWidget(r45, 0, 3);
    circleLayout->addWidget(r135, 2, 3);
    circleLayout->addWidget(r315, 0, 1);
    circleLayout->addWidget(r225, 2, 1);
    circleLayout->addWidget(r90, 1, 3);
    circleLayout->addWidget(r270, 1, 1);

    connect( m_angleGroup, SIGNAL (clicked (int)),
             this, SLOT( angleMode( int ) ) );
    connect (m_dialog->angleSlider, SIGNAL( valueChanged (int ) ),
             this, SLOT( angleMode( int ) ) );
    connect (m_dialog->angleSpinbox, SIGNAL (valueChanged (double) ),
             this, SLOT( angleChanged( double ) ) );
    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );

    setMainWidget( m_dialog );
}

void KPrRotationDialogImpl::slotOk()
{
    emit applyClicked();
    accept();
}

void KPrRotationDialogImpl::setAngle( double angle )
{
    if(noSignals) return;
    noSignals = true;
    int roundedAngle = (int) (angle + (angle >=0 ? 0.5:-0.5));
    m_dialog->angleSlider->setValue( roundedAngle );
    if(roundedAngle == -180)
        roundedAngle = 180;
    m_angleGroup->setAngle(roundedAngle);
    m_dialog->angleSpinbox->setValue( angle );
    m_preview->setAngle( angle );
    noSignals = false;
}

double KPrRotationDialogImpl::angle()
{
    return m_dialog->angleSpinbox->value();
}

void KPrRotationDialogImpl::angleChanged( double angle )
{
    setAngle( angle );
}

void KPrRotationDialogImpl::angleMode( int angle )
{
    setAngle( angle );
}


KPrCircleToggle::KPrCircleToggle( QWidget *parent, const QString &image, int id )
    : QLabel( parent )
{
    KIconLoader il("kpresenter");
    m_off = il.loadIcon("rotate/" + image, K3Icon::NoGroup, 28);
    m_on = il.loadIcon("rotate/" + image + "dn", K3Icon::NoGroup, 28);

    m_selected = false;
    m_id = id;
    setMouseTracking(true);
    setPixmap( m_off );
    KPrCircleGroup *cg = dynamic_cast<KPrCircleGroup*> (parent);
    if(cg != 0)
        cg->add(this);
}

void KPrCircleToggle::mousePressEvent ( QMouseEvent * e ) {
    if(e->button() != Qt:: LeftButton)
        return;
    setChecked(!m_selected);
}

void KPrCircleToggle::setChecked(bool on) {
    if(on == m_selected) return;
    m_selected = on;
    setPixmap( m_selected?m_on:m_off );
    emit clicked(m_id);
}

KPrCircleGroup::KPrCircleGroup(QWidget *parent)
    : Q3Frame(parent), m_buttons()
{
    noSignals=false;
}

void KPrCircleGroup::setAngle(int angle) {
    noSignals = true;
    KPrCircleToggle *button;
    for ( button = m_buttons.first(); button; button = m_buttons.next() )
        button->setChecked(angle == button->id());
    noSignals = false;
}

void KPrCircleGroup::add(KPrCircleToggle *button) {
    connect (button, SIGNAL(clicked (int)), this, SLOT (selectionChanged (int)) );
    m_buttons.append(button);
}

void KPrCircleGroup::selectionChanged(int buttonId) {
    if(noSignals)
        return;
    KPrCircleToggle *button;
    for ( button = m_buttons.first(); button; button = m_buttons.next() )
        button->setChecked(buttonId == button->id());
    emit clicked(buttonId);
}

#include "KPrRotationDialogImpl.moc"
