// OpenConsole - main

#define STB_IMAGE_IMPLEMENTATION
#define DEG2RAD (M_PI/180.0)

#include <stdio.h>
#include <vector>
#include "./common/vgl.h"
#include "./common/objloader.h"
#include "./common/utils.h"
#include "./common/vmath.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <locale>
#include <iosfwd>
#include <fstream>
#include <unordered_map>
#include <stack>
#include <sstream>

using namespace vmath;
using namespace std;

// Vertex array and buffer names
enum VAO_IDs {Cube, Cone, Torus, Cylinder, Sphere, Axes, NumVAOs};
enum ObjBuffer_IDs {PosBuffer, NormBuffer, TexBuffer, NumObjBuffers};
enum Color_Buffer_IDs {
    RedCube, RedCone, RedTorus, RedCylinder, RedSphere,
    GreenCube, GreenCone, GreenTorus, GreenCylinder, GreenSphere,
    BlueCube, BlueCone, BlueTorus, BlueCylinder, BlueSphere,
    YellowCube, YellowCone, YellowTorus, YellowCylinder, YellowSphere,
    CyanCube, CyanCone, CyanTorus, CyanCylinder, CyanSphere,
    MagentaCube, MagentaCone, MagentaTorus, MagentaCylinder, MagentaSphere,
    BlackCube, BlackCone, BlackTorus, BlackCylinder, BlackSphere,
    OrangeCube, OrangeCone, OrangeTorus, OrangeCylinder, OrangeSphere,
    PurpleCube, PurpleCone, PurpleTorus, PurpleCylinder, PurpleSphere,
    GrayCube, GrayCone, GrayTorus, GrayCylinder, GraySphere,
    AxesColor, NumColorBuffers
};

// Vertex array and buffer objects
GLuint VAOs[NumVAOs];
GLuint ObjBuffers[NumVAOs][NumObjBuffers];
GLuint ColorBuffers[NumColorBuffers];

// Number of vertices in each object
GLint numVertices[NumVAOs];

// Number of component coordinates
GLint posCoords = 4;
GLint normCoords = 3;
GLint texCoords = 2;
GLint colCoords = 4;

GLfloat axis_length = 3.0f;

// Model files
const char * cubeFile = "../models/unitcube.obj";
const char * coneFile = "../models/cone.obj";
const char * cylinderFile = "../models/cylinder.obj";
const char * torusFile = "../models/torus.obj";
const char * sphereFile = "../models/sphere.obj";

// Texture files
const char * blankFile = "../textures/blank.png";

// Camera
vec3 eye = {3.0f, 0.0f, 0.0f};
vec3 center = {0.0f, 0.0f, 0.0f};
vec3 up = {0.0f, 1.0f, 0.0f};
GLfloat azimuth = 0.0f;
GLfloat daz = 2.0f;
GLfloat elevation = 90.0f;
GLfloat del = 2.0f;
GLfloat radius = 3.0f;
GLfloat dr = 0.1f;
GLfloat min_radius = 2.0f;
GLfloat max_radius = 6.0f;

// Shader variables
// Default (color) shader program references
GLuint default_program;
GLuint default_vPos;
GLuint default_vCol;
GLuint default_proj_mat_loc;
GLuint default_cam_mat_loc;
GLuint default_model_mat_loc;
const char *default_vertex_shader = "../default.vert";
const char *default_frag_shader = "../default.frag";

// Global state
mat4 proj_matrix;
mat4 camera_matrix;
mat4 normal_matrix;
mat4 model_matrix;

