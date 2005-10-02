// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2004 Thorsten Zachmann <zachmann@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef PRESSTEP_H
#define PRESSTEP_H

/**
 * Class for keeping a presentation step
 * m_pageNumber the current page number 0 based
 * m_step       the current effect step 0 based
 * m_subStep    the current sub step 0 based for text animation
 * m_animate    true if the current step should be animated
 * m_animateSub true if the current sub step should be animated
 */
class PresStep
{
public:
    PresStep()
      : m_pageNumber( 0 ), m_step( 0 ), m_subStep( 0 ),
    m_animate( false ), m_animateSub( false )
    {};
    PresStep( unsigned int pageNumber, unsigned int step, unsigned int subStep,
        bool animate = false, bool animateSub = false )
      : m_pageNumber( pageNumber ), m_step( step ), m_subStep( subStep ),
    m_animate( animate ), m_animateSub( animateSub )
    {};

    int m_pageNumber;
    int m_step;
    int m_subStep;
    bool m_animate;
    bool m_animateSub;
};

#endif /* PRESSTEP_H */
