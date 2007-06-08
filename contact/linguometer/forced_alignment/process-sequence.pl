#!/usr/bin/perl -w

use strict;

my $wavdir = $ARGV[0];

my $ct = 0;
while (<STDIN>) {
    chomp($_);
    if (!($_ =~ /^\#/)) {
	my ($idx, $ignore, $txt, @rem) = split(/\;/,$_);
	my $fname = sprintf("%s/wd_%04d_us.wav",$wavdir,$idx);
	print "number $ct, string $txt ($fname)\n";
	if (1 || $ct==46) {
	    system "mkdir -p tmp";
	    system "cp $fname /tmp/input.wav";
	    system "echo $fname > tmp/source.txt";
	    open(FOUT,">/tmp/input.txt");
	    print FOUT $txt;
	    close(FOUT);
	    system "make img";
	    system sprintf("mv tmp/current.gif /tmp/wd_%04d_us.gif",$idx);
	}
	$ct++;
    }
}