// The list of possible colors a user can use is not dynamic in this program, and are thus predefined here.
// These are a few colors that came to mind first.
vector<pair<string, vector<float>>> colorMap = {
    {"red", {1.0f, 0.0f, 0.0f}},
    {"green", {0.0f, 1.0f, 0.0f}},
    {"blue", {0.0f, 0.0f, 1.0f}},
    {"yellow", {1.0f, 1.0f, 0.0f}},
    {"cyan", {0.0f, 1.0f, 1.0f}},
    {"magenta", {1.0f, 0.0f, 1.0f}},
    {"black", {0.0f, 0.0f, 0.0f}},
    {"orange", {1.0f, 0.5f, 0.0f}},
    {"purple", {0.5f, 0.0f, 0.5f}},
    {"gray", {0.4f, 0.4f, 0.4f}}
};

// Flag to track whether or not the user types in "quit".
atomic<bool> quitFlag(false);

// Background color
string background_color = "gray";

// Color library for shapes
unordered_map<string, GLuint> ColorLibrary;

// Cube struct to keep attributes for objects ordered.
struct object {
    string shape_type;
    vec3 position;
    vec3 scale;
    float angle;
    string color;
    object(string shape, vec3 pos, vec3 scale, float ang, string col = "") : shape_type(shape), position(pos), scale(scale), angle(ang), color(col) {}
};

// Vector of objects
vector<object> objects;

// Keeps track of all changes in current run
stack<string> state_stack;

// Global screen dimensions
GLint ww,hh;

// Rendering functions
void display();
void render_scene();
void build_geometry();
void build_solid_color_buffer(GLuint num_vertices, vec4 color, GLuint buffer);
void build_axes();
void draw_axes();
void load_model(const char * filename, GLuint obj);
void draw_color_obj(GLuint obj, GLuint color);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// Command functions
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void commandListener();
void show_welcome_screen();
void add_object(string shape, float x, float y, float z);
void move_object(int index, float dx, float dy, float dz);
void delete_object(int index);
void rotate_object(int index, float ang);
void scale_object(int index, vec3 scale_vector);
void set_background_color();
void save_state();
void load_state();
void save_change_to_stack();
void load_change_from_stack(const string& state);
void undo_state();
void clear_canvas();
void assign_color_to_object(int index, const string& colorName);
void print_help();
void list_objects();
void print_failed_command();
string lower_string(string str);
vector<float> get_color_rgb(string colorName);

