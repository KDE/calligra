#

#
# Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>
# License: LGPL 2.0
#

sub writei18n
{
    local $_ = @_[1];
    if ( $_ )
    {
        s/&lt;/</g;
        s/&gt;/>/g;
        s/&apos;/\'/g;
        s/&quot;/\"/g;
        s/&amp;/&/g;
        s/\"/\\\"/g;
        print "i18n( \"". @_[0] . "\", \"" . $_ . "\" );";
        if ( @_[2] )
        {
            print " // " . @_[2];
        }
        print "\n";
    }
}

if ( m/<Title +data=\"(.+)\"/ )
{
    writei18n( "Stencils", $1 , "Title" );
}

if ( m/<Description +data=\"(.+)\"/ )
{
    writei18n( "Stencils", $1 , "Description" );
}
