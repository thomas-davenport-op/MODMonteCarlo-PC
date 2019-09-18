# MODMonteCarlo-PC

CTEQ VM settings adjustments:

1. Before running, under "Settings>Display" change the "Graphics Controller" obtion to "VBoxSVGA".

2. When you start the VM, under view click on "Auto-resize Guest display"

Now the Linux screen shoul automaticall adjust to the size of the window you make on your computer (with some delay)

Instructions for getting MODMonteCarlo-PC working on the CTEQ VM:

1. Update apt-get and upgrade the default software:
   - `sudo apt-get update`
   - `sudo apt-get upgrade`	

2. Install git :
   - `sudo apt install git`

3. Update fastjet using the first two sets of instructions at http://fastjet.fr/quickstart.html with the following adjustments:
   - the option for the configure command should be : `--prefix=/opt/hep` (first line of second box) 
   - use sudo to run the make install command: `sudo make install` (fourth line of second box)
   
3a. (optional) To complete the sample fast-jet program follow the rest of instructions on the above link with the following adjustment:
   - the compile command should be: (first two lines of the last gray box)
   ```
   g++ short-example.cc -o short-example \
      `fastjet-config --cxxflags --libs --plugins`
   ```
   
4. Install fastjet-contrib: 
    (check https://fastjet.hepforge.org/contrib/ for the latest version number and replace it appropriately below)
```
curl -O https://fastjet.hepforge.org/contrib/downloads/fjcontrib-1.042.tar.gz
tar -xvzf fjcontrib-1.042.tar.gz
cd fjcontrib-1.042

./configure --fastjet-config=/opt/hep/bin/fastjet-config --prefix=/opt/hep
make
sudo make install
```

5. Clone the MODMonteCarlo-PC git: (from your home directory `cd ~`)
```
mkdir MOD
cd MOD
git clone https://github.com/thomas-davenport-op/MODMonteCarlo-PC
cd MODMonteCarlo-PC
mkdir data
mkdir bin
make
```

You should be able to use the python scripts in utilities to generate events
try (from the MOD-MonteCarlo-PC) directory:

`python ./utilities/run-large-weighted-mc.py ./data 100`

This should generate a .mod data file in the .data/pythia directory with 100 events. See how long that takes. Try generating a few thousand events and see how long it takes.


# Optional Installs

Emacs:
```
sudo apt-get install emacs
sudo apt-get install at-spi2-core
```


### !!What follows is the original MODMonteCarlo README!!
# MODMonteCarlo




### Note to self:

Things to do:

- Download Pythia.
- Extract everything.
- Configure it to have sharing enabled using: `./configure --enable-shared`.
- Compile everything with `make`.
- Add the following runtime variables to `~/.bashrc`.
  
  `export PYTHIA8=/home/aashish/pythia8212`

  `export PYTHIA8DATA=$PYTHIA8/share/Pythia8/xmldoc`
  
  `export LD_LIBRARY_PATH=/home/aashish/pythia8212/lib`

- Compile your file with something like the following:

  `g++ -std=c++11 -I$PYTHIA8/include generateEvents.cc -o generateEvents -L$PYTHIA8/lib -lpythia8 -ldl`


## Delphes
- Looks like you need ROOT 6 for Delphes. Not that you should be using ROOT 5. The only reason it matters is that, for some werid unexplained reason, the **default** version in my Ubuntu distro seems to be 5. Just typing in root loads ROOT 5. So to make sure that during Delphes' compilation, classes from ROOT 6 are loaded instead of from ROOT 5, you need to do the whole `. bin/thisroot.sh` thing. An alternative (and much better) way to do this is remove ROOT 5 and install ROOT 6 in something like `/usr/local/` but a superposition of laziness and `if it ain't broke, don't fix it` is preventing me from doing that. Do that asap.
- `./bin/runDelphes cards/delphes_card_CMS_NoFastJet.tcl  data/pythia_output.hepmc`.

## Herwig
- Go to the data directory- `cd data/`.
- Run `Herwig++ read ../cards/herwig_card.in`. Note that if you don't change the random number seed in the card file, you get the same set of events.
- Run `Herwig++ run herwig_run.run -d1`.

## Sherpa
- Go to the data directory- `cd data/`.
- `Sherpa -f../cards/Run.dat`.


### Troubleshooting:
  `/usr/bin/ld: cannot find -lPythia` can be fixed by adding `export LD_LIBRARY_PATH=/home/aashish/pythia8212/lib` to your environment variables.

  Sometimes adding them to `~/.bashrc~ doesn't work. So enter the following manually:
  `export LD_LIBRARY_PATH=/home/aashish/hepmc/lib:/home/aashish/pythia8212/lib`

  `dyld: Library not loaded: libpythia8.dylib
  Referenced from: /Users/aashish/MODMonteCarlo/bin/generateEvents
  Reason: image not found
	`, in OSX, can be fixed by simply copying libpythia8.dylib from pythia8/lib/ to /usr/local/lib, assuming your pythia installation is in `/usr/local/lib/`.
