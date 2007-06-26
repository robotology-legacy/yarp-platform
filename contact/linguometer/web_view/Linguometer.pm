
# data subdirectory should point to wherever linguometer data is
# you'll need to give loose permissions on that directory
sub getBase() {
    return "data";
}

sub setupLinguometer {
    my $verbose = shift;
    my $base = getBase();
    if (!(-e "$base/all.idx")) {
	if ($verbose) {
	    print "Setting up index (just happens once)<br>\n";
	}
	system "find $base/*.txt -exec grep -H . {} \\; > $base/all.idx";
    }
    return 1;
}

return 1;

