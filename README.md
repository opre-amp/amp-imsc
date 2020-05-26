# amp-imsc
__userspace-gpio__: Egyszerűen `make` paranccsal fordítható, aztán a binárist a Pi-re küldve indítható. Mivel `/dev/gpiomem` hozzáférést igényel, sudo-val szükséges indítani (vagy más ekvivalens megoldással). A 15. pint inputként olvassa, és a szintjét átmásolja a 23. pinre.

__kernel-gpio__: `make LINUX=<path-to-kernel-sources> M=$PWD` paranccsal fordítható, ami után a `kernel-gpio.ko` fájlt a Pi-re juttatva lehet `insmod kernel-gpio.ko` és `rmmod kernel-gpio` parancsokkal kezelni. A 15. pint inputként olvassa, és a szintjét átmásolja a 23. pinre.

__loader__: `make LINUX=<path-to-kernel-sources> M=$PWD` paranccsal fordítható, ami után a `loader.ko` fájlt a Pi-re juttatva lehet `insmod loader` és `rmmod loader` parancsokkal kezelni. A feladata: létrehozza a `/dev/loader` cdev fájlt, amibe írva egy bináris programot azt a 4. magon elkezdi futtatni. Fontos, hogy az egész program egyszerre kerüljön írásra, különben segfault-ot okoz(hat) a 4. magon, ami az egész rendszert lefagyasztja.

__baremetal-gpio__: `make` paranccsal fordítható, a `metal.img` fájl Pi/re juttatása után a következő paranccsal indítható: `dd if=metal.img of=/dev/loader bs=$(wc -c metal.img)`

__test__: Ez két részből áll: test-module illetve test-client. A test-module `make LINUX=<path-to-kernel-sources> M=$PWD` paranccsal fordítható, és a Pi-re juttatva `insmod gpio-test.ko` illetve `rmmod gpio-test` parancsokkal lehet kezelni. Beregisztrál egy IRQ kezelőt, amivel kb. pontosan fogunk tudni latency-t mérni. A `test-client` simán egy `make` paranccsal fordítható, és elindítva (ennek szintén kell sudo) egy számot paraméterként megadva ennyiszer fogja a 14. pin-re írt értéket a 24. pin-en várni. Van timeout, kb. fél másodperc után feladja, ekkor 1 000 000 us-ot, amúgy a mért értéket (us-ben) írja ki. Érdemes először késleltetésmentesen kipróbálni, nekem ekkor kb. 10us volt az "alap" késleltetés, és nem változott lényegesen a leterheltség függvényében.
