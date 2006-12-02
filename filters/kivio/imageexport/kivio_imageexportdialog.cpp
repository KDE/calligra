/* This file is part of the KDE project
   Copyright (C) 2005 Peter Simonsson <psn@linux.se>

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

#include "kivio_imageexportdialog.h"

#include <qspinbox.h>
#include <qcheckbox.h>

#include <kcombobox.h>
#include <klocale.h>
#include <kapplication.h>

#include "kivio_imageexportwidget.h"

namespace Kivio
{

ImageExportDialog::ImageExportDialog(QWidget* parent, const char* name)
  : KDialogBase(parent, name, false, i18n("Export to Image"), Ok|Cancel)
{
  kapp->restoreOverrideCursor();
  m_mainWidget = new ImageExportWidget(this);
  setMainWidget(m_mainWidget);
}

void ImageExportDialog::setPageList(const QStringList& pages)
{
  m_mainWidget->m_pageCombo->clear();
  m_mainWidget->m_pageCombo->insertStringList(pages);
}

void ImageExportDialog::setInitialDPI(const int dpi)
{
    if (dpi <= 72) m_mainWidget->m_DPIcomboBox->setCurrentItem(0);
    else if (dpi <= 96) m_mainWidget->m_DPIcomboBox->setCurrentItem(1);
    else if (dpi <= 150) m_mainWidget->m_DPIcomboBox->setCurrentItem(2);
    else if (dpi <= 300) m_mainWidget->m_DPIcomboBox->setCurrentItem(3);
    else if (dpi <= 600) m_mainWidget->m_DPIcomboBox->setCurrentItem(4);
    else if (dpi <= 720) m_mainWidget->m_DPIcomboBox->setCurrentItem(5);
    else if (dpi <= 1200) m_mainWidget->m_DPIcomboBox->setCurrentItem(6);
}

void ImageExportDialog::setInitialmargin(const int margin)
{
    m_mainWidget->m_marginSpinBox->setValue(margin);
}

QString ImageExportDialog::selectedPage() const
{
  return m_mainWidget->m_pageCombo->currentText();
}

int ImageExportDialog::imageDPI() const
{
    switch (m_mainWidget->m_DPIcomboBox->currentItem())
    {
        case 0:
            return 72;
        case 1:
            return 96;
        case 2:
            return 150;
        case 3:
            return 300;
        case 4:
            return 600;
        case 5:
            return 720;
        case 6:
            return 1200;
        default:
            return 300;
    }
}

bool ImageExportDialog::usePageBorders() const
{
  return (m_mainWidget->m_exportAreaCombo->currentItem() == 1);
}


int ImageExportDialog::margin() const
{
  return m_mainWidget->m_marginSpinBox->value();
}

}

#include "kivio_imageexportdialog.moc"
