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
#include <qgroupbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qobject.h>
#include <qevent.h>

#include <kdebug.h>
#include <klocale.h>
#include <knuminput.h>

#include "rotationdialogimpl.h"
#include "rotationpropertyui.h"
#include "textpreview.h"

RotationDialogImpl::RotationDialogImpl( QWidget *parent, const char* name )
: KDialogBase( parent, name, true, i18n( "Rotation"), Ok|Cancel|Apply, Ok, true )
, m_dialog( new RotationPropertyUI( this, name ) )
{
    noSignals = false;
    m_preview = new TextPreview( m_dialog->previewPanel );
    QHBoxLayout *lay = new QHBoxLayout( m_dialog->previewPanel, m_dialog->previewPanel->lineWidth(), 0 );
    lay->addWidget( m_preview );

    QHBoxLayout *hbox = new QHBoxLayout(m_dialog->angleFrame);
    m_angleGroup = new CircleGroup(m_dialog->angleFrame);
    hbox->addWidget(m_angleGroup);

    // Draw the circle of checkboxes.
    QGridLayout *circleLayout = new QGridLayout(m_angleGroup, 4, 5);
    circleLayout->addItem(new QSpacerItem ( 1, 1 ), 0, 0);
    circleLayout->addItem(new QSpacerItem ( 1, 1 ), 0, 5);
    CircleToggle *r0 = new CircleToggle(m_angleGroup, "/home/zander/images/tm", 0);
    CircleToggle *r45 = new CircleToggle(m_angleGroup, "/home/zander/images/tr", 45);
    CircleToggle *r90 = new CircleToggle(m_angleGroup, "/home/zander/images/mr", 90);
    CircleToggle *r135 = new CircleToggle(m_angleGroup, "/home/zander/images/br", 135);
    CircleToggle *r180 = new CircleToggle(m_angleGroup, "/home/zander/images/bm", 180);
    CircleToggle *r225 = new CircleToggle(m_angleGroup, "/home/zander/images/bl", -135);
    CircleToggle *r270 = new CircleToggle(m_angleGroup, "/home/zander/images/ml", -90);
    CircleToggle *r315 = new CircleToggle(m_angleGroup, "/home/zander/images/tl", -45);
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

void RotationDialogImpl::slotOk()
{
    emit apply();
    accept();
}

void RotationDialogImpl::setAngle( double angle )
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

double RotationDialogImpl::angle()
{
    return m_dialog->angleSpinbox->value();
}

void RotationDialogImpl::angleChanged( double angle )
{
    setAngle( angle );
}

void RotationDialogImpl::angleMode( int angle )
{
    setAngle( angle );
}


CircleToggle::CircleToggle( QWidget *parent, const QString &image, int id )
    : QLabel( parent ), m_off(image +".png"), m_on (image +"dn.png")
{
    m_selected = false;
    m_id = id;
    setMouseTracking(true);
    setPixmap( m_off );
    CircleGroup *cg = dynamic_cast<CircleGroup*> (parent);
    if(cg != 0)
        cg->add(this);
}

void CircleToggle::mousePressEvent ( QMouseEvent * e ) {
    if(e->button() != Qt:: LeftButton)
        return;
    setChecked(!m_selected);
}

void CircleToggle::setChecked(bool on) {
    if(on == m_selected) return;
    m_selected = on;
    setPixmap( m_selected?m_on:m_off );
    emit clicked(m_id);
}

CircleGroup::CircleGroup(QWidget *parent)
    : QFrame(parent), m_buttons()
{
    noSignals=false;
}

void CircleGroup::setAngle(int angle) {
    noSignals = true;
    CircleToggle *button;
    for ( button = m_buttons.first(); button; button = m_buttons.next() )
        button->setChecked(angle == button->id());
    noSignals = false;
}

void CircleGroup::add(CircleToggle *button) {
    connect (button, SIGNAL(clicked (int)), this, SLOT (selectionChanged (int)) );
    m_buttons.append(button);
}

void CircleGroup::selectionChanged(int buttonId) {
    if(noSignals)
        return;
    CircleToggle *button;
    for ( button = m_buttons.first(); button; button = m_buttons.next() )
        button->setChecked(buttonId == button->id());
    emit clicked(buttonId);
}

#include "rotationdialogimpl.moc"
