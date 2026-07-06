# Sta treba znati objasniti

Ovo je kratka verzija za pripremu odbrane.

## Ideja teme

Tema je konvertor za model solarne elektrane sa inverterom. Konvertor cita XML konfiguraciju, racuna osnovne velicine PV/inverter modela i generise dTwin model koji se moze ucitati kroz plugin.

## Ulazi

Glavni ulazi su:

- XML konfiguracija solarne elektrane i invertera,
- testni case mreze,
- broj cvora na koji se prikljucuje solarna elektrana.

U konfiguraciji se zadaju:

- DC nazivna snaga PV polja,
- AC nazivna snaga invertera,
- iradijansa,
- temperatura celije,
- temperaturni koeficijent,
- faktor snage,
- prikljucni cvor.

## Matematicki model

Raspoloziva DC snaga:

```text
Pdc = Pdc,rated * (G / Gref) * (1 + kT * (Tcell - Tref))
```

AC snaga invertera:

```text
Pac = min(Pdc, Pac,rated)
```

Reaktivna snaga:

```text
Qac = Pac * tan(arccos(pf))
```

U dTwin modelu se ove vrijednosti koriste kao referentne vrijednosti dinamickog odziva invertera:

```text
p_ac' = (p_ac_ref - p_ac) / inverter_tau
q_ac' = (q_ac_ref - q_ac) / inverter_tau
p_dc' = (p_dc_ref - p_dc) / inverter_tau
```

Za trenutnu konfiguraciju:

```text
Pdc = 7.3872 MW
Pac = 7.2 MW
Qac = 1.46202 MVAr
```

## Zasto dense i sparse matrice

Dense matrica cuva sve elemente matrice, ukljucujuci nule. Korisna je za manje modele i direktan prikaz.

Sparse matrica cuva samo nenulte elemente. Elektroenergetske mreze su prirodno rijetke jer svaki cvor nije povezan sa svakim drugim cvorom. Zato sparse matrice stede memoriju i ubrzavaju proracune.

U projektu:

```text
dense::DblMatrix
sparse::IDblMatrix
```

koriste se iz natID SDK-a.

## Threadovi

Konverzija radi u jednom thread-u. Drugi thread cita progress i prikazuje stanje konverzije. Time se simulira real-time progress indikator koji profesor trazi.

## dTwin plugin

Plugin se builda kao DLL i kopira u:

```text
C:\Users\Surface\ba.natID\dTwin\plugins
```

dTwin ga treba prikazati u meniju:

```text
Model -> Import -> Solar inverter converter
```

Plugin ima GUI za izbor:

- konfiguracijskog XML fajla,
- case XML fajla,
- izlaznog `.dmodl` fajla,
- brzi izbor `case9`, `case30`, `case118` i `case300`,
- scenarije za iradijansu, temperaturu i faktor snage,
- automatski i adaptivni rezim kada korisnik ne zeli rucno unositi parametre,
- rucno podesavanje busa i PV/inverter parametara,
- automatski prijedlog output fajla,
- izbor obicnog i/ili normalizovanog p.u. prikaza,
- `Preview` pregled izracunatih vrijednosti prije konverzije,
- status/progress poruke, validacijska upozorenja i `Reset` kontrolu.

## Testni slucajevi

Pokrenuti su:

- case9,
- case30,
- case118,
- case300.

Svi koriste isti PV/inverter model, ali mreza ima razlicit broj cvorova, pa se mijenjaju dimenzije dense/sparse matrica.

## Najbolji kratak odgovor profesoru

Ovaj projekt implementira C++ konvertor za solarnu elektranu sa inverterom. Model se konfigurise XML fajlom, prikljucuje se na odabrani cvor testne mreze, racuna aktivnu i reaktivnu snagu invertera i koristi natID dense/sparse matrice za predstavljanje mreze. Izracunate snage se u dTwin-u prikazuju kao dinamicki odziv invertera prema referentnim vrijednostima. Konverzija se izvrsava u posebnom thread-u uz progress indikator, a rezultat se moze generisati kroz dTwin plugin u formi DLL-a.
