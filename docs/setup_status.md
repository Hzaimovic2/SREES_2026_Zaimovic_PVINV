# Setup status

Datum provjere: 2026-06-22

## Instalirani alati

- Git: `2.54.0.windows.1`
- CMake: `4.3.2`
- Visual Studio Community: `18.7.0`
- MSVC compiler: `19.51.36247` za x64
- MSBuild: dostupan kroz Visual Studio instalaciju
- 7-Zip: dostupan na `C:\Program Files\7-Zip\7z.exe`
- dTwin: instaliran na `C:\Program Files\IDz\Digital Twin\dTwin.exe`

## Preuzeti GitHub fajlovi

Preuzeto u `downloads/release-assets`:

- `natIDSDKAllInOne_win_x64_20260511.7z`
- `dTwin_win_x64_20260511.7z`

`downloads/` je namjerno dodan u `.gitignore`, jer SDK i instalacioni paketi ne treba da idu u GitHub repozitorij seminarskog rada.

Napomena: direktni `git clone`/`curl` pokusaji su u ovom okruzenju padali zbog Windows TLS greske `schannel: AcquireCredentialsHandle failed`. Fajlovi su zato preuzeti preko Python download skripte.

## Raspakovano

Raspakovano u `downloads/natIDSDK_20260511`:

- `natID.SDK`
- `natID.Examples/dTwin/Plugin`
- `natID.Examples/dTwin/MatrixTests`

Ovi folderi sadrze profesorove primjere za:

- dTwin plugin DLL
- dense matrice
- sparse matrice

## Provjera naseg projekta

Build radi kada se prije CMake/MSBuild poziva normalizuje `Path`, jer trenutni terminal ima dupli `PATH/Path` environment kljuc koji zbunjuje MSBuild.

PowerShell:

```powershell
$savedPath = $env:Path
Remove-Item Env:PATH -ErrorAction SilentlyContinue
$env:Path = $savedPath

cmake -S src -B build-vsdev-cleanenv -G "Visual Studio 18 2026" -A x64
cmake --build build-vsdev-cleanenv --config Debug
.\build-vsdev-cleanenv\Debug\srees_solar_inverter.exe .\src\config\solar_inverter.xml .\src\cases\case9.xml
```

Provjereni rezultat:

```text
Case: case9 (9 buses)
PV AC power: 7.2 MW
PV reactive power: 1.46202 MVAr
Connection bus: 5
```

## natID build i plugin

natID varijanta je buildana sa:

```powershell
cmake -S src -B build-natid-plugin -G "Visual Studio 18 2026" -A x64 `
  -DSREES_USE_NATID=ON `
  -DSREES_BUILD_PLUGIN=ON `
  -DNATID_SDK_ROOT="...\downloads\natIDSDK_20260511\natID.SDK"
cmake --build build-natid-plugin --config Release
```

Provjereno:

- `srees_solar_inverter.exe` koristi `natID dense::DblMatrix + sparse::IDblMatrix`
- `srees_pvinv_plugin.dll` se builda kao Release DLL
- DLL eksportuje `getPluginInterface`
- Release DLL je kopiran u `C:\Users\Surface\ba.natID\dTwin\plugins\srees_pvinv_plugin.dll`
- Windows `LoadLibrary` test prolazi kada je dTwin folder postavljen kao DLL directory
- plugin ima GUI: izbor XML konfiguracije, case XML-a, izlaznog `.dmodl` fajla i `Preview` prikaz izracunatih vrijednosti

NatID testni slucajevi `case9`, `case30`, `case118` i `case300` su pokrenuti. Za svaki case su generisani `.dmodl` i `.vmodl` izlazi u `docs/generated_examples`. Rezultati su zapisani u `docs/test_results.md`.

Rucna provjera u dTwin aplikaciji je opisana u `docs/manual_dtwin_check.md`.

## Napomena za natID primjere

natID SDK i primjeri su preuzeti i CMake vidi compiler. Medjutim, dva primjera iz SDK-a imaju CMake helper pozive koji nisu definisani u isporucenom `Common.cmake`:

- `TestDense` poziva `setIDEPropertiesForExecutableWithAVX2`
- `EQPlugin` poziva `setIDEPropertiesForLib`

To nije blokada za nas projekt. Za nas CMake plugin target mozemo napisati direktno, po istom linkanju biblioteka kao u primjeru, bez oslanjanja na te helper funkcije.

U istom release-u nedostaju i headeri `matrix/MatrixLib.h`, `BLAS/Platform.h` i `arch/MemoryOut.h`, iako ih SDK headeri referenciraju. Za nas projekt su zato dodani mali compatibility headeri u `src/natid_compat`, bez mijenjanja originalnog SDK-a.
