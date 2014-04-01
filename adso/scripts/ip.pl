#!/usr/bin/perl

open(IFCONFIG,"/sbin/ifconfig|") || die "ERROR: Imposible ejecutar el binario.";

while (<IFCONFIG>) {
	chomp;
	if (/^[^\s]/) {
		@linea = split;
		print "Interfaz: $linea[0]\n";
	}
	if (/inet addr/) {
		@linea = split;
		@linea = split (/:/,$linea[1]);
		print "IP: $linea[1]\n\n";
	}
}
close(IFCONFIG);
