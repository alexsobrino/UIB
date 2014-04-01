#!/usr/bin/perl

open (NETSTAT,"/bin/netstat -a --inet|") || die "ERROR: Imposible ejecutar el binario.";

while (<NETSTAT>) {
	chomp;
	@linea = split;
	@serv = split /:/,$linea[3];
	if (/LISTEN/) {
		print "Servidor escuchando ($linea[0]) -> Servicio/Puerto: $serv[1].\n"
	} else {
		print "Conexión ($linea[0]) -> Servicio/Puerto: $serv[1]  Cliente: $linea[4].\n";
	}
}
close(NETSTAT);
