#!/usr/bin/perl

open(PASSWD,"/etc/passwd") || die "ERROR: Imposible abrir el /etc/passwd";

while (<PASSWD>) {
	chomp;
	@linea = split /:/;
	if ($linea[1] eq '') {
		print "ERROR ($linea[0]): usuario sin contraseña.\n";
	}
	if (($linea[5] eq '') || !(-e $linea[5])) { 
		print "ERROR ($linea[0]): usuario sin home.\n";
	}
}
close(PASSWD);
