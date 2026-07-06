# Test results

Datum: 2026-06-21

Build:

- `SREES_USE_NATID=ON`
- `SREES_BUILD_PLUGIN=ON`
- Visual Studio 18 2026, x64, Release
- natID SDK: `natIDSDKAllInOne_win_x64_20260511`

Konfiguracija solarne elektrane:

- prikljucni cvor: 5
- DC nazivna snaga: 8.0 MW
- AC nazivna snaga invertera: 7.2 MW
- iradijansa: 950 W/m2
- temperatura celije: 32 C
- faktor snage: 0.98

## Rezultati

| Case | Broj cvorova | Aktivna snaga | Reaktivna snaga | Dense matrica | Sparse matrica |
| --- | ---: | ---: | ---: | --- | --- |
| case9 | 9 | 7.2 MW | 1.46202 MVAr | 9x9 | 9x9, 26 nonzeros |
| case30 | 30 | 7.2 MW | 1.46202 MVAr | 30x30 | 30x30, 89 nonzeros |
| case118 | 118 | 7.2 MW | 1.46202 MVAr | 118x118 | 118x118, 353 nonzeros |
| case300 | 300 | 7.2 MW | 1.46202 MVAr | 300x300 | 300x300, 899 nonzeros |

Backend matrica:

```text
natID dense::DblMatrix + sparse::IDblMatrix
```

Generisan je i probni dTwin izlaz:

```text
docs/generated_examples/case9_solar_inverter.dmodl
docs/generated_examples/case9_solar_inverter.vmodl
```

Ispravka: `.dmodl` model je prebacen na dinamicki DAE oblik sa istim brojem varijabli i ODE jednacina. Snage invertera krecu od nule i prate referentne vrijednosti kroz vremensku konstantu `inverter_tau`:

```text
p_ac' = (p_ac_ref - p_ac) / inverter_tau
q_ac' = (q_ac_ref - q_ac) / inverter_tau
p_dc' = (p_dc_ref - p_dc) / inverter_tau
irradiance' = 0
cell_temp' = 0
```

Vizuelni model prikazuje dinamicki odziv `P ac`, `Reactive Q` i `P dc`, te dodatni normalizovani odziv u per-unit obliku. Radna tacka PV sistema, ukljucujuci iradijansu i temperaturu celije, prikazuje se u `Preview` dijelu GUI-a prije konverzije.

Napomena: ovo su pocetni testni slucajevi za konverziju i verifikaciju strukture. Sljedeci korak je prosirenje ulaznih case fajlova stvarnim podacima mreze i dodatno dotjerivanje grafickog prikaza.
