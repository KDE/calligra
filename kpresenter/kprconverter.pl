#!/usr/bin/perl

# converts a KPresenter document from the old format to the new one (v2)
# due to the new text object

use Time::Local;

open(INPUT, "<$ARGV[0]") || die "Cannot open $ARGV[0]";
open(OUTPUT, ">$ARGV[1]") || die "Cannot create $ARGV[1]";
$objType="";
$insideParag=0;
$insideObj=0;
$insidePixmaps=0;  # are we inside the <PIXMAPS> tags?
$currentText="";
$currentTextType=0;
while (<INPUT>)
{
  study;  # This speeds up the whole RE stuff because Perl creates some hash for the string
  if (/<DOC/)
    {
      # store the url because this is a prart of the "path" for the images
      $url=$1 if(m/url=\"(.*?)\"/);
      s/>$/ syntaxVersion=\"2\">/;
    }
  elsif (/<PIXMAPS>/)
    {
      $insidePixmaps=1;
    }
  elsif (/<\/PIXMAPS>/)
    {
      $insidePixmaps=0;
    }
  elsif (/<TEXTOBJ/)
    {
      # Save object type of the TEXTOBJ tag
      $objType=$1 if (m/objType=(\"[0-9]+\")/);
      s/gap=/margin=/;
    }
  elsif (/<PARAGRAPH/)
    {
      $insideParag=1;
      s/<PARAGRAPH /<P /;
      # In the old format we had horzAlign="[0|1|2]" 0=left, 1=center, 2=right
      # In the new one it's align="..." and uses the Qt::AlignmentFlags enums.
      # Qt::AlignLeft=1, AlignRight=2, AlignHCenter=4
      $alignment=$1 if(/horzAlign=(\"[0-2]+\")/);
      $alignment =~ tr/01/14/;
      s/horzAlign=\"[0-2]+\"/align=$alignment/;
      s/>$/ type=$objType>/;
    }
  elsif (/<\/PARAGRAPH>/)
    {
      $insideParag=0;
      s/<\/PARAGRAPH/<\/P/;
      # Flush last text tag
      $_ = $currentText . "</TEXT>\n" . $_ if ($currentText);
      $currentText="";
    }
  elsif (/<LINE/ || /<\/LINE/)
    {
      $_ = "" if ($insideParag); # ignore
    }
  elsif (/<OBJ>/)
    {
      $insideObj=1;
      $_ = ""; # ignore
    }
  elsif (/<\/OBJ>/)
    {
      $insideObj=0;
      $_ = ""; # ignore
    }
  elsif ($insideObj)
    {
      $toprint="";
      if (m/<TYPE value="([0-9]+)"/)
	{
	  $currentTextType=$1;
	  if ($currentTextType) # 1 -> this is a white space
	    {
	      # If we have a previous text element, we keep it (merging)
	      # Otherwise this white space is the first one in the object -> cheat
	      $currentTextType=0 if (!$currentText);
	    }
	  if (!$currentTextType) # 0 -> normal text
	    {
	      # If we have a previous text element, write it out
	      $toprint = $currentText . "</TEXT>\n" if ($currentText);
	      # Start a new text element
	      $currentText = "      <TEXT ";
	    }
	}
      elsif (/<FONT/ && !$currentTextType) # normal text
	{
	  s/\s*<FONT //;
	  s/\/>//;
	  chomp;
	  # Append all attributes
	  $currentText .= $_;  # the same as $currentText = $currentText . $_, but faster
	}
      elsif (/<COLOR/ && !$currentTextType) # normal text
	{
	  $red=$1 if (m/red=\"([0-9]+)\"/);
	  $green=$1 if (m/green=\"([0-9]+)\"/);
	  $blue=$1 if (m/blue=\"([0-9]+)\"/);
	  # Convert color to HTML representation
	  $currentText .= sprintf(" color=\"#%02x%02x%02x\"", $red, $green, $blue );
	}
      elsif (m/<TEXT>(.*)<\/TEXT>/)
	{
	  if (!$currentTextType) # normal text
	    {
	      # Close opening tag and append the text - but don t do more yet
	      @entities=split( "(&[a-z]+;)", $1);
	      $text="";
	      foreach ( @entities )
		{
		  if (!/&lt;/ && !/&gt;/ )
		    {
		      # Replace & by &amp; but only if not in an entity
		      s/\&/\&amp;/g;
		    }
		  $text .= $_;
		}
	      $currentText .= ">" . $text;
	    }
	  else
	    {
	      # White space. Simply appending, closing the text tag.
	      $toprint = $currentText . $1 . "</TEXT>\n";
	      $currentText = ""; # reset
	    }
	}
      $_=$toprint;
    }
  elsif ($insidePixmaps)
    {
      if(/\s+name=\"/)
        {
          # Aha - this file is not version 2 but was created as "proper" tgz storage... medieval times :)
          print "Found a name attribute, no need to create one.\n";
        }
      elsif (/<KEY(.*)\/>/)
        {
          # Okay - plain old kpresenter magic...
          $key=$1;
          study($key); # should be faster ;)
          # Note: The .*? is needed because it would be too greedy otherwise
          $filename=$1 if($key =~ /filename=\"(.*?)\"/);
          # Get the values - really straightforward
          $year=$1 if($key =~ /year=\"(\d+)\"/);
          $month=$1 if($key =~ /month=\"(\d+)\"/);
          $day=$1 if($key =~ /day=\"(\d+)\"/);
          $hour=$1 if($key =~ /hour=\"(\d+)\"/);
          $minute=$1 if($key =~ /minute=\"(\d+)\"/);
          $second=$1 if($key =~ /second=\"(\d+)\"/);
          # In Perl the month is <0...11>!!!
          $timestamp=timegm($second, $minute, $hour, $day, $month-1, $year);
          # Unfortunately we even have to mess with that string...
          $timestring=scalar gmtime($timestamp);
          # There are still some spaces too much when day<10
          $timestring =~ s/  / /;
          # Okay. Now let's cat the whole caboodle...
          $nameattrib=$url . $filename . "_" . $timestring;
          # ...and put it in place.
          s/\/>/ name=\"$nameattrib\" \/>/;
        }
    }

  print OUTPUT $_;
}

close(INPUT);
close(OUTPUT);
