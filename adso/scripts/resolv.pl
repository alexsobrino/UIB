#!/usr/bin/perl

open(RESOLV,"/etc/resolv.conf") || die "ERROR: Imposible abrir el fichero /etc/resolv.conf";

while (<RESOLV>) {
	if (/^[^#]/) {
		@linea = split;
		if ($linea[0] eq "nameserver") {
			print "Servidor DNS: $linea[1]\n";
			$ok=1;
		} elsif ($linea[0] eq "domain") {
			print "Dominio: $linea[1]\n";
			$ok=1;
		}
	} 
}
close(RESOLV);	

if (!$ok) {
	print "ERROR: fichero /etc/resolv.conf sin configuración valida.\nMás información: man resolv.conf\n";
}
