# xtm

Le format de fichier `XTM` est le format utilisé par le logiciel [XTRemSplit](http://xtremsplit.fr/xtremsplit-accueil.html). Ce logiciel permet de couper des fichiers en plusieurs fichiers de taille moins importante. Le programme XTM permet de réassembler les fichiers par l'intermédiaire d'une interface en ligne de commande.

`XTM` est écrit en C et est distribué sous licence GNU GPL.

# Installation

    $ git clone git://github.com/hcartiaux/xtm.git
    $ cd xtm
    $ make
    # make install

#Utilisation

    xtm -i path/to/file.001.xtm [-o output/directory/] [-f] [-v]

Lisez la page de manuel pour plus d'informations.