// Sets everything up, such as starting the thread for the commandListener and building geometry. Also holds the while loop that renders the scene continuously.
int main(int argc, char**argv) {
	// Create OpenGL window
	GLFWwindow* window = CreateWindow("Think Inside The Box");
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    } else {
        printf("OpenGL window successfully created\n");
    }

    // Store initial window size
    glfwGetFramebufferSize(window, &ww, &hh);

    // Register callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window,key_callback);

    // Load shaders and associate variables
    ShaderInfo default_shaders[] = { {GL_VERTEX_SHADER, default_vertex_shader},{GL_FRAGMENT_SHADER, default_frag_shader},{GL_NONE, NULL} };
    default_program = LoadShaders(default_shaders);
    default_vPos = glGetAttribLocation(default_program, "vPosition");
    default_vCol = glGetAttribLocation(default_program, "vColor");
    default_proj_mat_loc = glGetUniformLocation(default_program, "proj_matrix");
    default_cam_mat_loc = glGetUniformLocation(default_program, "camera_matrix");
    default_model_mat_loc = glGetUniformLocation(default_program, "model_matrix");

    // Create geometry buffers
    build_geometry();

    // Enable depth test
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // Enable alpha blending and set blend factors
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set background color
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    load_state();
    save_change_to_stack();
    save_state();

    // Set Initial camera position
    GLfloat x, y, z;
    x = (GLfloat)(radius*sin(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
    y = (GLfloat)(radius*cos(elevation*DEG2RAD));
    z = (GLfloat)(radius*cos(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
    eye = vec3(x, y, z);

    // Starts second thread to listen on the command-line.
    thread inputThread(commandListener);

    // Main while loop for rendering.
    while (!glfwWindowShouldClose(window) && !quitFlag.load()) {
        display();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Exit while loop when program is to end and do the following...
    save_state();

    quitFlag.store(true);
    inputThread.join();

    // Close window
    glfwTerminate();
    return 0;

}

void display() {
    // Declare projection and camera matrices
    proj_matrix = mat4().identity();
    camera_matrix = mat4().identity();

    set_background_color();

	// Clear window and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute anisotropic scaling
    GLfloat xratio = 1.0f;
    GLfloat yratio = 1.0f;

    // If taller than wide adjust y
    if (ww <= hh) {
        yratio = (GLfloat)hh / (GLfloat)ww;
    } else if (hh <= ww) {                  // If wider than tall adjust x
        xratio = (GLfloat)ww / (GLfloat)hh;
    }

    // DEFAULT ORTHOGRAPHIC PROJECTION
    proj_matrix = ortho(-5.0f*xratio, 5.0f*xratio, -5.0f*yratio, 5.0f*yratio, -20.0f, 20.0f);

    // Set camera matrix
    camera_matrix = lookat(eye, center, up);

    // Render objects
	render_scene();

    // draw axes
    draw_axes();

	// Flush pipeline
	glFlush();
}

///////////////////////////////////////////////////////////////////////
/// Function: render_scene()                                        ///
/// Description: Draws all objects in the scene and applies the     ///
/// corresponding transformations to them.                          ///
/// Parameters:                                                     ///
///     N/A                                                         ///
/// Return Value:                                                   ///
///     N/A                                                         ///
///////////////////////////////////////////////////////////////////////

void render_scene() {
    // Declare model matrix
    model_matrix = mat4().identity();

    // Iterates through objects vector and draws each element accordingly.
    for (const auto& obj : objects) {
        model_matrix = translate(obj.position) * rotate(obj.angle, 0.0f, 1.0f, 0.0f) * scale(obj.scale);

        if (obj.shape_type == "cube") {
            draw_color_obj(Cube, ColorLibrary[obj.color]);
        } else if (obj.shape_type == "cone") {
            draw_color_obj(Cone, ColorLibrary[obj.color]);
        } else if (obj.shape_type == "cylinder") {
            draw_color_obj(Cylinder, ColorLibrary[obj.color]);
        } else if (obj.shape_type == "sphere") {
            draw_color_obj(Sphere, ColorLibrary[obj.color]);
        } else {
            draw_color_obj(Torus, ColorLibrary[obj.color]);
        }
    }
}

///////////////////////////////////////////////////////////////////////
/// Function: build_geometry()                                      ///
/// Description: Sets up the models and colors as well as the axes. ///
/// Parameters:                                                     ///
///     N/A                                                         ///
/// Return Value:                                                   ///
///     N/A                                                         ///
///////////////////////////////////////////////////////////////////////

void build_geometry() {
    // Generate vertex arrays and buffers
    glGenVertexArrays(NumVAOs, VAOs);

    // Load models
    load_model(cubeFile, Cube);
    load_model(coneFile, Cone);
    load_model(torusFile, Torus);
    load_model(cylinderFile, Cylinder);
    load_model(sphereFile, Sphere);

    // Generate color buffers
    glGenBuffers(NumColorBuffers, ColorBuffers);

    // // Build color buffers and define object vertex color.
    // Each color from "colorMap" is getting prefixed to each object name.
    for (const auto& color : colorMap) {
        const string& colorName = color.first;
        const vector<float>& colorVec = color.second;
        vec4 colorValue = vec4(colorVec[0], colorVec[1], colorVec[2], 1.0f);

        // Build color buffers for each shape (e.g., redCube, redCone, redTorus..., blueCube, blueCone, blueTorus...)
        build_solid_color_buffer(numVertices[Cube], colorValue, ColorLibrary.size());
        ColorLibrary[colorName + "Cube"] = ColorLibrary.size();

        build_solid_color_buffer(numVertices[Cone], colorValue, ColorLibrary.size());
        ColorLibrary[colorName + "Cone"] = ColorLibrary.size();

        build_solid_color_buffer(numVertices[Torus], colorValue, ColorLibrary.size());
        ColorLibrary[colorName + "Torus"] = ColorLibrary.size();

        build_solid_color_buffer(numVertices[Cylinder], colorValue, ColorLibrary.size());
        ColorLibrary[colorName + "Cylinder"] = ColorLibrary.size();

        build_solid_color_buffer(numVertices[Sphere], colorValue, ColorLibrary.size());
        ColorLibrary[colorName + "Sphere"] = ColorLibrary.size();
    }

    // Build axes
    build_axes();
}

// Controls key presses for canvas movement.
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // ESC to quit
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }

    // Adjust azimuth
    if (key == GLFW_KEY_A) {
        azimuth += daz;
        if (azimuth > 360.0) {
            azimuth -= 360.0;
        }
    } else if (key == GLFW_KEY_D) {
        azimuth -= daz;
        if (azimuth < 0.0)
        {
            azimuth += 360.0;
        }
    }

    // Adjust elevation angle
    if (key == GLFW_KEY_W)
    {
        elevation += del;
        if (elevation > 179.0)
        {
            elevation = 179.0;
        }
    }
    else if (key == GLFW_KEY_S)
    {
        elevation -= del;
        if (elevation < 1.0)
        {
            elevation = 1.0;
        }
    }

    // Adjust radius (zoom)
    if (key == GLFW_KEY_X)
    {
        radius += dr;
        if (radius > max_radius)
        {
            radius = max_radius;
        }
    }
    else if (key == GLFW_KEY_C)
    {
        radius -= dr;
        if (radius < min_radius)
        {
            radius = min_radius;
        }
    }

    // Compute updated camera position
    GLfloat x, y, z;
    x = (GLfloat)(radius*sin(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
    y = (GLfloat)(radius*cos(elevation*DEG2RAD));
    z = (GLfloat)(radius*cos(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
    eye = vec3(x,y,z);

}

///////////////////////////////////////////////////////////////////////
/// Function: commandListener()                                     ///
/// Description: This function runs on its own thread, constantly   ///
/// checking and interpreting user input.                           ///
/// Parameters:                                                     ///
///     N/A                                                         ///
/// Return Value:                                                   ///
///     N/A                                                         ///
///////////////////////////////////////////////////////////////////////

void commandListener() {
    show_welcome_screen();
    string command;

    while (!quitFlag.load()) {
        cout << "\nEnter command: ";
        cin >> command;
        if (command == "add") {
            string shape;
            float x, y, z;
            cout << "Enter type of shape, position (x y z): ";
            cin >> shape >> x >> y >> z;

            // Check if input was valid
            if (cin.fail()) {
                print_failed_command();
            } else {
                add_object(shape, x, y, z);
            }
        } else if (command == "move") {
            int index;
            float dx, dy, dz;
            cout << "Enter object index and movement vector (dx dy dz): ";
            cin >> index >> dx >> dy >> dz;

            // Check if input was valid
            if (cin.fail()) {
                print_failed_command();
            } else {
                move_object(index, dx, dy, dz);
            }
        } else if (command == "delete") {
            int index;
            cout << "Enter index of object you would like to delete: ";
            cin >> index;

            // Check if input was valid
            if (cin.fail()) {
                print_failed_command();
            } else {
                delete_object(index);
            }
        } else if (command == "background") {
            cout << "Enter a common color name (e.g., blue, red, yellow, etc.): ";
            cin >> background_color;

            // Check if input was valid
            if (cin.fail()) {
                print_failed_command();
            } else {
                background_color = lower_string(background_color);
            }
        } else if (command == "quit") {
            quitFlag.store(true);
        } else if (command == "load") {
            load_state();
        } else if (command == "rotate") {
            int index;
            float ang;
            cout << "Enter object index and angle: ";
            cin >> index >> ang;

            // Check if input was valid
            if (cin.fail()) {
                print_failed_command();
            } else {
                rotate_object(index, ang);
            }
        } else if (command == "scale") {
            int index;
            vec3 scale_vector;
            cout << "Enter object index and scale vector (x, y, z): ";
            cin >> index >> scale_vector[0] >> scale_vector[1] >> scale_vector[2];

            // Check if input was valid
            if (cin.fail()) {
                print_failed_command();
            } else {
                scale_object(index, scale_vector);
            }
        }  else if (command == "uscale") {
            int index;
            vec3 scale_vector;
            cout << "Enter object index and scale factor: ";
            cin >> index >> scale_vector[0];

            // Check if input was valid
            if (cin.fail()) {
                print_failed_command();
            } else {
                scale_vector[1] = scale_vector[2] = scale_vector[0];
                scale_object(index, scale_vector);
            }
        } else if(command == "clear_canvas") {
            clear_canvas();
        } else if (command == "color") {  // change color
            int index;
            string col_name;
            cout << "Enter object index and color name ";
            cin >> index >> col_name;

            // Check if input was valid
            if (cin.fail()) {
                print_failed_command();
            } else {
                assign_color_to_object(index, col_name);
            }
        } else if (command == "undo") {
            undo_state();
        } else if (command == "help") {
            print_help();
        } else if (command == "list") {
            list_objects();
        } else if (command == "clear_terminal") {
            system("cls");
        } else {
            cout << "Not a valid command" << endl;
        }
        save_change_to_stack();
        save_state();
    }
}

// Welcome screen for terminal when program first starts.
void show_welcome_screen() {
    cout << R"(
    *********************************************
    *                                           *
    *      WELCOME TO OPEN CONSOLE 1.0          *
    *                                           *
    *********************************************

    Create, modify, and manage 3D scenes with ease!

    Type 'help' to see the list of available commands.
    Type 'quit' to exit the program.

    Enjoy your session!
    )" << endl;
}

///////////////////////////////////////////////////////////////////////
/// Function: add_object()                                          ///
/// Description: Takes in the shape type and position coords        ///
/// (i.e., x, y, z) as parameters. It then creates a cube object    ///
/// and pushes it into the vector of objects.                       ///
/// Parameters:                                                     ///
///    shape (string) - Type of shape.                              ///
///    x, y, z (float) - The position coordinates.                  ///
///                                                                 ///
/// Return Value:                                                   ///
///     N/A                                                         ///
///////////////////////////////////////////////////////////////////////

void add_object(string shape, float x, float y, float z) {
    shape = lower_string(shape);

    if (shape == "cube") {
        objects.push_back(object(shape, vec3(x, y, z), vec3(1.0f, 1.0f, 1.0f), 0.0f, "redCube"));
    } else if (shape == "cone") {
        objects.push_back(object(shape, vec3(x, y, z), vec3(1.0f, 1.0f, 1.0f), 0.0f, "redCone"));
    } else if (shape == "torus") {
        objects.push_back(object(shape, vec3(x, y, z), vec3(1.0f, 1.0f, 1.0f), 0.0f, "redTorus"));
    } else if (shape == "cylinder") {
        objects.push_back(object(shape, vec3(x, y, z), vec3(1.0f, 1.0f, 1.0f), 0.0f, "redCylinder"));
    } else if (shape == "sphere") {
        objects.push_back(object(shape, vec3(x, y, z), vec3(1.0f, 1.0f, 1.0f), 0.0f, "redSphere"));
    } else {
        cerr << "'" << shape << "' is not a valid shape" << endl;
    }
}

///////////////////////////////////////////////////////////////////////
/// Function: move_object()                                          ///
/// Description: Takes in an index and amount to move the object by ///
/// (i.e., dx, dy, dz) as parameters. It then alters the            ///
/// cooresponding object by these parameters.                       ///
/// Parameters:                                                     ///
///    index (int) - The index of the object in the object vector.  ///
///    dx, dy, dz (float) - The change in position coordinates.     ///
///                                                                 ///
/// Return Value:                                                   ///
///     N/A                                                         ///
///////////////////////////////////////////////////////////////////////

void move_object(int index, float dx, float dy, float dz) {
    if (index >= 0 && index < objects.size()) {
        objects[index].position += vec3(dx, dy, dz);
    } else {
        cout << "Invalid object index." << endl;
    }
}

// Deletes the element from the objects vector with the passed index.
void delete_object(int index) {
    if (index >= 0 && index < objects.size()) {
        objects.erase(objects.begin() + index);
    } else {
        cout << "Invalid object index." << endl;
    }
}

// Sets the rotation angle of the element from the objects vector with the passed index and angle.
void rotate_object(int index, float ang) {
    if (index >= 0 && index < objects.size()) {
        objects[index].angle = ang;
    } else {
        cout << "Invalid object index." << endl;
    }
}

// Sets the scale of the element from the objects vector with the passed index and scale.
void scale_object(int index, vec3 scale_vector) {
    if (index >= 0 && index < objects.size()) {
        objects[index].scale = scale_vector;
    } else {
        cout << "Invalid object index." << endl;
    }
}

// Gets the rgb values of the new background color and clears the color buffers with the new rgb values.
void set_background_color() {
    // Get RGB value of user-inputted color
    vector<float> color_rgb = get_color_rgb(background_color);

    // Set background color
    glClearColor(color_rgb[0],color_rgb[1],color_rgb[2],1.0f);
}

// Saves the current states of the program in a .txt file called "save.txt" in /bin.
void save_state() {
    ofstream save_file("save.txt");

    if (!save_file) {
        cerr << "Error opening save file!" << endl;
        return;
    }

    // Save data
    save_file << "background_color: " << background_color << "\n" << endl;
    for (int i = 0; i < objects.size(); i++) {
        save_file << i << ": " << objects[i].shape_type << " "
                  << objects[i].position[0] << " "
                  << objects[i].position[1] << " "
                  << objects[i].position[2] << " "
                  << objects[i].scale[0] << " "
                  << objects[i].scale[1] << " "
                  << objects[i].scale[2] << " "
                  << objects[i].angle << " "
                  << objects[i].color << "\n";
    }

    save_file.close();
}

// Reads the information from the "save.txt" file from /bin and sets everything accordingly.
void load_state() {
    ifstream load_file("save.txt");

    if (!load_file) {
        cerr << "No save file found, loading default..." << endl;
        return;
    }

    string key;
    string shape_type;
    vec3 position;
    vec3 scale_vector;
    float angle;
    string color;
    while (load_file >> key) {
        if (key == "background_color:") {
            load_file >> background_color;
        } else if (isdigit(key[0])) { // Check if the key starts with a digit
            load_file >> shape_type >> position[0] >> position[1] >> position[2] >> scale_vector[0] >> scale_vector[1] >> scale_vector[2] >> angle >> color;
            objects.push_back(object(shape_type, position, scale_vector, angle, color));
        }
    }
}

// Pops off the top element from the "state_stack" which undoes the most recent change.
void undo_state() {
    if (state_stack.empty()) {
        cerr << "Undo stack is empty!" << endl;
        return;
    }

    // Pop the current state
    state_stack.pop();

    if (!state_stack.empty()) {
        // Load the previous state
        load_change_from_stack(state_stack.top());
    } else {
        cerr << "No more states to undo to!" << endl;
    }
}

// Pushes the current state of the program to the "state_stack", and does this for each change.
void save_change_to_stack() {
    ostringstream state_stream;

    // Serialize current state
    state_stream << "background_color: " << background_color << "\n";
    for (int i = 0; i < objects.size(); i++) {
        state_stream << i << ": " << objects[i].shape_type << " "
                     << objects[i].position[0] << " "
                     << objects[i].position[1] << " "
                     << objects[i].position[2] << " "
                     << objects[i].scale[0] << " "
                     << objects[i].scale[1] << " "
                     << objects[i].scale[2] << " "
                     << objects[i].angle << " "
                     << objects[i].color << "\n";
    }

    string new_state = state_stream.str();
    if (state_stack.empty() || state_stack.top() != new_state) {
        state_stack.push(new_state);
    }
}

// Called from the "undo_state" function, overwriting all current data with the data from the top of the "state_stack".
void load_change_from_stack(const string& state) {
    istringstream state_stream(state);
    string key;
    string shape_type;
    vec3 position;
    vec3 scale_vector;
    float angle;
    string color;

    objects.clear();
    while (state_stream >> key) {
        if (key == "background_color:") {
            state_stream >> background_color;
        } else if (isdigit(key[0])) { // Check if the key starts with a digit
            state_stream >> shape_type >> position[0] >> position[1] >> position[2] >> scale_vector[0] >> scale_vector[1] >> scale_vector[2] >> angle >> color;
            objects.push_back(object(shape_type, position, scale_vector, angle, color));
        }
    }
}

// Clears the object vector, making it empty.
void clear_canvas() {
    objects.clear();
}

// Changes the color field of the corresponding object to the passed colorName.
void assign_color_to_object(int index, const string& colorName) {
    if (index < 0 || index >= objects.size()) {
        cout << "Invalid object index." << endl;
        return;
    }

    if (ColorLibrary.find(colorName) != ColorLibrary.end()) {
        objects[index].color = colorName;
        cout << "Assigned color '" << colorName << "' to object ID " << index << endl;
    } else {
        cerr << "Color '" << colorName << "' not found!" << endl;
    }
}

// Prints a complete list of all avaliable commands to the terminal.
void print_help() {
    cout << "Available Commands:\n";
    cout << "  add <shape> <x> <y> <z>                         - Add a new object to the scene\n";
    cout << "  move <index> <dx> <dy> <dz>                     - Move an object in the scene\n";
    cout << "  scale <index> <scale_vector>                    - Scale an object in the scene\n";
    cout << "  uscale <index> <scale_factor>                   - Scale an object in the scene uniformly\n";
    cout << "  rotate <index> <angle>                          - Rotate an object in the scene\n";
    cout << "  delete <index>                                  - Delete an object by its index\n";
    cout << "  color <index> <color_name>                      - Change the color of a specified object\n";
    cout << "  background <color_name>                         - Change the background color\n";
    cout << "  clear_canvas                                    - Clear the canvas of all objects\n";
    cout << "  clear_terminal                                  - Clear the terminal\n";
    cout << "  undo                                            - Undo the last action\n";
    cout << "  save                                            - Save the current state to a file\n";
    cout << "  load                                            - Load the state from a file\n";
    cout << "  list                                            - List all objects in the scene\n";
    cout << "  help                                            - Display this help message\n";
    cout << "  quit                                            - Exit the program\n";
}

// Prints a complete list of all elements in the objects vector to the terminal.
void list_objects() {
    if (objects.empty()) {
        cout << "No objects in the scene.\n";
        return;
    }

    cout << "Objects in the scene:\n";
    for (size_t i = 0; i < objects.size(); ++i) {
        cout << i << ": "
                  << "Shape: " << objects[i].shape_type << ", "
                  << "Position: (" << objects[i].position[0] << ", "
                  << objects[i].position[1] << ", "
                  << objects[i].position[2] << "), "
                  << "Scale: (" << objects[i].scale[0] << ", "
                  << objects[i].scale[1] << ", "
                  << objects[i].scale[2] << "), "
                  << "Angle: " << objects[i].angle << ", "
                  << "Color: " << objects[i].color << "\n";
    }
}


#include "utilfuncs.cpp"
