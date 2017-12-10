# SpeedRacing

![Could not display screenshot](https://i.imgur.com/ZGGrUel.jpg)

Igrica voznje automobila po kruznoj stazi koja se nasumicno generise. Za generisanje staze koriscen je Catmull-Rom splajn koji je radjen na predavanjima, takodje za detekciju da li je igrac na asfaltu je koriscen algoritam sa predavanja scan linije (pronalazi da li je tacka u okviru poligona, modifikacija na popunjavanje poligona scan linijama).

Za izradu projekta je korisceno okruzenje CLion koje se oslanja na CMake. Na kraju izrade je iskoriscen CMake da izgenerise standardni Makefile.

Projekat iz Racunarske Grafike, napravljen u jeziku C koristeci OpenGL.

***
## :package: Instalacija
U konzoli navigirajte do foldera u koji ste skinuli ovaj projekat (gde se nalazi **Makefile**) i onda kompajlirajte koriscenjem komande **make**:
```
make
```
:exclamation: **Upozorenje:** Kako bi **make** radio, morate imati OpenGL/GLUT biblioteke instalirane.


Pokrenite **SpeedRacing** izvrsni fajl:
```
./SpeedRacing
```

***
## :video_game: Kontrole

<table>
  <tr>
    <th>Dugme</th><th>Akcija</th>
  </tr>
  <tr>
    <td>Strelica Gore</td><td>Ubrzavanje</td>
  </tr>
  <tr>
    <td>Strelica Dole</td><td>Kocenje</td>
  </tr>
  <tr>
    <td>Strelica Levo</td><td>Okretanje tockova levo</td>
  </tr> 
  <tr>
    <td>Strelica Desno</td><td>Okretanje tockova desno</td>
  </tr>
  <tr>
    <td>ESC</td><td>Izlazak iz igrice</td>
  </tr>
</table>