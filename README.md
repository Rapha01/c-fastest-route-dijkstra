# c-fastest-route-dijkstra

Finde die schnellste Verbindung ab vonbahnhof und vonstunde:vonminute
nach nachbahnhof. Gib für jeden genommenen Zug die Zugnummer, Abfahrts-
und Ankunftsbahnhof, sowie die Abfahrts- und Ankunftszeit in richtiger
Reihenfolge aus.

Implementierungstipp: Jeder Bahnhof bekommt einen Eintrag mit bestem
Von-Bahnhof, Zugnummer, Abfahrts- und Ankunftszeit (Pointer auf
Fahrplan?). Am Anfang sind alle Einträge als nicht-erledigt
gekennzeichnet, bis auf den vonbahnhof, der vonstunde:vonminute als
Ankunftszeit bekommt. Finde dann den Bahnhof, der am frühesten von
einem erledigten Bahnhof aus erreichbar ist. Trage Zugnummer und Von-
Bahnhof in diesen Bahnhof ein und kennzeichne ihn als erledigt. Wenn die
beste Zugnummer beim Von-Bahnhof die gleiche ist, trage den Von-Bahnhof
des Von-Bahnhofs in den neuen Bahnhof ein. Wiederhole das, bis
nachbahnhof erledigt ist.  Verfolge die Kette von Von-Bahnhöfen zurück
zu vonbahnhof und gib sie in umgekehrter Reihenfolge aus.
