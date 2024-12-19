# OpenConsole - Graphics Program

OpenConsole is a 3D graphics program that allows users to create, manipulate, and visualize 3D objects such as cubes, spheres, and other shapes. This program was built using C++ and features functionality for adding, moving, scaling, rotating, and coloring objects in a scene.

## Prerequisites

The follow setup instructions are for Windows.

Before running the program, ensure you have the following installed:

### 1. **CLion IDE (for C++ development)**
- [Download CLion](https://www.jetbrains.com/clion/)

### 2. **C++ Compiler**
For Windows users, you can use WSL, MinGW, Visual Studio, etc. The following instructions will pertain to Visual Studio.
- [Download Visual Studio 2022](https://visualstudio.microsoft.com/vs/)
- During installation, select "Desktop development with C++".

## Installation

### Clone the Repository

Clone this repository to your local machine using the following command:

```bash
git clone https://github.com/jrohrbaugh0812/open-console.git
```

## Set Up Development Environment

1. Open **CLion**, select **Open** from the main screen (you may need to close any open projects), and navigate to the repository. This should open the project and execute the CMake script to configure the toolchain.
2. In the popup dialog, in the Toolchain drop down select Visual Studio.
3. Open the project folder: `File > Open` and select the folder where the repository was cloned.
3. Then select Manage toolchains.
4. In the toolchain dialog, be sure Visual Studio is selected, then in the Architecture: dropdown, choose `amd64`
5. Then click OK to exit the dialog boxes. This will set Visual Studio x64 as the compiler for this project.

## Usage

### Running the Program

Once you run the program, a window will open with a 3D-canvas in which 'WASD' can be used to rotate the canvas. The terminal will display a welcome screen and you will be prompted to enter a command.

### Available Commands

You can type ```help``` to generate a list of all the commands. Here are some of the commands:

- **add**: Adds a new shape to the scene.
- **move**: Moves an object by a specified vector.
- **delete**: Deletes an object from the scene.
- **background**: Changes the background color of the scene.
- **list**: Lists all objects currently in the scene.
- **quit**: Exits the program.

### Example

```
Enter command: add 
Enter type of shape, position (x y z): cube 0 0 0

Enter command: list 
Objects in the scene: 
0: Shape: cube, Position: (0, 0, 0), Scale: (1, 1, 1), Angle: 0, Color: redCube

Enter command: move 
Enter object ID and movement vector (dx dy dz): 0 1 1 1

Enter command: background 
Enter background color (e.g., red, blue, green): blue

Enter command: color 
Enter object index and color name: 0 blueCube
Assigned color 'blueCube' to object ID 0

Enter command: delete 
Enter index of object you would like to delete: 0

Enter command: quit
```

### Note
There are five models currently avaliable to be added and manipulated: cube, torus, cylinder, sphere, and cone.
Also, when it comes to colors, there are ten colors that can be applied: red, green, blue, yellow, cyan, magenta, black, orange, purple, and gray.
When coloring the background, just enter the explict color name (e.g., red, blue). When coloring the background, prefix whatever color you want onto the name of the object you want colored (e.g., blueCube, magentaCone, yellowSphere).

## Acknowledgements
This project was the final project for the **CS340** course (Programming Language Design), but is based on materials and instructions provided in the **CS370** course (Computer Graphics I) at **YCP**, taught by **Dr. Babcock**. Much of the setup process and some of the functions used in this project, such as `build_solid_color_buffer` and `draw_color_obj`, were derived from the course resources. Thank you to Dr. Babcock for these valuable resources, which helped guide the development of this project.
