#!/usr/bin/ruby
#
# Ruby script for generating tarball releases of the Plan repository
# This script can create signed tarballs with source code and translations.
# Documentation is not supported atm.
#
# (c) 2017 Dag Andersen <danders@get2net.dk>
# (c) 2016 Dag Andersen <danders@get2net.dk>
#
# Parts of this script is from create_tarball_kf5.rb, copyright by:
# (c) 2005 Mark Kretschmann <markey@web.de>
# (c) 2006-2008 Tom Albers <tomalbers@kde.nl>
# (c) 2007 Angelo Naselli <anaselli@linux.it> (command line parameters)
# Some parts of this code taken from cvs2dist
#
# License: GNU General Public License V2

require 'optparse'
require 'ostruct'
require 'find'
require 'fileutils'

# check command line parameters
options = OpenStruct.new
options.help  = false
options.sign  = false
options.program = "gpg2"
options.translations = true
options.docs = false
options.languages = []
options.tag = "HEAD"
options.infolevel = 0
options.checkversion = true

opts = OptionParser.new do |opts|
    opts.on_tail("-h", "--help", "Show this usage statement") do |h|
        options.help = true
    end
    opts.on("-v", "--version <version>", "Package version (Default: no version)") do |v|
        options.version = v
    end
    opts.on("-c", "--cstring <version string>", "Version string to check against version string in CMakeList.txt (Default: use version given in --version option)") do |c|
        options.cstring = c
    end
    opts.on("-n", "--no-check", "Disable version check") do |n|
        options.checkversion = false;
    end
    opts.on("-g", "--gittag <tag>", "Git tag (Default: 'HEAD')") do |g|
        options.tag = g
    end
    opts.on("-t", "--no-translations", "Do not include translations (Default: translations included)") do |t|
        options.translations = false
    end
    opts.on("-d", "--docs", "TODO Include documentation (Default: docs not included)") do |d|
        # TODO
        #options.translations = true
    end
    opts.on("-s", "--sign", "Sign tarball (Default: tarball is not signed)") do |s|
        options.sign = true
    end
    opts.on("-p", "--program <program>", "Which program to use for signing (Default: gpg2)") do |p|
        options.program = p
    end
    opts.on("-l", "--languages <language,..>", "Include comma separated list of languages only (Default: All available languages)") do |l|
        options.languages = l.split(/\s*,\s*/)
    end
    opts.on("-i", "--infolevel <level>", "Select amount of info to print during processing (0-2) (Default: 0)") do |i|
        options.infolevel = i.to_i
    end
end

begin
  opts.parse!(ARGV)
rescue Exception => e
  puts e, "", opts
  puts
  exit
end

if (options.help)
  puts
  puts opts
  puts
  exit
end

############# START #############
    
app = "calligraplan"

puts
puts "-> Processing " + app
puts  "            Git tag: #{options.tag}"
puts  "            Version: #{options.version}"
puts  "     Version string: #{options.cstring}"
puts  "             Signed: #{options.sign}"
puts  "            Program: #{options.program}"
puts  "       Translations: #{options.translations}"
print "          Languages: "
if (options.languages.empty?)
    puts "all"
else
    puts "#{options.languages}"
end
puts "      Documentation: #{options.docs}"
puts

gitdir = "calligraplan"
if options.version
    gitdir += "-" + options.version
end
gittar = "#{gitdir}.tar.xz"
gitsig = "#{gittar}.sig"

# clean up first, in case
calligradir = ".calligra"
`rm -rf #{calligradir} 2> /dev/null`
`rm -rf #{gitdir} 2> /dev/null`
if File.exist?(gittar)
    File.delete(gittar)
end
if File.exist?(gitsig)
    File.delete(gitsig)
end

Dir.mkdir(calligradir)
Dir.chdir(calligradir)
puts "-> Fetching git archive tag=#{options.tag} .."
`git archive --remote git://anongit.kde.org/calligra.git #{options.tag} | tar -x`
Dir.chdir("..")

# get plan and get rid of rest of calligra
`mv #{calligradir}/plan #{gitdir}`
`rm -rf #{calligradir}`

Dir.chdir(gitdir)

if !File.exist?("CMakeLists.txt")
    puts
    puts "Failed: 'git archive' failed to fetch repository"
    puts
    exit
end

