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

#include <QSpinBox>
#include <QCheckBox>

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

void ImageExportDialog::setInitialCustomSize(const QSize& size)
{
  m_mainWidget->m_widthSpinBox->setValue(size.width());
  m_mainWidget->m_heightSpinBox->setValue(size.height());
}

QString ImageExportDialog::selectedPage() const
{
  return m_mainWidget->m_pageCombo->currentText();
}

bool ImageExportDialog::usePageBorders() const
{
  return (m_mainWidget->m_exportAreaCombo->currentItem() == 0);
}

bool ImageExportDialog::useCustomSize() const
{
  return m_mainWidget->m_customSizeCheckBox->isChecked();
}

QSize ImageExportDialog::customSize() const
{
  QSize size;
  size.setWidth(m_mainWidget->m_widthSpinBox->value());
  size.setHeight(m_mainWidget->m_heightSpinBox->value());

  return size;
}

int ImageExportDialog::margin() const
{
  return m_mainWidget->m_marginSpinBox->value();
}

}

#include "kivio_imageexportdialog.moc"
