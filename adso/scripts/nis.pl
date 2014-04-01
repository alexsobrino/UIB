#!/usr/bin/perl

# Información extraída de;
# - http://breu.bulma.net/?l3117 [tldp.org]
# - http://bulma.net/body.phtml?nIdNoticia=1841 [bulma.net]

open (RPCINFO,"/usr/bin/rpcinfo -p|") || die "ERROR: Imposible ejecutar el binario.\n";
while (<RPCINFO>) {
	chomp;
	if (/ypbind/) {
		print "Cliente NIS ejecutándose.\n";
		open (YPCONF,"/etc/yp.conf");
		while (<YPCONF>) {
			chomp;
			@linea = split;
			if ($linea[0] eq "domain") {
				print "Dominio: $linea[1]\n";
				if ($linea[2] eq "broadcast") {
					print "Servidor: broadcast\n";
				} elsif ($linea[2] eq "server") {
					print "Servidor: $linea[3]\n";
				}
			}
		}
	}
}
close(RPCINFO);
