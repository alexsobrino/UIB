#!/usr/bin/perl

# tratamiento del /etc/pam.conf
open(PAMCONF,"/etc/pam.conf") || die "ERROR: Imposible abrir el /etc/pam.conf";
while (<PAMCONF>) {
	chomp;
	if (/^[^#]/) {
		@linea = split;
		print "Servicio: $linea[0]  Tipo de módulo: $linea[1]  Flag de control: $linea[2]  Path al módulo: $linea[3]  Argumentos: $linea[4]";
	}
}
close(PAMCONF);

# tratamiento del /etc/pam.d/
$pamd_dir="/etc/pam.d";
opendir(PAMDIR,$pamd_dir) || die "ERROR: Imposible abrir el directorio $pamd_dir";
while ($nombre = readdir(PAMDIR)) {
	if ($nombre =~ /^[^.]/) {
		open(PAMFILE,$pamd_dir."/".$nombre) || die "ERROR: Imposible abrir el fichero $pamd_dir."/".$nombre";
		while (<PAMFILE>) {
			chomp;
			if (/^[^#]/) {
				if (/^[@]/) {
					print "Include ($nombre): $_\n";
				} else {
					@linea = split;
					print "Servicio: $nombre  Tipo de módulo: $linea[0]  Flag de control: $linea[1]  Path al módulo: $linea[2]  Argumentos: $linea[3].\n";
				}
			}
		}
		print "\n";
		close(PAMFILE);
	}
}
closedir(PAMDIR);
