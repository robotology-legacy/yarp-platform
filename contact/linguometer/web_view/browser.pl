#!/usr/bin/perl -w

use strict;

use CGI;
use HTML::Template;

use Linguometer;

# get linguometer data directory
my $base = getBase();

my $q = new CGI;

my $filter = $q->param("filter");
my $exp = $q->param("exp");
my $seq = $q->param("seq");
my $wrd = $q->param("wrd");
my $exact = $q->param("exact");
my $start = $q->param("start");

my $first = 0;
my $last = 0;
my $total = 0;

if (!defined($filter)) { $filter = ""; }
if (!defined($exp)) { $exp = ""; }
if (!defined($seq)) { $seq = ""; }
if (!defined($wrd)) { $wrd = ""; }
if (!defined($exact)) { $exact = "0"; }
if (!defined($start)) { $start = "0"; }
my $next = "";

print $q->header();

setupLinguometer(1);

my $message = "";
my @items = ();

my $block = 100;
if (defined($filter)) {
    open(IDX,"<$base/all.idx");
    my $at = 0;
    while(<IDX>) {
	#print "$_";
	if ($_ =~ /$filter/) {
	    if ($_ =~ /(.*).txt:(.*)/) {
		my $fname = $1;
		my $txt = $2;
		my $img = "$fname.gif";
		my $ok = 1;
		if ($exact) {
		    my $qm = quotemeta($filter);
		    $ok &&= ($_ =~ /(.*).txt:[\n\r ]*$qm[\n\r ]*$/);
		}
		if ($exp ne "") {
		    $ok &&= ($fname =~ /^(.*\/)*0*${exp}_/);
		}
		if ($seq ne "") {
		    $ok &&= ($fname =~ /^(.*\/)*[0-9]*_0*${seq}_/);
		}
		if ($wrd ne "") {
		    $ok &&= ($fname =~ /^(.*\/)*[0-9]*_[0-9]*_0*${seq}$/);
		}
		if ($ok && (-e $img)) {
		    if ($at>=$start && $#items<$block-1) {
			my $e = "?";
			my $s = "?";
			my $w = "?";
			if ($fname=~/(.*\/)*([0-9]{4})_([0-9]{4})_([0-9]{4})$/) {
			    $e = $2;
			    $s = $3;
			    $w = $4;
			}
			push(@items, 
			     {
				 fname=>$fname, 
				 name=>$txt, 
				 image=>"$fname.gif",
				 exp => $e,
				 seq => $s,
				 wrd => $w
				 });
		    } else {
			if ($at>=$start) {
			    if ($next eq "") {
				$next = $at;
			    }
			}
		    }
		    $at++;
		}
	    }
	}
    }
    close(IDX);
    $total = $at;
    $first = $start+1;
    $last = $start+$#items+1;
}


my $template = HTML::Template->new(filename => "browser.html",
                                   global_vars => 1,
                                   die_on_bad_params => 0);

my $qNext = $q;  # should clone
$qNext->param("start" => $next);
my $nextUrl = $qNext->self_url;

my $baseUrl = "browser.pl";

$template->param(
                 message => $message,
		 filter => $filter,
		 exp => $exp,
		 seq => $seq,
		 wrd => $wrd,
		 exact => $exact,
		 start => $start,
		 "next" => $next,
		 nextUrl => $nextUrl,
		 first => $first,
		 "last" => $last,
		 total => $total,
		 baseUrl => $baseUrl,
		 items => \@items
                 );



# print the template
print $template->output;
