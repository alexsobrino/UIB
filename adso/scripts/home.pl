#!/usr/bin/perl

# Información obtenida de man 2 stat

@perm_oct=(7,4,2);

open (PASSWD,"/etc/passwd") || die "ERROR: Imposible abrir el fichero /etc/passwd";

while (<PASSWD>) {
	chomp;
	@linea = split /:/;
	if (-e $linea[5]) {
		$perm_fic = (stat("$linea[5]"))[2];
		$own_uid = (stat("$linea[5]"))[4];
		if ($own_uid != $linea[2]) {
			print "UID del directorio $linea[5] incorrecto.\n";
		}
		# Permisos de propietario
		if ($perm_oct[0] == 0) {
			if (($perm_fic & 00400) || ($perm_fic & 00200) || ($perm_fic & 00100)) {
				print "Permisos del directorio $linea[5] incorrectos (owner).\n";
			}
		} elsif ($perm_oct[0] == 1) {
			if (($perm_fic & 00400) || ($perm_fic & 00200) || !($perm_fic & 00100)) {
				print "Permisos del directorio $linea[5] incorrectos (owner).\n";
			}
		} elsif ($perm_oct[0] == 2) {
			if (($perm_fic & 00400) || !($perm_fic & 00200) || ($perm_fic & 00100)) {
				print "Permisos del directorio $linea[5] incorrectos (owner).\n";
			}
		} elsif ($perm_oct[0] == 3) {
			if (($perm_fic & 00400) || !($perm_fic & 00200) || !($perm_fic & 00100)) {
				print "Permisos del directorio $linea[5] incorrectos (owner).\n";
			}
		} elsif ($perm_oct[0] == 4) {
			if (!($perm_fic & 00400) || ($perm_fic & 00200) || ($perm_fic & 00100)) {
				print "Permisos del directorio $linea[5] incorrectos (owner).\n";
			}
		} elsif ($perm_oct[0] == 5) {
			if (!($perm_fic & 00400) || ($perm_fic & 00200) || !($perm_fic & 00100)) {
				print "Permisos del directorio $linea[5] incorrectos (owner).\n";
			}
		} elsif ($perm_oct[0] == 6) {
			if (!($perm_fic & 00400) || !($perm_fic & 00200) || ($perm_fic & 00100)) {
				print "Permisos del directorio $linea[5] incorrectos (owner).\n";
			}
		} elsif ($perm_oct[0] == 7) {
			if (!($perm_fic & 00400) || !($perm_fic & 00200) || !($perm_fic & 00100)) {
				print "Permisos del directorio $linea[5] incorrectos (owner).\n";
			}
		}
		# Permisos de grupo
		if ($perm_oct[1] == 0) {
			if (($perm_fic & 00040) || ($perm_fic & 00020) || ($perm_fic & 00010)) {
				print "Permisos del directorio $linea[5] incorrectos (group).\n";
			}
		} elsif ($perm_oct[1] == 1) {
			if (($perm_fic & 00040) || ($perm_fic & 00020) || !($perm_fic & 00010)) {
				print "Permisos del directorio $linea[5] incorrectos (group).\n";
			}
		} elsif ($perm_oct[1] == 2) {
			if (($perm_fic & 00040) || !($perm_fic & 00020) || ($perm_fic & 00010)) {
				print "Permisos del directorio $linea[5] incorrectos (group).\n";
			}
		} elsif ($perm_oct[1] == 3) {
			if (($perm_fic & 00040) || !($perm_fic & 00020) || !($perm_fic & 00010)) {
				print "Permisos del directorio $linea[5] incorrectos (group).\n";
			}
		} elsif ($perm_oct[1] == 4) {
			if (!($perm_fic & 00040) || ($perm_fic & 00020) || ($perm_fic & 00010)) {
				print "Permisos del directorio $linea[5] incorrectos (group).\n";
			}
		} elsif ($perm_oct[1] == 5) {
			if (!($perm_fic & 00040) || ($perm_fic & 00020) || !($perm_fic & 00010)) {
				print "Permisos del directorio $linea[5] incorrectos (group).\n";
			}
		} elsif ($perm_oct[1] == 6) {
			if (!($perm_fic & 00040) || !($perm_fic & 00020) || ($perm_fic & 00010)) {
				print "Permisos del directorio $linea[5] incorrectos (group).\n";
			}
		} elsif ($perm_oct[1] == 7) {
			if (!($perm_fic & 00040) || !($perm_fic & 00020) || !($perm_fic & 00010)) {
				print "Permisos del directorio $linea[5] incorrectos (group).\n";
			}
		}
		# Permisos otros
		if ($perm_oct[2] == 0) {
			if (($perm_fic & 00004) || ($perm_fic & 00002) || ($perm_fic & 00001)) {
				print "Permisos del directorio $linea[5] incorrectos (others).\n";
			}
		} elsif ($perm_oct[2] == 1) {
			if (($perm_fic & 00004) || ($perm_fic & 00002) || !($perm_fic & 00001)) {
				print "Permisos del directorio $linea[5] incorrectos (others).\n";
			}
		} elsif ($perm_oct[2] == 2) {
			if (($perm_fic & 00004) || !($perm_fic & 00002) || ($perm_fic & 00001)) {
				print "Permisos del directorio $linea[5] incorrectos (others).\n";
			}
		} elsif ($perm_oct[2] == 3) {
			if (($perm_fic & 00004) || !($perm_fic & 00002) || !($perm_fic & 00001)) {
				print "Permisos del directorio $linea[5] incorrectos (others).\n";
			}
		} elsif ($perm_oct[2] == 4) {
			if (!($perm_fic & 00004) || ($perm_fic & 00002) || ($perm_fic & 00001)) {
				print "Permisos del directorio $linea[5] incorrectos (others).\n";
			}
		} elsif ($perm_oct[2] == 5) {
			if (!($perm_fic & 00004) || ($perm_fic & 00002) || !($perm_fic & 00001)) {
				print "Permisos del directorio $linea[5] incorrectos (others).\n";
			}
		} elsif ($perm_oct[2] == 6) {
			if (!($perm_fic & 00004) || !($perm_fic & 00002) || ($perm_fic & 00001)) {
				print "Permisos del directorio $linea[5] incorrectos (others).\n";
			}
		} elsif ($perm_oct[2] == 7) {
			if (!($perm_fic & 00004) || !($perm_fic & 00002) || !($perm_fic & 00001)) {
				print "Permisos del directorio $linea[5] incorrectos (others).\n";
			}
		}
	}
}
close(PASSWD);
