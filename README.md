# Scattering Equation Builder - SEB
*A C++ library where one can build a structure from geometric shapes from which one can get its small angle scattering properties*.

## Introduction
SEB is a C++ library that allows you to build structures from building blocks such as spheres, polymers, rods, etc., and obtain their scattering properties symbolically. SEB provides functions that enable you to attach building blocks together at specific points called reference points, and compute the form factor, $F$, the radius of gyration, $R^2_g$, and other scattering properties of the resulting structure. These properties are for small angle scattering theory and are meant for structures floating around, some litterature would call the form factor the isotropic form factor.

### Workflow
To give you a quick overview of how SEB works, imagine a structure that you want to build from the available building blocks. You can use SEB in C++ to attach the building blocks together and generate the form factor as a symbolic equation. You can then plot the equation by substituting the symbols with numerical values. A typical work flow of SEB is shown in the following picture:

![Workflow](https://github.com/Tobionecenobi/SEB/assets/45657039/59a2129c-ce13-4534-8aef-69e5b7e02600)
1. Imagine a structure one can build from the given building blocks (Here a polymer star).
2. Using the SEB librabry in C++ build the structure by attaching the building blocks together (Here a polymer is the building block).
3. End the code with calling ```cout << getFormFactor("PolymerStar") << endl``` to generate the form factor as an symbolic equation.
4. Plot the equation by substituting the symbols in the equation. 

### SEB Articles
The theory behind SEB can be found in the journal articles: Carsten Svaneborg, Jan Skov Pedersen, J. Chem. Phys. 136, 104105 (2012) "[A formalism for scattering of complex composite structures. I. Applications to branched structures of asymmetric sub-units](https://doi.org/10.1063/1.3682778)" and Carsten Svaneborg, Jan Skov Pedersen, J. Chem. Phys. 136, 154907 (2012) "[A formalism for scattering of complex composite structures. II. Distributed reference points](https://doi.org/10.1063/1.3701737)"

The reference for the SEB library itself is T. W. J. Jarret and Carsten Svaneborg "[A computational tool for symbolic derivation of the small angle scattering from complex composite structures](https://arxiv.org/abs/2311.17585)"  (DRAFT MANUSCRIPT, Submitted for Journal of Applied Crystallography 2023). See below for Bibtex citations for these papers.


## Dependencies

SEB has only been compiled on linux systems and has not been tested on mac OS or Windows.  If you are using Windows, we recommend using the *Windows Subsystem for Linux* (WSL), search for it in the search bar. Before downloading the source code make sure you have installed
1. a working C++ compiler and make system on your system. 
2. The [GiNaC](https://www.ginac.de/Download.html) symbolic manipulation librabry for C++
3. The [GNU scientific Libary](https://www.gnu.org/software/gsl/) 


## Installing SEB on Linux

### Cloning the code

You can clone the source code in the terminal using
```
git clone https://github.com/Tobionecenobi/SEB.git
```
this will create a SEB folder in the current working directory and download the source code into this folder.

### Install dependencies

Ubuntu:
```
sudo apt install libgsl-dev libginac-dev
```

This will install the GNU scientific library and GiNaC package along with their dependencies.

### Compiling

```
cd SEB
make
```

will compile the SEB library (into build/libseb.a), along with all the examples (in Examples), code for generating figures in the SEB paper (in PaperFigs). Assuming everything went well, then Examples and PaperFigs will be full of executables which you can run to see how SEB works. These targets can be recompiled individually by calling make with "seb", "examples", "paperFigs", or "work" as an argument. The last argument compiles user own code in the work folder, which is empty when you clone the repo.


## Using SEB in your own C++ Code

To use SEB you can either 1) develop code in the SEB/work folder  or 2) develop code anywhere on your computer. Option 1 allows you to reuse the SEB compilation infrastructure and no installation of SEB is required. Option 2 requires the user to manually compile the code specifying where SEB header files and library is located.

### Creating a world

SEB is straightforward to use. To use SEB in a C++ file, include the library by adding ```#include "PATH_TO_FOLDER/SEB.h"```, to your code, where `PATH_TO_FOLDER` is the given path to `SEB.h`. 

To get started, create a world object using the World constructor:
```
World world;
```
### Adding one building block
Next, add one building block to the world.. For now there is 1 building block:
1. Random walk polymer `"GaussianPolymer"`
A random walk polymer can be added to the world by using:
```
GraphID g = world.Add("GaussianPolymer", "poly1");
```
### Adding and linking a building blocks simulatniously
When adding the polymer it is given a graph id `g` which other linked building blocks will share such that they are a part of the same building block. Other polymers can then be added to the world and then linked to `poly1` by:
```
world.Link("GaussianPolymer", "poly2.end1", "poly1.end2");
```
Here the polymer has two reference points (possible points to link to) in each end called "end1" and "end2". Here `poly2` is added to `world` and `poly2`'s "end1" is linked to `poly1`'s "end2". ***BE AWARE THAT ONE CANNOT LINK BUILDING BLOCKS THAT ARE ALREADY ADDED TO THE WORLD*** this is to prevent loops of building blocks in the structure. 

### Wrapping the building blocks into a structure
Now one has two polymers linked together by their ends. To get the form factor of such a structure one needs to "wrap" a structure around the sub unit:
```
graphID g2 = world.Add(g, "twoPolymers");
```

### Obtaining the form factor of the structure
Now a new graph is generated and carryies a graph id `g2`. Inside graph `g2` is graph `g` nested.
Now one can print the sybolic equation for the form factor with the code:
```
ex formFactor=w.FormFactor("twoPolymers");
cout << latex;
cout << formFactor << endl;
```
The output is the form factor in latex format. For other formats check the [GiNaC documentation](https://www.ginac.de/tutorial/#Expression-output).

<!-- Comment is starting that cannot be viewed in the markdown file
Now g2 -> g1 == two linked polymers.

After doing the stuff in the installation session start the code by including `SEB.h`
```#include "PATH_TO_FOLDER/SEB.h"```, where `PATH_TO_FOLDER` is the given path to `SEB.h`. 
Then in the main function start by creating a world and give the world a name:
-->

### Compiling 1) work folder

```
make work
```
Running this command in the SEB parent folder will compile and link user c++ code in the work folder using the SEB compile infrastructure. The resulting executable is placed in the SEB/work/ folder.

### Compiling 2) anywhere else

Assuming your code is in code.cpp then uou can compile and link your code with
```
c++ -O2 -c -I<location of SEB/SEB folder/> code.cpp
c++  code.o  -lseb -lgsl -lgslcblas -lm -lginac -L<location of build folder/>  -o myexecutable
```
where the user manually has to specify the location of the header files (FOLDER/SEB/SEB/) and the library (FOLDER/SEB/build/), where you should modify FOLDER to fit your local environment. Note its important the object file is specified before the libraries.

## Bibtex


@article{svaneborg2012formalism1,
  title={A formalism for scattering of complex composite structures. I. Applications to branched structures of asymmetric sub-units},
  author={Svaneborg, Carsten and Pedersen, Jan Skov},
  journal={J. Chem. Phys.},
  volume={136},
  number={10},
  pages={104105},
  year={2012},
  publisher={AIP Publishing}
}


@article{svaneborg2012formalism2,
  title={A formalism for scattering of complex composite structures. I. Distributed reference points},
  author={Svaneborg, Carsten and Pedersen, Jan Skov},
  journal={J. Chem. Phys.},
  volume={136},
  pages={154907},
  year={2012},
  publisher={AIP Publishing}
}
