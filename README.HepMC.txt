Saved from: http://svn.cern.ch/guest/AliRoot/trunk/PYTHIA8/pythia8170/README.HepMC

How to install HepMC on your laptop (Linux/Mac OSX) and link to Pythia

Here are two alternative recipes, one simpler with a more recent version
and one more flexible with an older one. 
Note: This comes with no guarantee; it is what worked for me.
Other combinations should also work, but have not been tried.

------------------------------------------------------------------------

Alternative A, with HepMC 2.06.08.

1) Create a directory where you want to do the installation, e.g.
     mkdir hepmc
and move to it
     cd hepmc

2) Open
     http://lcgapp.cern.ch/project/simu/HepMC/download 
in a browser, scroll down to "HepMC 2.06.08" and  pick the version relevant 
for your platform from the list to the right. If there is no match, you 
have to go for Alternative B below.

3) In the pop-up menu pick "Save to file" and then the above hepmc directory.
You should now have the appropriate version in that directory,
e.g. HepMC-2.06.08-x86_64-mac106-gcc42-opt.tar.gz on a current Mac.

4) Unpack with
    tar -xzf file.tar.gz
with "file" the name of the downloaded file. This will create a new directory, 
e.g. x86_64-mac106-gcc42-opt/, where ready-to-use libraries can be found.

5) Now move to your pythia8160 (or later) directory and configure with
     ./configure --with-hepmc=mypath/hepmc/newdir --with-hepmcversion=2.06.08
   where "mypath" is the absolute path to the hepmc directory (use pwd if 
   in doubt) and "newdir" is the name of the new directory created in step 4.
   (Re)compile with "make" (or "gmake") as usual.

6) Move to the examples subdirectory and do
     source config.sh
     source config.csh
Only one of them will work, depending on which shell you use, but the other
should be harmless.

7) You should now be able to make and run e.g. main41 to produce HepMC 
output files.

------------------------------------------------------------------------

Alternative B, with HepMC 2.04.02.

1) Create a directory where you want to do the installation, e.g.
     mkdir hepmc
and move to it
     cd hepmc

2) Open
     http://lcgapp.cern.ch/project/simu/HepMC/download 
in a browser, scroll down to "HepMC 2.04.02" and 
click on "source" rightmost on the line below.

3) In the pop-up menu pick "Save to file" and then the above hepmc directory.
You should now have a file "HepMC-2.04.02.tar.gz" in that directory.

4) Unpack with
    tar -xzf HepMC-2.04.02.tar.gz

5) Create two new subdirectories
    mkdir build install
and move to build
    cd build
  
6) Configure the compilation with
    ../HepMC-2.04.02/configure -prefix=mypath/hepmc/install -with-momentum=GEV -with-length=MM
all on one line, where "mypath" is the absolute path to the hepmc directory 
(use pwd if in doubt).  

7) Compile and install with
     make
     make check
     make install
which takes a minute or two and an amount of output. 

8) Now move to your pythia8160 (or later) directory and configure with
     ./configure --with-hepmc=mypath/hepmc/install --with-hepmcversion=2.04.02
   (Re)compile with "make" (or "gmake") as usual.

9) Move to the examples subdirectory and do
     source config.sh
     source config.csh
Only one of them will work, depending on which shell you use, but the other
should be harmless.

10) You should now be able to make and run e.g. main41 to produce HepMC 
output files (you can ignore compilation warnings about variables being 
shadowed; this is harmless).

NOTE: HepMC 2.04 allows you to set the units of the event record,
i.e. to ensure that the Pythia default choice of GeV and mm (with c = 1)
is propagated to the HepMC event record. This feature is not available
in 2.03. The two test programs main31 and main32 therefore come with
the Units code commented out, but with instructions how to restore it.  
