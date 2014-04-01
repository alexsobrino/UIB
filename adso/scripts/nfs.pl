#!/usr/bin/perl

# Información extraída de:
# - http://breu.bulma.net/?l3116 [tldp.org]
# - http://bulma.net/body.phtml?nIdNoticia=1841

open (RPCINFO,"/usr/bin/rpcinfo -p|") || die "ERROR: Imposible ejecutar el binario.\n";
while (<RPCINFO>) {
	chomp;
	if (/nfs/) {
		@linea = split;
		print "Servidor NFS ejecutandose (v.$linea[1])\n\n";
	}
}
close(RPCINFO);

open(MOUNT,"/bin/mount|") || die "ERROR: Imposible ejecutar el binario\n";
while (<MOUNT>) {
	chomp;
	@linea = split;
	if ($linea[4] eq "nfs") {
		@tmp = split /:/,$linea[0];
		print "Servidor: $tmp[0]\nDirectorio remoto: $tmp[1]\nPto. montaje: $linea[2]\n\n";
	}
}
close(MOUNT);
