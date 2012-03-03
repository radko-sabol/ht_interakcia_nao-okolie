## Vytvorenie Unix Makefile (Linux, Mac)/Visual Studio 9 2008 solution/Visual Studio 2010 solution súborov:
* nastavienie qibuild-u (zdroj: http://www.aldebaran-robotics.com/documentation/dev/cpp/tutos/using_qibuild.html):
  * vytvoriť adresár, kde bude uložený projekt (v tomto adresári spustíte aj `git clone git@github.com:radko-sabol/ht_interakcia_nao-okolie.git`)
  * spustiť príkaz: `qibuild init --interactive`
  * nastaviť qibuild-u aby používal toolchain z Aldebaran SDK: `qitoolchain create <nazov_profilu> <cesta/ku/sdk/toolchain.xml> --default`
  * príkazom `qitoolchain status` si viete skontrolovať správnosť nastavenia
  * spustiť príkaz `qibuild configure ht_interakcia_nao-okolie` (musíte byť o adresár vyššie ako je adresár "ht_interakcia_nao-okolie" - čiže aktuálny ak ste sa nehýbali)

  * v prípade ak si zvolíte názov toolchain profilu iný ako "nao-win32", tak prosím, pridajte adresár "build-vas_nazov_profilu" do ".gitignore" súboru (aby ste necommitovali aj solution súbory)

## Nastavenie solution-u vo Visual Studio 2010:
* pridanie systémovej alebo používateľskej premennej "ALDEBARAN_SDK" s hodnotou "cesta/ku/sdk"
* menu: "Project/Properties/Configuration Properties/VC++ Directories"
  * do "Include Directories" pridať hodnotu "$(ALDEBARAN_SDK)\include"
* menu: "Project/Properties/Configuration Properties/Linker/Input"
  * do "Additional Dependencies" pridať hodnoty:
    `$(ALDEBARAN_SDK)\lib\alvision_d.lib
     $(ALDEBARAN_SDK)\lib\cv200d.lib
     $(ALDEBARAN_SDK)\lib\cvaux200d.lib
     $(ALDEBARAN_SDK)\lib\cxcore200d.lib
     $(ALDEBARAN_SDK)\lib\highgui200d.lib
     $(ALDEBARAN_SDK)\lib\ml200.lib
     $(ALDEBARAN_SDK)\lib\alextractor_d.lib
     $(ALDEBARAN_SDK)\lib\alcommon_d.lib
     $(ALDEBARAN_SDK)\lib\alsoap_d.lib
     $(ALDEBARAN_SDK)\lib\alvalue_d.lib
     $(ALDEBARAN_SDK)\lib\tinyxml_d.lib
     $(ALDEBARAN_SDK)\lib\rttools_d.lib
     $(ALDEBARAN_SDK)\lib\althread_d.lib
     $(ALDEBARAN_SDK)\lib\alerror_d.lib
     $(ALDEBARAN_SDK)\lib\pthreadVCE2.lib
     $(ALDEBARAN_SDK)\lib\libboost_signals-vc100-mt-gd-1_44.lib
     $(ALDEBARAN_SDK)\lib\libboost_program_options-vc100-mt-gd-1_44.lib
     $(ALDEBARAN_SDK)\lib\allogremote_d.lib
     $(ALDEBARAN_SDK)\lib\qi_d.lib
     $(ALDEBARAN_SDK)\lib\libboost_filesystem-vc100-mt-gd-1_44.lib
     $(ALDEBARAN_SDK)\lib\libboost_system-vc100-mt-gd-1_44.lib
     $(ALDEBARAN_SDK)\lib\libboost_thread-vc100-mt-gd-1_44.lib
     $(ALDEBARAN_SDK)\lib\libboost_date_time-vc100-mt-gd-1_44.lib
     $(ALDEBARAN_SDK)\lib\zmq_d.lib`