/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2013 Jigar Raisinghani <jigarraisinghani@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local


#ifndef PIVOT_H
#define PIVOT_H

#include <KoDialog.h>

namespace Calligra{
    namespace Sheets{
/**
 * \ingroup UI
 * Dialog to create Pivot.
 */


    class Selection;
  

    class Pivot : public KoDialog
    {
      Q_OBJECT

    public:
      Pivot(QWidget* parent,Selection* selection);
      ~Pivot() override;

    private Q_SLOTS:
      void slotUser2Clicked();

    private:
     
      class Private;
      Private *const d;
    };
  }//Sheets
}//Calligra


#endif // PIVOT_H
