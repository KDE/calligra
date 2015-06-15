#!/bin/sh
#
# Shows all headers for custom (promoted) widgets in UI files of specified directory
# Why is it useful?
# To make sure global KF5 includes are used, i.e. <KLineEdit>, not "klineedit.h".
# The latter can pick up an old header from kdelibs4.
#
# So it should be like:
# <header location="global">KLineEdit</header>
#
# And not like:
# <header>klineedit.h</header>
#
# Instead of altering that in Designer, you can edit the file by hand.
#
# PS: The UI file format: http://doc.qt.io/qt-5.4/designer-ui-file-format.html
# Relevant part:
#      <xs:complexType name="Header" mixed="true" >
#         <xs:attribute name="location" type="xs:string" />  <!-- local or global -->
#      </xs:complexType>
#

grep "\<header\>" `find $1 -name "*.ui"`
