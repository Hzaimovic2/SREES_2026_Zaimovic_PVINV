# Rucna provjera dTwin plugin-a

Ovaj dio se provjerava rucno u dTwin aplikaciji.

## Prije provjere

Plugin DLL mora biti na lokaciji:

```text
C:\Users\Surface\ba.natID\dTwin\plugins\srees_pvinv_plugin.dll
```

Ako dTwin vec radi, zatvoriti ga i ponovo otvoriti nakon kopiranja DLL-a.

## Koraci u dTwin-u

Postoje dvije razlicite stvari:

- plugin DLL se pokrece preko `Model -> Import`,
- vec generisani `.dmodl` fajl se otvara kao model fajl, ne kao plugin import.

## A) Provjera plugin-a

1. Otvoriti dTwin.
2. U meniju otvoriti `Model -> Import`.
3. Provjeriti da li postoji stavka:

```text
Solar inverter converter
```

4. Kliknuti tu stavku.
5. U prozoru plugin-a odabrati:

```text
Config XML: C:\Users\Surface\OneDrive\Documents\SREES Solarna elektrana + inverter\src\config\solar_inverter.xml
Case XML:   C:\Users\Surface\OneDrive\Documents\SREES Solarna elektrana + inverter\src\cases\case9.xml
Output:     C:\Users\Surface\OneDrive\Documents\SREES Solarna elektrana + inverter\docs\generated_examples\case9_from_dtwin.dmodl
```

Alternativno, nakon izbora `Config XML`, mogu se koristiti brza dugmad `case9`, `case30`, `case118` ili `case300`. Plugin tada automatski popunjava `Case XML` i predlaze izlazni `.dmodl` fajl.

6. Kliknuti `Preview` i provjeriti sazetak:

```text
PV AC power: 7.2 MW
PV reactive power: 1.46202 MVAr
Matrix backend: natID dense::DblMatrix + sparse::IDblMatrix
```

7. Kliknuti `Convert`.

## Ocekivani rezultat

Plugin treba generisati:

```text
case9_from_dtwin.dmodl
case9_from_dtwin.vmodl
```

U dTwin-u treba da se otvore plotovi:

- `PV inverter dynamic response` sa krivama `P ac`, `Reactive Q` i `P dc`,
- `Normalized inverter response` sa per-unit odzivom `P ac pu`, `Q pu` i `P dc pu`.

GUI dodatno ima:

- `Reset` za brisanje trenutnog unosa,
- automatski prijedlog output fajla,
- scenarije `Sunny`, `Cloudy`, `Hot day` i `Low PF`,
- rezime `Auto defaults`, `Adapt to case` i `Manual edit`,
- rucni unos prikljucnog busa, iradijanse, temperature i faktora snage,
- izbor da li dTwin generise `MW/MVAr` plot, `p.u.` plot ili oba,
- status/progress poruke tokom konverzije,
- pregled solarne radne tacke i natID matrica prije generisanja modela,
- indikativnu procjenu naponskog uticaja i osnovna upozorenja.

`Auto defaults` je namijenjen za najbrzu demonstraciju bez rucnog kucanja. `Adapt to case` bira jednostavne razumne parametre prema odabranom testnom slucaju. `Manual edit` se koristi kada se zeli eksplicitno promijeniti bus, iradijansa, temperatura ili faktor snage.

## B) Otvaranje vec generisanog modela

Ako zelis otvoriti model koji je vec generisan bez plugin GUI-a, koristi:

```text
docs\generated_examples\case9_solar_inverter.dmodl
```

Taj fajl treba otvoriti kao model fajl u dTwin-u, ne kroz `Model -> Import` plugin meni. Ako dTwin pita za vizuelni model, uz njega ide:

```text
docs\generated_examples\case9_solar_inverter.vmodl
```

Ako plugin ne vidis u meniju, provjeriti:

- da li je DLL kopiran u `C:\Users\Surface\ba.natID\dTwin\plugins`,
- da li je dTwin restartovan nakon kopiranja,
- da li je buildana Release verzija DLL-a, ne Debug.

## Probni izlaz bez dTwin-a

Console aplikacija vec generise primjer:

```text
docs\generated_examples\case9_solar_inverter.dmodl
docs\generated_examples\case9_solar_inverter.vmodl
```

Napomena: prethodna verzija ovog probnog `.dmodl` fajla je imala vise varijabli nego jednacina, pa je dTwin javljao `Number of variables is not equal to number of equations`. Ispravljena verzija ima pet varijabli i pet ODE jednacina. Trenutna verzija prikazuje dinamicki odziv invertera, pa `P ac`, `Q ac` i `P dc` krecu od nule i priblizavaju se izracunatim referentnim vrijednostima.
