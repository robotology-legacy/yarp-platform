#!/usr/bin/perl -w

use strict;
use GD;

my $img_in = new GD::Image("tmp/map.gif");
my $img = $img_in->clone();
my $color = $img->colorAllocate(0,0,128);
my $color2 = $img->colorAllocate(0,0,255);

my $scale = `cat tmp/scale.txt | grep -v "\#"`;
my $name = `cat tmp/label.txt`;
my $fname = `cat tmp/source.txt || echo 9999/9999/9999`;
chomp($fname);
chomp($name);
$fname=~ s/.*exp_//;
$fname=~ s/seq_//;
$fname=~ s/wd_([0-9]+).*/$1/;
$fname=~ s/0/O/g;

open(FIN,"<tmp/labelled.phseg");
while (<FIN>) {
    chomp($_);
    my ($start, $stop, $label, @rem) = split(/[ \t]+/,$_);
    my $x = int((($start+$stop)/2)*$scale)-
	length($label)*gdMediumBoldFont->width/2;
    if ($label ne "SIL") {
	$img->string(gdMediumBoldFont,$x+2,20,$label,$color);
    }
}

$img->string(gdMediumBoldFont,10,10,"\"".$name."\"",$color2);
if ($fname =~ /([0-9O]+)\/([0-9O]+)\/([0-9O]+)/) {
    my $n1 = "E " . $1;
    my $n2 = "S " . $2;
    my $n3 = "W " . $3;
    $img->string(gdMediumBoldFont,10,20,$n1,$color2);
    $img->string(gdMediumBoldFont,10,30,$n2,$color2);
    $img->string(gdMediumBoldFont,10,40,$n3,$color2);
}

open(FOUT,">tmp/current.gif");
print FOUT $img->gif;
close(FOUT);

print "***\n";
print "*** Annotated image in tmp/current.gif\n";
print "***\n";
