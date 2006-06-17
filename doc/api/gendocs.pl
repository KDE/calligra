#!/usr/bin/perl
# Copyright (C) 2006 Thomas Zander <zander@kde.org>
# this script generates dox based on the availability of a Mainpage.dox file.
# whereever there is one, doxygen is started.


#set basedir relative to ours.
$rootdir=`pwd`;
chomp($rootdir);
$basedir = $rootdir."/doc/api/";
if(! -d $basedir) {
    print "Please start this script from the root of the checkout and make sure that there is a doc/api/ dir\n";
    exit 1;
}

$doxygenconftmp=".doxygen.conf.tmp";

#init
print "Finding all sections...";
@output=`find . -name Mainpage.dox -type f | grep -v _darcs`;
foreach $section (@output) {
    chomp($section);
    $section=~s/^\.\///;
    $section=~s/\/Mainpage.dox//;
    # skip root dir while there are so little Mainpage.dox files
    if($section eq "Mainpage.dox") {
        next;
    }
    push @sections,$section;
}
@output="";

# for each section
print $#sections+1 ." found";
$i=1;
$totalSteps=$#sections*3 + 4;
foreach $section (@sections) {
    print "\n". $i++ ."/$totalSteps) Indexing section: '$section' ";

    chdir $section;
    @dirs=`find . -type d | grep -v .svn | grep -v _darcs`;
    print "\n". $i++ ."/$totalSteps) Creating tags ";
    &createConf($section, @dirs);
    system "mkdir -p \"$basedir$section\"";
    system "doxygen $doxygenconftmp >/dev/null 2>/dev/null";
    chdir $rootdir;
}

foreach $section (@sections) {
    print "\n". $i++ ."/$totalSteps) Creating docs for section: '$section' ";
    chdir $section;
    &alterConf();
    $sect=$section;
    $sect=~s/\//-/;
    system "doxygen $doxygenconftmp.2 >/dev/null 2>$basedir$sect/err.log";
    unlink "$doxygenconftmp";
    unlink "$doxygenconftmp.2";
    chdir $rootdir;
}

