# natID/dTwin integracija

Prema profesorovom uputstvu potrebno je koristiti:

- natID GUI
- natID dense matrice za pune matrice
- natID sparse matrice za rijetke matrice
- dTwin plugin u formi DLL-a

GitHub release koji je profesor poslao:

- natID `v4.1.0`, release `natID_20260511`
- dTwin `v1.2.47`, release `2026.05.11`

Napomena iz release-a: natID 4.1.0 dodaje plugin support, dTwin progress indikatore i primjere za dense/sparse matrix manipulaciju u `natID.Examples/dTwin/MatrixTests`.

## Sljedeci koraci

1. Preuzeti ili raspakovati natID SDK. Status: uradjeno lokalno u `downloads/natIDSDK_20260511`.
2. Provjeriti dTwin instalaciju. Status: instaliran u `C:\Program Files\IDz\Digital Twin`.
3. Provjeriti primjer `natID.Examples/dTwin/Plugin`. Status: preuzet i pregledan.
4. Provjeriti primjer `natID.Examples/dTwin/MatrixTests`. Status: preuzet i pregledan.
5. Prebaciti interne matrice iz ovog projekta na natID dense/sparse tipove.
6. Dodati DLL plugin target u CMake. Status: uradjeno, target `srees_pvinv_plugin`.
7. Kopirati plugin u korisnicki dTwin plugin folder `$HOME/ba.natID/dTwin/plugins`, kako bi se pojavio u dTwin meniju `Model -> Import`. Status: uradjeno lokalno za Release DLL.

## Trenutni plugin status

Plugin DLL se builda i eksportuje funkciju `getPluginInterface`. Implementiran je pocetni GUI prozor po uzoru na `natID.Examples/dTwin/Plugin/EQPlugin`, sa izborom XML konfiguracije, case fajla i izlaznog `.dmodl` fajla.

Trenutni GUI tok:

1. Izaberi `solar_inverter.xml`.
2. Izaberi jedan od case fajlova.
3. Izaberi izlazni `.dmodl`.
4. Klikni `Convert`.

Plugin zatim koristi isti C++ model kao console aplikacija, generise `.dmodl` i pocetni `.vmodl` fajl.

## Pravilo za kod

Ostatak modela treba ostati nezavisan od GUI-a. natID/dTwin sloj treba samo:

- procitati XML konfiguraciju,
- pozvati konverziju,
- prikazati real-time progress,
- vratiti rezultat u dTwin model.
