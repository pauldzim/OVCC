This is a fork of Walter Zambotti's OVCC project: https://github.com/WallyZambotti/OVCC

I have added an xcodeproj file, so that OVCC can be built using Apple's Xcode IDE.

'homebrew' and the SDL2, fontconfig, freetype, and png16 libraries are required
to build OVCC. See https://brew.sh

The AGAR library at https://github.com/JulNadeauCA/libagar.git is also required.
To work under OS X, it needs to have the patches in the 'Patches/AGAR' directory
in this repo applied before building it.

See Walter's page for more info on OVCC and the build requirements for it.

This version only supports the arm64 architecture (M1 chips and newer) mainly
because I couldn't figure out how to do dual-architecture builds using Homebrew.

This version of OVCC has a few differences from Walter's:

    - The required ROMs (at least 'coco3.rom' and 'fd502.rom' are needed) should
      be copied to the 'Contents' folder in the app after building it but before
      running it. This folder is at 'CoCo/Debug/ovcc.app/Contents' after the build.

      You can then double-click this 'ovcc.app' folder from Finder to launch
      OVCC. You can also copy this 'ovcc.app' folder into your local Applications
      folder, and OVCC should then show up in Launchpad.

    - When running OVCC, in the 'Cartridge->Load Cart' menu, the loadable
      modules (.so) can be found in the 'Frameworks' subfolder.

    - After loading the FD502 module for the first time, select the 'Cartridge->
      FD-502 Config' menu entry, then in the dialog click the 'Browse' button
      and load the 'fd502.rom' file. Click the 'External Rom Image' button and
      then click 'OK'. Then do a Hard Reset. and you should see that "Disk
      Extended Color Basic" has loaded.

You can find the needed ROMs at https://colorcomputerarchive.com. I used the
ones from the repo/ROMs/XRoar/CoCo directory.

There is a prebuilt image of OVCC (MacOS arm64 only) at
https://drive.google.com/drive/folders/1Va9Vq35dOAkd4joCtuB7zRYQ8CY6mssN?usp=sharing
The prebuilt image also needs to have the ROMs added to it before it will run.

One note: If you load the libmpi.so module first ("Mult-Pak Interface"), and
then load the libfd502.so module (floppy disk controller) into it, you *must*
load it into MPI Slot 4. The floppy disk controller won't work in any other slot.
I didn't see this documented anywhere, and it took me quite some time to figure
out.
