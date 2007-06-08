#!/usr/bin/perl -w

use strict;


my %dict;

open(FREF,"<$ARGV[0]") || die "need a dictionary\n";
while (<FREF>) {
    chomp($_);
    my ($in, $out) = split(/\t/,$_);
    #print "$in --> $out\n";
    $dict{$in} = $out;
}
close(FREF);

my $txt = "";
while (<STDIN>) {
    my $ph = $_;
    $ph =~ s/[^a-z0-9_]//gi;
    my $phout = $dict{$ph};
    if (!defined($phout)) {
	print STDERR "Do not recognize $ph\n";
	exit(1);
    }
    $txt = $txt . "$phout ";
}
$txt =~ s/(_LY_ )+/L Y /g;
$txt =~ s/(_NY_ )+/N Y /g;
$txt =~ s/(_TS_ )+/T S /g;
print "$txt\n";