#Create linking page
print "\n". $i ."/$totalSteps) Creating wrapper pages";
my %classes;
chdir "$rootdir/doc/api";
foreach $section (@sections) {
    $sect=$section;
    $sect=~s/\//-/;
    # read from TAGS
    $tagfile="$section\_TAGS";
    $tagfile=~s/\//_/g;
    open(INPUT, "$sect/$tagfile");
    $class="";
    foreach $line (<INPUT>) {
        if($line=~/compound kind=\"class\"/) { $class="Class"; }
        elsif($line=~/compound kind=\"struct\"/) { $class="Struct"; }
        elsif($line=~/compound kind=\"namespace\"/) { $class="Namespace"; }
        elsif($class ne "" && $line=~/name\>(.*)\<\/name/) {
            $className=$1;
        }
        elsif($class ne "" && $line=~/filename\>(.*)\<\/filename/) {
            my $file = $1;
            $file=~s/^$rootdir/$project/;
            &addClass($file, $section, $className, $class);
        }
    }
    close INPUT;

    # read undocumented from err.log
    &parseErrorLog("$sect", "err.log", $section);
}
chdir $rootdir;

# sort and print
open(FILE, ">$rootdir/doc/api/allClasses.html");
print FILE "<html><body>\n";
print FILE "<style>.FrameItemFont { font-size:  90%; font-family: Helvetica, Arial, sans-serif }</style>\n";
print FILE "<table border=\"0\" width=\"100%\"><tr><td nowrap><font class=\"FrameItemFont\">\n";
print FILE "<b><a href=\"allClasses.html\">All Classes</a> / <a href=\"allClasses-light.html\">Most Classes</a></b></br></br>\n";
foreach $key (sort {uc($a) cmp uc($b)} keys %classes) {
    print FILE $classes{$key};
}
print FILE "</font></td></tr></body></html>\n";
close FILE;
system "grep -v '/undocumented.html' $rootdir/doc/api/allClasses.html > $rootdir/doc/api/allClasses-light.html";

# generate simple static index.html
open (FILE,">$rootdir/doc/api/index.html");
if($rootdir=~/.*\/(.*)$/) {
    $project=$1;
}
print FILE "<html><head><title>$project API docs</title><frameset cols=\"20%,80%\"><frame src=\"allClasses.html\"><frame src=\"sections.html\" name=\"main\"></frameset>\n</head></html>";
close (FILE);

# generate sections.html
open (FILE,">$rootdir/doc/api/sections.html");
print FILE "<html><body><ul>\n";
foreach $section (@sections) {
    $sect=$section;
    $sect=~s/\//-/;
    # find out errorcount;
    &parseErrorLog("$rootdir/doc/api/$sect", "err.log", $section);
    print FILE "<li><a href=\"$sect/annotated.html\">$section</a>&nbsp;&nbsp;<font size=\"-2\">(<a href=\"$sect/annotated.html\" target=\"_top\">NoFrames</a>)  ";
    if($error > 0) {
        print FILE "[<a href=\"$sect/errors.html\">$error errors</a>] ";
    }
    if($undocumented > 0) {
        print FILE "[<a href=\"$sect/undocumented.html\">$undocumented undocumented</a>]";
    }
    print FILE "</font></li>\n";
}
print FILE "</ul></body></html>\n";
close (FILE);
print "\n";


##############
sub alterConf() {
    open(INPUT, "$doxygenconftmp") || die "internal error: $doxygenconftmp missing!";
    open(FILE, ">$doxygenconftmp.2");
    foreach $line (<INPUT>) {
        if($line=~/^GENERATE_HTML/) {
            print FILE "GENERATE_HTML=YES\n";
        }
        elsif(! ($line=~/^GENERATE_TAGFILE/)) {
            print FILE $line;
        }
    }
    close INPUT;
    close FILE;
}

sub createConf() {
    my $name = shift(@_);
    open FILE, ">$doxygenconftmp";

    # copy arguments from Mainpage.dox
    open INPUT, "Mainpage.dox";
    my $comment=0;
    foreach $in (<INPUT>) {
        if($in=~/\/\*/) { $comment=1; }
        if($in=~/\*\//) {$comment=0; next; }
        if($comment == 1) { next; }
        $in=~s/^\/\/\s*//;
        print FILE $in;
    }

    print FILE "INPUT=";
    dirs: foreach $dir (@_) {
        chomp($dir);
        $dir=~s/^\.\///;
        if($dir=~/tests$/) { next; }
        foreach $sect (@sections) {
            # if another section is a subdir of this one; don't include its dirs.
            $target="$name/$dir";
            #print "name: '$name', target: '$target', sect: '$sect'\n";
            if($sect ne $name && $target=~m/$sect/ && !($name=~m/^$sect/)) {
                #print "  skipping, '$sect' is in the start of '$target'\n";
                next dirs;
            }
        }
        #print "USING '$dir'\n";
        print FILE "$dir ";
    }
    print FILE "\n";
    print FILE "PROJECT_NAME=$name\n";
    #print FILE "PROJECT_NUMBER=\n";
    my $output="$name";
    $output=~s/\//-/g;
    print FILE "OUTPUT_DIRECTORY=$basedir$output\n";
    #print FILE "CREATE_SUBDIRS=YES\n";
    #print FILE "OUTPUT_LANGUAGE=English\n";
    #print FILE "USE_WINDOWS_ENCODING=NO\n";
    print FILE "BRIEF_MEMBER_DESC=YES\n";
    print FILE "REPEAT_BRIEF=YES\n";
    #print FILE "ABBREVIATE_BRIEF=\n";
    #print FILE "ALWAYS_DETAILED_SEC=NO\n";
    #print FILE "INLINE_INHERITED_MEMB=NO\n";
    print FILE "FULL_PATH_NAMES=YES\n";
    print FILE "STRIP_FROM_PATH=$rootdir\n";
    #print FILE "STRIP_FROM_INC_PATH=\n";
    #print FILE "SHORT_NAMES=NO\n";
    print FILE "JAVADOC_AUTOBRIEF=YES\n";
    #print FILE "MULTILINE_CPP_IS_BRIEF=NO\n";
    #print FILE "DETAILS_AT_TOP=NO\n";
    #print FILE "INHERIT_DOCS=NO\n";
    #print FILE "SEPARATE_MEMBER_PAGES=NO\n";
    #print FILE "TAB_SIZE=4\n";
    #print FILE "ALIASES=\n";
    #print FILE "OPTIMIZE_OUTPUT_FOR_C=NO\n";
    #print FILE "OPTIMIZE_OUTPUT_JAVA=NO\n";
    #print FILE "BUILTIN_STL_SUPPORT=NO\n";
    #print FILE "DISTRIBUTE_GROUP_DOC=NO\n";
    #print FILE "SUBGROUPING=YES\n";
    #print FILE "EXTRACT_ALL=NO\n";
    #print FILE "EXTRACT_PRIVATE=NO\n";
    #print FILE "EXTRACT_STATIC=NO\n";
    print FILE "EXTRACT_LOCAL_CLASSES=NO\n";
    #print FILE "EXTRACT_LOCAL_METHODS=NO\n";
    #print FILE "HIDE_UNDOC_MEMBERS=NO\n";
    #print FILE "HIDE_UNDOC_CLASSES=NO\n";
    #print FILE "HIDE_FRIEND_COMPOUNDS=NO\n";
    #print FILE "HIDE_IN_BODY_DOCS=NO\n";
    #print FILE "INTERNAL_DOCS=NO\n";
    #print FILE "CASE_SENSE_NAMES=YES\n";
    #print FILE "HIDE_SCOPE_NAMES=NO\n";
    #print FILE "SHOW_INCLUDE_FILES=YES\n";
    #print FILE "INLINE_INFO=YES\n";
    #print FILE "SORT_MEMBER_DOCS=YES\n";
    #print FILE "SORT_BRIEF_DOCS=NO\n";
    #print FILE "SORT_BY_SCOPE_NAME=NO\n";
    #print FILE "GENERATE_TODOLIST=YES\n";
    #print FILE "GENERATE_TESTLIST=YES\n";
    #print FILE "GENERATE_BUGLIST=YES\n";
    #print FILE "GENERATE_DEPRECATEDLIST=YES\n";
    #print FILE "ENABLED_SECTIONS= \n";
    #print FILE "MAX_INITIALIZER_LINES=30\n";
    #print FILE "SHOW_USED_FILES=YES\n";
    #print FILE "SHOW_DIRECTORIES=NO\n";
    #print FILE "FILE_VERSION_FILTER=\n";
    #print FILE "QUIET=YES\n";
    #print FILE "WARNINGS=YES\n";
    #print FILE "WARN_IF_UNDOCUMENTED=YES\n";
    #print FILE "WARN_IF_DOC_ERROR=YES\n";
    #print FILE "WARN_NO_PARAMDOC=NO\n";
    #print FILE "WARN_FORMAT=\"\$file:\$line: \$text\"\n";
    #print FILE "WARN_LOGFILE=\n";
    #print FILE "FILE_PATTERNS=*.h\n";
    #print FILE "RECURSIVE=NO\n";
    #print FILE "EXCLUDE=\n";
    #print FILE "EXCLUDE_SYMLINKS=NO\n";
    #print FILE "EXCLUDE_PATTERNS=\n";
    #print FILE "EXAMPLE_PATH=\n";
    #print FILE "EXAMPLE_PATTERNS=\n";
    #print FILE "EXAMPLE_RECURSIVE=NO\n";
    print FILE "IMAGE_PATH=$basedir\n";
    #print FILE "INPUT_FILTER=\n";
    #print FILE "FILTER_PATTERNS=\n";
    #print FILE "FILTER_SOURCE_FILES=NO\n";
    #print FILE "SOURCE_BROWSER=NO\n";
    #print FILE "INLINE_SOURCES=NO\n";
    #print FILE "STRIP_CODE_COMMENTS=YES\n";
    #print FILE "REFERENCED_BY_RELATION=YES\n";
    #print FILE "REFERENCES_RELATION=YES\n";
    #print FILE "USE_HTAGS=NO\n";
    print FILE "VERBATIM_HEADERS=NO\n";
    #print FILE "ALPHABETICAL_INDEX=NO\n";
    #print FILE "COLS_IN_ALPHA_INDEX=5\n";
    #print FILE "IGNORE_PREFIX=\n";
    print FILE "GENERATE_HTML=NO\n"; # no since we enabled this in the alterConf()
    print FILE "HTML_OUTPUT=.\n";
    #print FILE "HTML_FILE_EXTENSION=.html\n";
    #print FILE "HTML_HEADER=\n";
    #print FILE "HTML_FOOTER=\n";
    #print FILE "HTML_STYLESHEET=\n";
    #print FILE "HTML_ALIGN_MEMBERS=YES\n";
    #print FILE "GENERATE_HTMLHELP=NO\n";
    #print FILE "CHM_FILE=\n";
    #print FILE "HHC_LOCATION=\n";
    #print FILE "GENERATE_CHI=NO\n";
    #print FILE "BINARY_TOC=YES\n";
    #print FILE "TOC_EXPAND=NO\n";
    #print FILE "DISABLE_INDEX=NO\n";
    #print FILE "ENUM_VALUES_PER_LINE=4\n";
    #print FILE "GENERATE_TREEVIEW=NO\n";
    #print FILE "TREEVIEW_WIDTH=250\n";
    print FILE "GENERATE_LATEX=NO\n";
    #print FILE "GENERATE_RTF=NO\n";
    #print FILE "GENERATE_MAN=NO\n";
    #print FILE "GENERATE_XML=NO\n";
    #print FILE "GENERATE_AUTOGEN_DEF=NO\n";
    #print FILE "GENERATE_PERLMOD=NO\n";
    #print FILE "ENABLE_PREPROCESSING=YES\n";
    #print FILE "MACRO_EXPANSION=NO\n";
    #print FILE "EXPAND_ONLY_PREDEF=NO\n";
    #print FILE "SEARCH_INCLUDES=YES\n";
    #print FILE "INCLUDE_PATH=\n";
    #print FILE "INCLUDE_FILE_PATTERNS=\n";
    #print FILE "PREDEFINED=\n";
    #print FILE "EXPAND_AS_DEFINED=\n";
    #print FILE "SKIP_FUNCTION_MACROS=YES\n";
    print FILE "TAGFILES=";
    foreach $sect (@sections) {
        # one tag for each section
        my $tagfile="$sect\_TAGS";
        my $section=$sect;
        $section=~s/\//-/g;
        $tagfile=~s/\//_/g;
        if($sect ne $name) {
            print FILE "\"$basedir$section/$tagfile=../$section\" ";
        }
    }
    print FILE "\n";
    $tagfile="$name\_TAGS";
    $tagfile=~s/\//_/g;
    print FILE "GENERATE_TAGFILE=$basedir$output/$tagfile\n";
    #print FILE "ALLEXTERNALS=NO\n";
    #print FILE "EXTERNAL_GROUPS=YES\n";
    #print FILE "PERL_PATH=/usr/bin/perl\n";
    #print FILE "CLASS_DIAGRAMS=YES\n";
    #print FILE "HIDE_UNDOC_RELATIONS=YES\n";
    #print FILE "HAVE_DOT=NO\n";
    #print FILE "CLASS_GRAPH=YES\n";
    #print FILE "COLLABORATION_GRAPH=YES\n";
    #print FILE "GROUP_GRAPHS=YES\n";
    #print FILE "UML_LOOK=NO\n";
    #print FILE "TEMPLATE_RELATIONS=NO\n";
    #print FILE "INCLUDE_GRAPH=YES\n";
    #print FILE "INCLUDED_BY_GRAPH=YES\n";
    #print FILE "CALL_GRAPH=NO\n";
    #print FILE "GRAPHICAL_HIERARCHY=YES\n";
    #print FILE "DIRECTORY_GRAPH=YES\n";
    #print FILE "DOT_IMAGE_FORMAT=png\n";
    #print FILE "DOT_PATH=\n";
    #print FILE "DOTFILE_DIRS=\n";
    #print FILE "MAX_DOT_GRAPH_WIDTH=1024\n";
    #print FILE "MAX_DOT_GRAPH_HEIGHT=1024\n";
    #print FILE "MAX_DOT_GRAPH_DEPTH=0\n";
    #print FILE "DOT_TRANSPARENT=NO\n";
    #print FILE "DOT_MULTI_TARGETS=NO\n";
    #print FILE "GENERATE_LEGEND=YES\n";
    #print FILE "DOT_CLEANUP=YES\n";
    #print FILE "SEARCHENGINE=NO\n";
    close FILE;
}

# writes the undocumented and error pages
sub parseErrorLog() {
    my $dir = shift(@_);
    my $logfile = shift(@_);
    my $section = shift(@_);
    $undocumented=0;
    $error=0;
    open INPUT,"$dir/$logfile" || die "Can't read logfile '$dir/$logfile'\n";
    open UNDOC, ">$dir/undocumented.html";
    print UNDOC "<html><body><h2>$section: undocumented constructs found</h2><ul>Please consider typing some docs, or adding a \\internal tag to the headerfile as soon as you find out what these items do or mean. Thanks!\n";
    open ERRORS, ">$dir/errors.html";
    $inParameters=0;
    foreach $line (<INPUT>) {
        if($line=~/^\S*$/) {
            if($inParameters == 1) {
                print UNDOC "</ul></li>\n";
                $inParameters = 0;
            }
            next;
        }
        chomp($line);
        if($inParameters == 1) {
            print UNDOC "<li>$line</li>\n";
            next;
        }
        if($line=~m/^(.*):(\d+): (.*)$/) {
            my $file=$1;
            my $lineNumber=$2;
            my $message=$3;
            $file=~s/^$rootdir/$project/;
            if($message=~m/Warning: (Compound|Member) (.*) is not documented/) {
                $undocumented++;
                print UNDOC &printFile(1, $file);
                print UNDOC "<li>$lineNumber: $1 $2</li>\n";
                if($1 eq "Compound") {
                    &addClass("", $dir, $2, "Class");
                }
                next;
            }
            if($message=~m/Warning: The following parameters of (.*) are not documented:/) {
                $inParameters = 1;
                $undocumented++;
                print UNDOC "<li>$lineNumber: parameters of $1<ul>\n";
                next;
            }
            if($message=~m/Warning: argument `(.*)' of command \@param is not found in the argument list of /) {
                $error++;
                print ERRORS &printFile(2, $file);
                print ERRORS "<li>$lineNumber: argument $1 is not present in argument list</li>\n";
                next;
            }
            if($message=~m/Warning: Internal inconsistency: /) {
                # ignore.
                next;
            }
            $error++;
            print ERRORS &printFile(2, $file);
            print ERRORS "<li>$lineNumber: $message\n";
            next;
        }
        $error++;
        print ERRORS "$line<br>\n";
    }
    close ERRORS;
    if($error == 0) {
        unlink "$dir/errors.html";
    }
    close UNDOC;
    if($undocumented == 0) {
        unlink "$dir/undocumented.html";
    }
    close INPUT;
}

sub printFile() {
    my $type = shift(@_);
    my $filename = shift(@_);
    if($filename eq ($type==1?$lastfile:$lasterrorfile)) {
        return "";
    }
    if($type==1) {
        $lastfile = $filename;
    } else {
        $lasterrorfile = $filename;
    }
    return $string."</ul><b>$filename</b><ul>";
}

sub addClass() {
    my $filename=shift(@_);
    my $section=shift(@_);
    my $className=shift(@_);
    my $classType = shift(@_);

    my $subsection=$section;
    $section=~s/\//-/g;

    if($classType eq "Class" && $className=~/^(.*)::/) {
        $subsection.="/$1";
        $className=~s/^.*:://;
    }
    my $string;
    my $target;
    if($filename eq "") {
        $string = "<a href=\"$section/undocumented.html\" title=\"$classType in $subsection\" target=\"main\"><font color=\"red\" target=\"main\">"
    } else {
        $string = "<a href=\"$section/$filename\" title=\"$classType in $subsection\" target=\"main\">";
    }
    if($classType eq "Namespace") { $string .="<i>"; }
    $string .= $className;
    if($classType eq "Namespace") { $string .= "</i>"; }
    if($filename eq "") {
        $string .="</font>"
    }
    $string .= "</a><br>\n";

    $classes{$className}=$string;
}