if options.checkversion
    cversion=`grep '(PLAN_VERSION_STRING' CMakeLists.txt | cut -d'"' -f2`
    cversion = cversion.delete("\n").delete("\r").strip
    cstring = options.version
    if options.cstring
        cstring = options.cstring
    end
    if cversion != cstring
        puts
        puts "Failed: Specified version is not the same as in CMakeLists.txt"
        puts "        Specified version: '#{cstring}'"
        puts "        CMakeLists.txt   : '#{cversion}'"
        puts
        puts "        Did you forget to update version in CMakeLists.txt?"
        puts
        puts "        You can disable this test with the option: --no-check"
        puts
        exit
    end
end

# translations
if options.translations
    
    svnbase = "svn+ssh://svn@svn.kde.org/home/kde"
    
    if options.branch == "trunk"
        svnroot = "#{svnbase}/trunk"
    else
        svnroot = "#{svnbase}/branches/stable"
    end
    rev = ""
    
    puts "-> Fetching po file names .."
    Dir.mkdir("po")
    if FileTest.exist?("po_tmp")
        `rm -rf "po_tmp"`
    end
    Dir.mkdir("po_tmp")
    pofilenames = "po_tmp/pofilenames"
    `x=$(find $gitdir -name 'Messages.sh' | while read messagefile; do \
            if grep -q '^potfilename=' $messagefile; then \
                cat $messagefile | grep '^potfilename=' | cut -d'=' -f2 | cut -d'.' -f1; \
            fi; \
        done);\
    echo "$x" >#{pofilenames}`

    if !File.size?(pofilenames)
        puts "Failed: Could not fetch any po file names"
        exit
    end
    if options.infolevel > 0
        c = `wc -l #{pofilenames} | cut -d' ' -f1`
        puts "     Number of po file names found: " + c
    end

    puts "-> Fetching translations .."

    # get languages
    i18nlangs = `svn cat #{svnroot}/l10n-kf5/subdirs #{rev}`.split
    i18nlangsCleaned = []
    for lang in i18nlangs
        l = lang.chomp
        if !options.languages.empty?
            if options.languages.include?(l)
                i18nlangsCleaned += [l]
            end
        else l != "x-test" && !i18nlangsCleaned.include?(l)
            i18nlangsCleaned += [l]
        end
    end
    i18nlangs = i18nlangsCleaned

    if FileTest.exist?("po")
        `rm -rf "po"`
    end
    Dir.mkdir("po")
    for lang in i18nlangs
        lang.chomp!
        tmp = "po_tmp/#{lang}"
        dest = "po/#{lang}"

        # always checkout all po-files
        print "  -> Fetching #{lang} from repository ..\n"
        pofolder = "l10n-kf5/#{lang}/messages/calligra"
        if options.infolevel > 0
            `svn co #{svnroot}/#{pofolder} #{tmp}`
        else
            `svn co #{svnroot}/#{pofolder} #{tmp} 2>/dev/null`
        end

        # copy over the po-files we actually use in calligraplan
        File.foreach(pofilenames) do |pofile|
            pofile.chomp!
            pofilepath = "#{tmp}/#{pofile}.po"
            if !FileTest.exist?(pofilepath)
                # all files have not always been translated
                if options.infolevel > 1
                    puts "     Skipping #{pofilepath} .."
                end
                next
            end
            if !FileTest.exist?(dest)
                Dir.mkdir(dest)
            end
            if FileTest.exist?(pofilepath)
                if options.infolevel > 0
                    puts "     Copying #{pofile}.po .."
                end
                `mv #{pofilepath} #{dest}`
            end
        end
    end
    # remove temporary po dir
    `rm -rf "po_tmp"`
    
    # add l10n to compilation.
    `echo "find_package(KF5I18n CONFIG REQUIRED)" >> CMakeLists.txt`
    `echo "ki18n_install(po)" >> CMakeLists.txt`

    if options.docs
        # add docs to compilation.
        `echo "find_package(KF5DocTools CONFIG REQUIRED)" >> CMakeLists.txt`
        `echo "kdoctools_install(po)" >> CMakeLists.txt`
    end
end

# Remove cruft
`find -name ".svn" | xargs rm -rf`

Dir.chdir( ".." ) # root folder

print "-> Compressing ..  "
`tar -Jcf #{gittar} --group=root --owner=root  #{gitdir}`
puts " done."
puts ""
print "md5sum: ", `md5sum #{gittar}`
print "sha256sum: ", `sha256sum #{gittar}`

if (options.sign)
    puts "-> Signing ..  "
    `#{options.program} -a --output #{gitsig} --detach-sign #{gittar}`
    puts ""
    print "sha256sum: ", `sha256sum #{gitsig}`
end
