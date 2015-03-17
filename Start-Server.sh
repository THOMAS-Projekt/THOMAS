#!/bin/bash
# Diese Datei sorgt dafür, dass der Server neugestartet wird,
# sobald dieser crashen sollte -> Notaus etc.

# Dauerschleife
while true;
do
	# THOMAS-Server Software aufrufen
	output=$("./thomas")

	# Fehler in Syslog schreiben
	logger "[THOMAS] Programm abgestürzt - Fehler:"
	logger "[THOMAS] "$output

	# Fehler ausgeben
	echo $output

	# 10 Sekunden warten
	sleep 10
done
