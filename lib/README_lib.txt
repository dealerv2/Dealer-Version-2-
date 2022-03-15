This folder contains files that support the functioning of DealerV2 and some 
files needed for rebuilding from source.
1) libdds.a is a pre-compiled static library with Bo Haglund's Double Dummy Solver code.
The binaries of DealerV2 have been linked against this library. Since it is a static 
library its presence is not required to run DealerV2, but is required if you want to 
rebuild DealerV2

2) dds290-src_2022.tar.gz is a gzipped tar archive of the source code for the DDS library.
It is not the complete distribution of the DDS but it is sufficient to rebuild the library if needed.
Unpack it, cd to the src/ directory and type make linux to rebuild.
                                             ^^^^^^^^^^
3) DOP4DealerV2.tar is a tar archive of the Perl code that implements Optimal Point Count evaluation
To use the opc functionality of DealerV2
a) create a new directory /usr/local/bin/DOP
b) unpack the contents of this file into this new directory.
You will need super user priviledges to do these steps.

you can then run the dop executable in this directory with dop -h or dop -H5 for help.
Or you can browse the usage.txt file.



