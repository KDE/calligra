#!/usr/bin/perl
# Copyright Thomas Zander <zander@kde.org>
# this script generates dox based on the availability of a Mainpage.dox file.
# whereever there is one doxygen is started.


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
@sections=`find . -name Mainpage.dox -type f`;

# for each section
print "$#sections found\n";
$i=1;
foreach $section (@sections) {
    chomp($section);

    # skip root dir while there are so little Mainpage.dox files
    if($section eq "Mainpage.dox") {
        next;
    }
    $section=~s/^\.\///;
    $section=~s/\/Mainpage.dox//;
    print "Doing section $i: '$section'\n";
    $i++;

    chdir $section;
    print "   step 1/2\n";
    @dirs=`find . -type d | grep -v .svn | grep -v _darcs`;
    print "   step 2/2\n";
    &createConf($section, @dirs);
    system "mkdir -p \"$basedir$section\"";
    symlink "Mainpage.dox", "92sdlkfjsdlkjf.h";
    system "/usr/bin/doxygen $doxygenconftmp >/dev/null 2>$basedir$section/err.log";
    unlink "$doxygenconftmp";
    unlink "92sdlkfjsdlkjf.h";
    chdir "$rootdir";
}

sub createConf() {
    my $name = shift(@_);
    open FILE, ">$doxygenconftmp";
    print FILE "INPUT=";
    dirs: foreach $dir (@_) {
        chomp($dir);
        $dir=~s/^\.\///;
        foreach $sect (@sections) {
            # if another section is a subdir of this one; don't include its dirs.
            chomp($sect);
            $sect=~s/^\.\///;
            $sect=~s/\/Mainpage.dox//;
#print "-> '$name/$dir'\n";
#print "   '$sect'\n";
           if($sect ne $name && $sect=~m/$name\/$dir/) {
                #print "  skipping, '$sect' is in the start of '$name/$dir'\n";
                next dirs;
           }
        }
#print "   ** '$dir'\n";
        print FILE "$dir ";
    }
    print FILE "\n";
    print FILE "PROJECT_NAME=$name\n";
    print FILE "PROJECT_NUMBER=\n";
    print FILE "OUTPUT_DIRECTORY=$basedir$name\n";
    print FILE "CREATE_SUBDIRS=YES\n";
    print FILE "OUTPUT_LANGUAGE=English\n";
    print FILE "USE_WINDOWS_ENCODING=NO\n";
    print FILE "BRIEF_MEMBER_DESC=YES\n";
    print FILE "REPEAT_BRIEF=YES\n";
    print FILE "ABBREVIATE_BRIEF=\n";
    print FILE "ALWAYS_DETAILED_SEC=NO\n";
    print FILE "INLINE_INHERITED_MEMB=NO\n";
    print FILE "FULL_PATH_NAMES=YES\n";
    print FILE "STRIP_FROM_PATH=\n";
    print FILE "STRIP_FROM_INC_PATH=\n";
    print FILE "SHORT_NAMES=NO\n";
    print FILE "JAVADOC_AUTOBRIEF=YES\n";
    print FILE "MULTILINE_CPP_IS_BRIEF=NO\n";
    print FILE "DETAILS_AT_TOP=NO\n";
    print FILE "INHERIT_DOCS=YES\n";
    print FILE "SEPARATE_MEMBER_PAGES=NO\n";
    print FILE "TAB_SIZE=4\n";
    print FILE "ALIASES=\n";
    print FILE "OPTIMIZE_OUTPUT_FOR_C=NO\n";
    print FILE "OPTIMIZE_OUTPUT_JAVA=NO\n";
    print FILE "BUILTIN_STL_SUPPORT=NO\n";
    print FILE "DISTRIBUTE_GROUP_DOC=NO\n";
    print FILE "SUBGROUPING=YES\n";
    print FILE "EXTRACT_ALL=NO\n";
    print FILE "EXTRACT_PRIVATE=NO\n";
    print FILE "EXTRACT_STATIC=NO\n";
    print FILE "EXTRACT_LOCAL_CLASSES=YES\n";
    print FILE "EXTRACT_LOCAL_METHODS=NO\n";
    print FILE "HIDE_UNDOC_MEMBERS=NO\n";
    print FILE "HIDE_UNDOC_CLASSES=NO\n";
    print FILE "HIDE_FRIEND_COMPOUNDS=NO\n";
    print FILE "HIDE_IN_BODY_DOCS=NO\n";
    print FILE "INTERNAL_DOCS=NO\n";
    print FILE "CASE_SENSE_NAMES=YES\n";
    print FILE "HIDE_SCOPE_NAMES=NO\n";
    print FILE "SHOW_INCLUDE_FILES=YES\n";
    print FILE "INLINE_INFO=YES\n";
    print FILE "SORT_MEMBER_DOCS=YES\n";
    print FILE "SORT_BRIEF_DOCS=NO\n";
    print FILE "SORT_BY_SCOPE_NAME=NO\n";
    print FILE "GENERATE_TODOLIST=YES\n";
    print FILE "GENERATE_TESTLIST=YES\n";
    print FILE "GENERATE_BUGLIST=YES\n";
    print FILE "GENERATE_DEPRECATEDLIST=YES\n";
    print FILE "ENABLED_SECTIONS= \n";
    print FILE "MAX_INITIALIZER_LINES=30\n";
    print FILE "SHOW_USED_FILES=YES\n";
    print FILE "SHOW_DIRECTORIES=NO\n";
    print FILE "FILE_VERSION_FILTER=\n";
    print FILE "QUIET=NO\n";
    print FILE "WARNINGS=YES\n";
    print FILE "WARN_IF_UNDOCUMENTED=YES\n";
    print FILE "WARN_IF_DOC_ERROR=YES\n";
    print FILE "WARN_NO_PARAMDOC=NO\n";
    print FILE "WARN_FORMAT=\"\$file:\$line: \$text\"\n";
    print FILE "WARN_LOGFILE=\n";
    print FILE "FILE_PATTERNS=*.h\n";
    print FILE "RECURSIVE=NO\n";
    print FILE "EXCLUDE=\n";
    print FILE "EXCLUDE_SYMLINKS=NO\n";
    print FILE "EXCLUDE_PATTERNS=\n";
    print FILE "EXAMPLE_PATH=\n";
    print FILE "EXAMPLE_PATTERNS=\n";
    print FILE "EXAMPLE_RECURSIVE=NO\n";
    print FILE "IMAGE_PATH=\n";
    print FILE "INPUT_FILTER=\n";
    print FILE "FILTER_PATTERNS=\n";
    print FILE "FILTER_SOURCE_FILES=NO\n";
    print FILE "SOURCE_BROWSER=NO\n";
    print FILE "INLINE_SOURCES=NO\n";
    print FILE "STRIP_CODE_COMMENTS=YES\n";
    print FILE "REFERENCED_BY_RELATION=YES\n";
    print FILE "REFERENCES_RELATION=YES\n";
    print FILE "USE_HTAGS=NO\n";
    print FILE "VERBATIM_HEADERS=YES\n";
    print FILE "ALPHABETICAL_INDEX=NO\n";
    print FILE "COLS_IN_ALPHA_INDEX=5\n";
    print FILE "IGNORE_PREFIX=\n";
    print FILE "GENERATE_HTML=YES\n";
    print FILE "HTML_OUTPUT=.\n";
    print FILE "HTML_FILE_EXTENSION=.html\n";
    print FILE "HTML_HEADER=\n";
    print FILE "HTML_FOOTER=\n";
    print FILE "HTML_STYLESHEET=\n";
    print FILE "HTML_ALIGN_MEMBERS=YES\n";
    print FILE "GENERATE_HTMLHELP=NO\n";
    print FILE "CHM_FILE=\n";
    print FILE "HHC_LOCATION=\n";
    print FILE "GENERATE_CHI=NO\n";
    print FILE "BINARY_TOC=YES\n";
    print FILE "TOC_EXPAND=NO\n";
    print FILE "DISABLE_INDEX=NO\n";
    print FILE "ENUM_VALUES_PER_LINE=4\n";
    print FILE "GENERATE_TREEVIEW=NO\n";
    print FILE "TREEVIEW_WIDTH=250\n";
    print FILE "GENERATE_LATEX=NO\n";
    print FILE "GENERATE_RTF=NO\n";
    print FILE "GENERATE_MAN=NO\n";
    print FILE "GENERATE_XML=NO\n";
    print FILE "GENERATE_AUTOGEN_DEF=NO\n";
    print FILE "GENERATE_PERLMOD=NO\n";
    print FILE "ENABLE_PREPROCESSING=YES\n";
    print FILE "MACRO_EXPANSION=NO\n";
    print FILE "EXPAND_ONLY_PREDEF=NO\n";
    print FILE "SEARCH_INCLUDES=YES\n";
    print FILE "INCLUDE_PATH=\n";
    print FILE "INCLUDE_FILE_PATTERNS=\n";
    print FILE "PREDEFINED=\n";
    print FILE "EXPAND_AS_DEFINED=\n";
    print FILE "SKIP_FUNCTION_MACROS=YES\n";
    print FILE "TAGFILES=\n";
    print FILE "GENERATE_TAGFILE=\n";
    print FILE "ALLEXTERNALS=NO\n";
    print FILE "EXTERNAL_GROUPS=YES\n";
    print FILE "PERL_PATH=/usr/bin/perl\n";
    print FILE "CLASS_DIAGRAMS=YES\n";
    print FILE "HIDE_UNDOC_RELATIONS=YES\n";
    print FILE "HAVE_DOT=NO\n";
    print FILE "CLASS_GRAPH=YES\n";
    print FILE "COLLABORATION_GRAPH=YES\n";
    print FILE "GROUP_GRAPHS=YES\n";
    print FILE "UML_LOOK=NO\n";
    print FILE "TEMPLATE_RELATIONS=NO\n";
    print FILE "INCLUDE_GRAPH=YES\n";
    print FILE "INCLUDED_BY_GRAPH=YES\n";
    print FILE "CALL_GRAPH=NO\n";
    print FILE "GRAPHICAL_HIERARCHY=YES\n";
    print FILE "DIRECTORY_GRAPH=YES\n";
    print FILE "DOT_IMAGE_FORMAT=png\n";
    print FILE "DOT_PATH=\n";
    print FILE "DOTFILE_DIRS=\n";
    print FILE "MAX_DOT_GRAPH_WIDTH=1024\n";
    print FILE "MAX_DOT_GRAPH_HEIGHT=1024\n";
    print FILE "MAX_DOT_GRAPH_DEPTH=0\n";
    print FILE "DOT_TRANSPARENT=NO\n";
    print FILE "DOT_MULTI_TARGETS=NO\n";
    print FILE "GENERATE_LEGEND=YES\n";
    print FILE "DOT_CLEANUP=YES\n";
    print FILE "SEARCHENGINE=NO\n";
    close FILE;
}
