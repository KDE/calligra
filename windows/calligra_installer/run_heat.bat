:: Copyright (c) 2011-2012 KO GmbH.  All rights reserved.
:: Copyright (c) 2011-2012 Stuart Dickson <stuartmd@kogmbh.com>
::
:: SPDX-License-Identifier: CPL-1.0
:: ------------------------------------------------------------------------
"heat.exe" dir "%C2WINSTALL_INPUT%" -cg CalligraBaseFiles -gg -scom -sreg -sfrag -srd -dr CALLIGRADIR -var env.C2WINSTALL_INPUT -t PreserveGUIDs.xslt -out "%~dp0HeatFragment.wxs"
copy HeatFragment.wxs PreviousHeatFragment.xml /Y
