# Scattering Equation Builder - SEB
*A C++ library where one can build a structure from geometric shapes from which one can get its small angle scattering properties*.

## Introduction
SEB is a C++ library that allows you to build structures from building blocks such as spheres, polymers, rods, etc., and obtain their scattering properties symbolically. SEB provides functions that enable you to attach building blocks together at specific points called reference points, and compute the form factor, $F$, the radius of gyration, $R^2_g$, and other scattering properties of the resulting structure. These properties are for small angle scattering theory and are meant for structures floating around, some litterature would call the form factor the isotropic form factor.

### Workflow
To give you a quick overview of how SEB works, imagine a structure that you want to build from the available building blocks. You can use SEB in C++ to attach the building blocks together and generate the form factor as a symbolic equation. You can then plot the equation by substituting the symbols with numerical values. A typical work flow of SEB is shown in the following picture:

![Workflow](https://user-images.githubusercontent.com/45657039/224036019-3ec657a6-4b16-4d69-8783-e1eb8fb7a654.png)
1. Imagine a structure one can build from the given building blocks (Here a polymer star).
2. Using the SEB librabry in C++ build the structure by attaching the building blocks together (Here a polymer is the building block).
3. End the code with calling ```cout << getFormFactor("PolymerStar") << endl``` to generate the form factor as an symbolic equation.
4. Plot the equation by substituting the symbols in the equation. 

### The theory of SEB
The theory behind SEB can be found in the journal articles: "[A formalism for scattering of complex composite structures. I. Applications to branched structures of asymmetric sub-units](https://doi.org/10.1063/1.3682778)" and "[A formalism for scattering of complex composite structures. II. Distributed reference points](https://doi.org/10.1063/1.3701737)"

## Installation/requirement for usage
SEB has only been compiled on linux systems and has not been tested on mac OS or Windows.  If you are using Windows, we recommend using the *Windows Subsystem for Linux* (WSL), search for it in the search bar. Before downloading the source code make sure you have installed
1. a C++ compiler on your system. 
2. The [GiNaC](https://www.ginac.de/Download.html) symbolic manipulation librabry for C++

Once you have installed these dependencies, you can clone the source code in the terminal using ```git clone https://github.com/Tobionecenobi/SEB.git```.
To use SEB in a C++ file, include the library by adding ```#include "PATH_TO_FOLDER/SEB.h"```, to your code, where `PATH_TO_FOLDER` is the given path to `SEB.h`. 

## Using SEB in C++ Code
### Creating a world
SEB is straightforward to use. To get started, create a world object using the World constructor:
```
World world = World("world");
```
### Adding one building block
Next, add one building block to the world.. For now there is 1 building block:
1. Random walk polymer `new RWpolymer()`
A random walk polymer can be added to the world by using:
```
graphID g = world.add(new RWpolymer(), "poly1");
```
### Adding and linking a building blocks simulatniously
When adding the polymer it is given a graph id `g` which other linked building blocks will share such that they are a part of the same building block. Other polymers can then be added to the world and then linked to `poly1` by:
```
world.addAndLink(new RWpolymer(), "poly2.end1", "poly1.end2");
```
Here the polymer has two reference points (possible points to link to) in each end called "end1" and "end2". Here `poly2` is added to `world` and `poly2`'s "end1" is linked to `poly1`'s "end2". ***BE AWARE THAT ONE CANNOT LINK BUILDING BLOCKS THAT ARE ALREADY ADDED TO THE WORLD*** this is to prevent loops of building blocks in the structure. 

### Wrapping the building blocks into a structure
Now one has two polymers linked together by their ends. To get the form factor of such a structure one needs to "wrap" a structure around the sub unit:
```
graphID g2 = worldb.add(g, "twoPolymers");
```

### Acurring the form factor of the structure
Now a new graph is generated and carryies a graph id `g2`. Inside graph `g2` is graph `g` nested.
Now one can print the sybolic equation for the form factor with the code:
```
ex formFactor = pow(worldb.betaSum("brush", 2, 2),-2)*worldb.getFormFactor("brush", 2, 2);
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


