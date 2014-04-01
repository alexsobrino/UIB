#!/usr/bin/perl

open(INETD,"/etc/inetd.conf") || die "ERROR: Imposible abrir el fichero /etc/inetd.conf";

while (<INETD>) {
	chomp;
	if (/^[^#]/) {
		@servicio = split;
		if ($servicio[6]) {
			print "Servicio: $servicio[0]\nTipo del socket: $servicio[1]\nProtocolo: $servicio[2]\nMarcas: $servicio[3]\nUsuario: $servicio[4]\nPath al servidor: $servicio[5]\nArgumentos: $servicio[6]\n\n";
		} else {
			print "Servicio: $servicio[0]\nTipo del socket: $servicio[1]\nProtocolo: $servicio[2]\nMarcas: $servicio[3]\nUsuario: $servicio[4]\nPath al servidor: $servicio[5]\nArgumentos: ninguno\n\n";
		}
	}
}
close(INETD);
