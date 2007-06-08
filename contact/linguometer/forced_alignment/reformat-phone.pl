#!/usr/bin/perl -w 

use strict;


sub nord {
    my $str = shift;
    my $idx = shift;
    my $o = 0;
    if ($idx<length($str)) {
	$o = ord(substr($str,$idx,1));
    }
    if ($o==0) { $o = ord(" "); }
    return $o;
}

my $txt = "";

my $samples = $ARGV[0] || die "need number of samples";

my @starts;
my @stops;
my @labs;
my $latest = 0;

while (<STDIN>) {
    chomp($_);
    if ($_ =~ /^[ \t]*([0-9]+)[ \t]+([0-9]+)[ \t]+[\-0-9]+ ([a-zA-Z0-9]+)/) {
	my $start = $1;
	my $stop = $2;
	my $lab = $3;
#	print ">>> $start $stop $lab\n";
	push(@starts, $start);
	push(@stops, $stop);
	push(@labs, $lab);
	$latest = $stop;
    }
}

my $scale = $samples/$latest;

for (my $i=0; $i<=$#starts; $i++) {
    my $start = int($starts[$i]*$scale);
    my $stop = int($stops[$i]*$scale);
    my $lab = $labs[$i];
    print STDERR "$start $stop $lab\n";
    print "$start $stop ", nord($lab,0), " ", nord($lab,1), " ", nord($lab,2), " ", "\n";
}

