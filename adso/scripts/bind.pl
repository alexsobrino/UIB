#!/usr/bin/perl

if (`ps aux |grep named |grep -v grep`) {
	open(NAMED,"/etc/bind/named.conf") || die "ERROR: Imposible abrir el fichero /etc/bind/named.conf";
	while (<NAMED>) {
		chomp;
		@linea = split;
		if ($linea[0] eq "forwarders") {
			print "Bind ejecutándose como caché de DNS.\n";
		} elsif ($linea[0] eq "type") {
			@linea = split /;/,$linea[1];
			print "Bind ejecutándose como $linea[0].\n";
		}
	}
	close(NAMED);
} else {
	print "Servidor de DNS (bind) no se está ejecutando.\n";
}
