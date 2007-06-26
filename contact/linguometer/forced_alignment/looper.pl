#!/usr/bin/perl -w

use strict;

# deal with I/O

my $inbase = $ARGV[0];
my $outbase = $ARGV[1];
my $code = $ARGV[2];
if (!defined($code)) {
    $code = "";
}

my $exp = "[0-9]+";
my $seq = "[0-9]+";
my $word = "[0-9]+";

if ($code=~/^([0-9]+)/) {
    $exp = sprintf("%04d",$1);
}


if ($code=~/^[0-9]*_([0-9]+)/) {
    $seq = sprintf("%04d",$1);
}

if ($code=~/^[0-9]*_[0-9]*_([0-9]+)/) {
    $word = sprintf("%04d",$1);
}

my $lst = `cd $inbase; find . -iregex "./exp_$exp/seq_$seq/wd_${word}_us.wav"`;
#print "cd $inbase; find . -wholename \"./exp_$exp/seq_$seq/wd_${word}_us.wav\"\n";
chomp($lst);

my @targets = split(/[ \t\n\r]+/,$lst);

print join("\n",@targets), "\n";

foreach my $target (@targets) {
    if ($target =~ /exp_([0-9]+)\/seq_([0-9]+)\/wd_([0-9]+)/) {
	my $e = $1;
	my $s = $2;
	my $w = $3;
	print "\n\n\nexp $e seq $s word $w\n";
	my $OUTDIR = "$outbase/all";
	my $LABELS = "$inbase/exp_$e/temp/ws_sequence_$s.txt";
	my $WAVDIR = "$inbase/exp_$e/seq_$s";
	system "mkdir -p $OUTDIR";
	system "rm -f /tmp/wd*.gif /tmp/wd*.txt";
	system "cat $LABELS | ./process-sequence.pl $WAVDIR $w";
	system "mv /tmp/wd*.gif $OUTDIR/${e}_${s}_${w}.gif";
	system "mv /tmp/wd*.txt $OUTDIR/${e}_${s}_${w}.seg";
	system "( cat tmp/label.txt; echo ) > $OUTDIR/${e}_${s}_${w}.txt";
	print "MOVED image  to $OUTDIR/${e}_${s}_${w}.gif\n";
	print "MOVED region to $OUTDIR/${e}_${s}_${w}.seg\n";
	print "MOVED label  to $OUTDIR/${e}_${s}_${w}.txt\n";
    }
}



