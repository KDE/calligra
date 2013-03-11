/* This file is part of the KDE project
 * Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KPRDURATIONPARSER_H
#define KPRDURATIONPARSER_H

class QString;

/**
 * This implements a parser for clock values as defined in SMIL 2.0, 10.3.1 Attributes
 * Clock values                                                                                                                  
 * 
 * Clock values have the following syntax:                                                                                   
 * 
 *     Clock-value         ::= ( Full-clock-value | Partial-clock-value | Timecount-value )                                     
 *     Full-clock-value    ::= Hours ":" Minutes ":" Seconds ("." Fraction)?                                                   
 *     Partial-clock-value ::= Minutes ":" Seconds ("." Fraction)?                                                             
 *     Timecount-value     ::= Timecount ("." Fraction)? (Metric)?                                                           
 *     Metric              ::= "h" | "min" | "s" | "ms"                                                                          
 *     Hours               ::= DIGIT+; any positive number                                                                       
 *     Minutes             ::= 2DIGIT; range from 00 to 59                                                                       
 *     Seconds             ::= 2DIGIT; range from 00 to 59                                                                       
 *     Fraction                ::= DIGIT+                                                                                        
 *     Timecount           ::= DIGIT+                                                                                            
 *     2DIGIT                    ::= DIGIT DIGIT                                                                                 
 *     DIGIT                        ::= [0-9]                                                                                    
 * 
 * For Timecount values, the default metric suffix is "s" (for seconds). No embedded white 
 * space is allowed in clock values, although leading and trailing white space characters 
 * will be ignored.                                                                     
 * 
 * The following are examples of legal clock values:                                                                             
 * 
 * o Full clock values:                                                                                                            
 *     02:30:03     = 2 hours, 30 minutes and 3 seconds                                                                          
 *     50:00:10.25  = 50 hours, 10 seconds and 250 milliseconds                                                                  
 * o Partial clock value:                                                                                                          
 *     02:33    = 2 minutes and 33 seconds                                                                                       
 *     00:10.5  = 10.5 seconds = 10 seconds and 500 milliseconds                                                                 
 * o Timecount values:                                                                                                             
 *     3.2h     = 3.2 hours = 3 hours and 12 minutes                                                                             
 *     45min    = 45 minutes                                                                                                     
 *     30s      = 30 seconds                                                                                                     
 *     5ms      = 5 milliseconds                                                                                                 
 *     12.467   = 12 seconds and 467 milliseconds                                                                                
 * 
 * Fractional values are just (base 10) floating point definitions of seconds. The number
 * of digits allowed is unlimited (although actual precision may vary among implementations).                                                                                   
 * For example:                                                                                                                  
 *     00.5s = 500 milliseconds                                                                                                  
 *     00:00.005 = 5 milliseconds                                                                                                
 *  */
class KPrDurationParser
{
public:
    static int durationMs(const QString & duration);
    static QString msToString(const int ms);

};

#endif /* KPRDURATIONPARSER_H */
