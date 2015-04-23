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

	# Beep ausgeben
	beep -f 1400 -l 1500

	# 5 Sekunden warten
	sleep 5
done

