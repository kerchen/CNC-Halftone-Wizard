
Building the CNC Halftone Wizard
If you want to (or have to) build the app from source, it's pretty painless if 
you've already got Qt and a C++ compiler installed on your computer.  Many 
linux distros include these in their standard configuration, so if you're 
building the app under linux, you probably won't even have to worry about them.  
Under Windows, you may need to install these things (Qt supports the Intel, 
MinGW, and Microsoft C++ compilers); how to set up a Windows C++/Qt development 
environment is more than I want to tackle in these instructions, but there are probably 
plenty of tutorials out there on the intertubes.  I have no idea how to go about
building C++/Qt apps (or any apps, for that matter) on a Mac; my understanding 
is that it's probably about as easy as it is under linux, but you'll have to ask
Mr. Google about that.

Once you have Qt and a C++ compiler installed and configured on your machine,
you should be able to build the app by typing these commands from a command 
prompt in the directory that holds the CNC Halftone Wizard source code (most
likely, that is the directory where the file that you're reading right now is):
        qmake CNCHalftoneWizard.pro
        make

If you're using Microsoft's compiler, you'll need to type 'nmake' instead of 
'make'.  Also, you may have noticed that I've been a bit schizophrenic about 
naming this thing.

The qmake command should only take a few seconds; it's building the 'makefile' 
that the next command will use.  The make (or nmake) command will take a bit 
longer as it compiles each source file and then links them into the final app.  
If, after the make command finishes, there is nothing saying "error" in the 
output, the app should be in the 'release' directory of the directory where you 
ran the commands from.

I developed this app under Windows 7 using Microsoft Visual Studio 2005 and 
Qt 4.5.0.  I didn't do anything tricky in my code, so it should have no problems
compiling under more current versions of MSVC and Qt (and it should run under
Windows XP, Vista and Windows 7).  I've also verified that the app builds and
runs under Ubuntu 10.10, so there shouldn't be any problems building and running
this app under most recent linux distros.

Using the CNC Halftone Wizard

Using it is fairly straight-forward (spoken like a true software developer).  
First, you load an image that you want to generate some halftone g-code for: 
Click on 'Open' in the File menu and then navigate to the file that you want 
to perform some Halftone wizardry on.

In the Halftone tab of the app, there are three fields that you can change:
* Source Pixel Step: This number determines how many pixels the halftoning 
algorithm considers in the source image when calculating the size of the dot
in the output.  The smaller the number, the more detail will be preserved in
the output.  For example, if you set the source pixel step to 2, the intensity
of four (2x2) pixels will be averaged to determine the dot size of the
corresponding output dot.  Valid values are in the range [2..30], though
anything above 12 or so becomes pretty abstract unless the source image is very
large.  Also, the spin control changes the value by +/- 2 with each click, but
you can specify an odd number by typing it into the control.  Odd values will 
result in additional loss of detail due to rounding errors, but that can create 
some interesting effects too.
* Min Dot Gap: This number specifies the minimum gap, in inches, between each 
dot.  Thus, if there are two dots of maximum intensity next to each other in the 
output, they will be separated by a gap of the size specified in this field.
* Max Cut Depth: This number specifies the maximum depth of the cut as a 
percentage of the Tool Depth.

In the G-Code tab, there is a single field:
* Preamble: Use this text field to enter anything you want to appear at the
beginning of every generated g-code.  The app doesn't interpret or change
this text in any way--it just slaps it into the g-code file before the
g-code that controls the actual cutting and movement commands.

In the Tool tab, there are several values you can change to suit the tool
you want to generate g-code for.
* Feed: This is the feed rate for cutting operations.
* Speed: This is the spindle speed.
* Fast Z: This specifies the z height at which the cutting head can be
moved safely at a fast rate.
* Coolant: Check this box to add coolant on and off g-code commands at the
start and end, respectively, of the cutting program.  I don't have a machine
that is cooled, so this feature is UNTESTED.
* Full Tool Depth: The height of the cutting area of the cutting tool/bit.
* Full Tool Width: The width of the cutting tool/bit at its widest.



