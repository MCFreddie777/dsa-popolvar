# Zadanie 3 – Popolvár

Popolvár je jednoduchý chlapec z dediny, ktorý celé dni nerobí nič iné 
ako vynášanie popola zo sporáka v kuchyni a programovanie pre dobročinné účely. 
Dnes, ako tak surfoval po internete, sa k nemu dostala veľmi pohoršujúca správa. 
V neďalekej krajine uniesol drak viaceré princezné a schoval sa vysoko v horách,
kde je veľmi ťažký prístup a chystá sa tam zajtra o polnoci všetky princezné zjesť.

Samozrejme, že sa tomu nemôžete len tak nečinne prizerať. 
Vďaka moderným technológiám máte k dispozícii aj mapu, ktorú rýchlo zverejnil kráľ – otec unesených nešťastníc. 
Vašou úlohou je teda prirodzene čo najskôr najprv zneškodniť draka 
a potom zachrániť všetky unesené princezné. 

Predpokladajte, že drak uniesol __najviac 5 princezien__.

Mapa predstavuje dvojrozmernú mriežku políčok: 
- __C__ - Lesný chodník
- __H__ - Hustý lesný porast, cez ktorý idete dva-krát pomalšie ako po lesnom chodníku
- __N__ - Nepriechodná prekážka (napr. príliš strmá skala) D Drak
- __P__ - Princezná

## Implementácia 

```c
int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty);
```

Táto funkcia má nájsť cestu, ktorou postupovať,
aby Popolvár čo najskôr zachránil všetky unesené princezné a zneškodnil draka. 

Mapa je vysoká n políčok a široká m
políčok (1 ≤ n,m ≤ 100). 

Parameter _t (0 ≤ t ≤ 106)_ určuje, kedy od začiatku vášho hľadania sa drak zobudí a zje prvú princeznú. 
Keďže drak lieta veľmi rýchlo, spoľahnite sa, že ak sa vám ho nepodarí zneškodniť
dovtedy ako sa zobudí, princezné už nezachránite. 

Prechod cez lesný chodník trvá jednu jednotku času a drak sa zobudí v t-tej jednotke času, kedy už bude neskoro. 
A nezabudnite, že najprv musíte zneškodniť draka, až potom môžete zachraňovať princezné (hoci by ste aj prechádzali predtým cez políčko kde je princezná).
Veď ako by to bolo, keby ste bojovali s drakom pri zástupe princezien ako diváčkami...

Nájdenú cestu vráťte ako postupnosť súradníc (dvojíc celých čísel x,y, kde 0 ≤ x < m a 0 ≤ y < n) všetkých navštívených políčok. Na začiatku sa vždy nachádzate na políčku so súradnicami 0,0 a na poslednom navštívenom políčku musí byť jedna z unesených princezien. Ak existuje viacero rovnako dlho trvajúcich ciest, môžete vrátiť ľubovoľnú z nich. Výstupný argument dlzka_cesty nastavte na počet súradníc, ktoré ste vrátili ako návratovú hodnotu.
Implementujte vyššie uvedenú funkcionalitu čo možno najefektívnejšie. 

Pre hľadanie najkratšej cesty použite __Dijkstrov algoritmus__ s __binárnou haldou__ (Min Heap). 
Môžete použiť aj inú funkčnú metódu na nájdenie najkratšej cesty, príp. bez binárnej haldy,
ale bude to za bodovú sankciu.

    
## Príklad použitia vašej funkcie

```c
int i, *cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty); 

for (i=0;i<dlzka_cesty;++i)
    printf("%d %d\n", cesta[i*2], cesta[i*2+1]);
```

Riešenie zadania sa odovzdáva do miesta odovzdania v AIS do stanoveného termínu
(oneskorené odovzdanie je prípustné len vo vážnych prípadoch, ako napr. choroba, o možnosti odovzdať zadanie oneskorene rozhodne cvičiaci, príp. aj o bodovej penalizácii). 

Odovzdáva sa jeden zip archív, ktorý obsahuje jeden zdrojový súbor s implementáciou a jeden súbor s dokumentáciou vo formáte pdf.
Dokumentácia musí obsahovať hlavičku (kto, aké zadanie odovzdáva), 
stručný opis použitého algoritmu s názornými nákresmi/obrázkami a krátkymi ukážkami zdrojového kódu, 
vyberajte len kód, na ktorý chcete extra upozorniť.

Pri opise sa snažte dbať osobitý dôraz na zdôvodnenie správnosti vášho riešenia – teda dôvody prečo je dobré/správne, spôsob a vyhodnotenie testovania riešenia. Nakoniec musí technická dokumentácia obsahovať odhad výpočtovej (časovej) a priestorovej (pamäťovej) zložitosti vášho algoritmu. Celkovo musí byť cvičiacemu jasné, že viete čo ste spravili, že viete odôvodniť, že to je správne riešenie, a viete aké je to efektívne.

## Dôležité poznámky
 
Popolvár sa presúva len v štyroch smeroch (hore, dole, doľava, doprava). 
Ak nie je zadané inak, prechod cez políčko trvá štandardne jednu jednotku času.
 Teda cez políčka s drakom a princeznou trvá prechod tiež jednu jednotku času. 
Do výsledného času sa započítavajú všetky políčka, cez ktoré Popolvár prejde. Ak teda začne na políčku (0,0), prejde cez políčko (1,0) a skončí na políčku (1,1), pričom všetky tri políčka obsahujú lesný chodník, tak Popolvárovi to trvá 3 jednotky času. Zneškodnenie draka je okamžitá akcia, ktorej trvanie je zanedbateľné vzhľadom na čas trvania prechodu cez políčko (teda ju zarátavame s nulovou dĺžkou trvania). Keďže Popolvár má celú radu ďalších princezien, ktoré musí ešte zachrániť v iných častiach sveta (na vstupe je viacero kráľovstiev, v ktorých chce zachrániť princezné), musíte čo najskôr zachrániť princezné (celkovo vrátane zneškodnenia draka), aby ste mohli čím skôr prejsť do ďalšieho kráľovstva. Na zadanej mape je vyznačených najviac 5 princezien.

## Hodnotenie
Môžete získať 15 bodov, minimálna požiadavka 6 bodov.
12 bodov je za program a testovanie (za riešenie neobsahujúce Dijkstrov algoritmus s binárnou haldou max 9 bodov; z toho vlastné testovanie 2 body), 3 body sú za dokumentáciu (bez funkčnej implementácie 0 bodov), pričom body môžu byť výrazne ovplyvnené prezentáciou cvičiacemu (napr. keď neviete reagovať na otázky vzniká podozrenie, že to nie je vaša práca, a teda je hodnotená 0 bodov).
