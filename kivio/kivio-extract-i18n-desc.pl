#

#
# Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>
# License: LGPL 2.0
#

if ( m/<Title +data=\"(.+)\"/ )
{
    $_ = $1;
    s/&lt;/</g;
    s/&gt;/>/g;
    s/&apos;/\'/g;
    s/&quot;/\"/g;
    s/&amp;/&/g;
    s/\"/\\\"/g;
    print "i18n( \"Stencils\", \"" . $_ . "\" );\n";
}