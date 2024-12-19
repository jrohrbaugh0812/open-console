// OpenConsole - utility functions

void load_model(const char * filename, GLuint obj) {
    vector<vec4> vertices;
    vector<vec2> uvCoords;
    vector<vec3> normals;

    // Load model and set number of vertices
    loadOBJ(filename, vertices, uvCoords, normals);
    numVertices[obj] = vertices.size();

    // Create and load object buffers
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);
    glBindVertexArray(VAOs[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], uvCoords.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Create solid color buffer
void build_solid_color_buffer(GLuint num_vertices, vec4 color, GLuint buffer) {
    vector<vec4> obj_colors;
    for (int i = 0; i < num_vertices; i++) {
        obj_colors.push_back(color);
    }

    glBindBuffer(GL_ARRAY_BUFFER, ColorBuffers[buffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*colCoords*num_vertices, obj_colors.data(), GL_STATIC_DRAW);
}

// Draw object with color
void draw_color_obj(GLuint obj, GLuint color) {

    // Select default shader program
    glUseProgram(default_program);

    // Pass projection matrix to default shader
    glUniformMatrix4fv(default_proj_mat_loc, 1, GL_FALSE, proj_matrix);

    // Pass camera matrix to default shader
    glUniformMatrix4fv(default_cam_mat_loc, 1, GL_FALSE, camera_matrix);

    // Pass model matrix to default shader
    glUniformMatrix4fv(default_model_mat_loc, 1, GL_FALSE, model_matrix);

    // Bind vertex array
    glBindVertexArray(VAOs[obj]);

    // Bind position object buffer and set attributes for default shader
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glVertexAttribPointer(default_vPos, posCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(default_vPos);

    // Bind color buffer and set attributes for default shader
    glBindBuffer(GL_ARRAY_BUFFER, ColorBuffers[color]);
    glVertexAttribPointer(default_vCol, colCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(default_vCol);

    // Draw object
    glDrawArrays(GL_TRIANGLES, 0, numVertices[obj]);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    ww = width;
    hh = height;
}

void build_axes() {
    vector<vec4> vertices;
    vector<ivec3> indices;
    vector<vec4> colors;

    // Bind target vertex array object
    glBindVertexArray(VAOs[Axes]);

    // Define vertices for axes
    vertices = {
            {0.0, 0.0f, 0.0f, 1.0f},
            {axis_length, 0.0f, 0.0f, 1.0f},  // x-axis
            {0.0f, 0.0f, 0.0f, 1.0f},
            {0.0f, axis_length, 0.0f, 1.0f}, // y-axis
            {0.0f, 0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, axis_length, 1.0f}, // z-axis
    };

    // Define axis colors (red - x, green - y, blue - z)
    colors.push_back(vec4(1.0f, 0.0f, 0.0f, 1.0f));
    colors.push_back(vec4(1.0f, 0.0f, 0.0f, 1.0f));
    colors.push_back(vec4(0.0f, 1.0f, 0.0f, 1.0f));
    colors.push_back(vec4(0.0f, 1.0f, 0.0f, 1.0f));
    colors.push_back(vec4(0.0f, 0.0f, 1.0f, 1.0f));
    colors.push_back(vec4(0.0f, 0.0f, 1.0f, 1.0f));

    // Set numVertices
    numVertices[Axes] = 6;

    // Generate object buffer for table
    glGenBuffers(NumObjBuffers, ObjBuffers[Axes]);

    // Bind axes positions
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[Axes][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[Axes], vertices.data(), GL_STATIC_DRAW);

    // Bind axes colors
    glBindBuffer(GL_ARRAY_BUFFER, ColorBuffers[AxesColor]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*colCoords*numVertices[Axes], colors.data(), GL_STATIC_DRAW);
}

void draw_axes(){
    model_matrix = mat4().identity();

    // Select default shader program
    glUseProgram(default_program);

    // Pass projection matrix to default shader
    glUniformMatrix4fv(default_proj_mat_loc, 1, GL_FALSE, proj_matrix);

    // Pass camera matrix to default shader
    glUniformMatrix4fv(default_cam_mat_loc, 1, GL_FALSE, camera_matrix);

    // Pass model matrix to default shader
    glUniformMatrix4fv(default_model_mat_loc, 1, GL_FALSE, model_matrix);

    // Bind vertex array
    glBindVertexArray(VAOs[Axes]);

    // Bind position object buffer and set attributes for default shader
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[Axes][PosBuffer]);
    glVertexAttribPointer(default_vPos, posCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(default_vPos);

    // Bind color buffer and set attributes for default shader
    glBindBuffer(GL_ARRAY_BUFFER, ColorBuffers[AxesColor]);
    glVertexAttribPointer(default_vCol, colCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(default_vCol);

    // Draw object
    glDrawArrays(GL_LINES, 0, 6);
}

void print_failed_command() {
    cin.clear(); // Clear the error state
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
    cout << "Invalid input. Please enter the command in the suggested format.\n";
}

// Function to turn string lower-case
string lower_string(string str) {
    string temp_str = str;
    str = "";

    for (int i = 0; i < temp_str.length(); i++) {
        str += tolower(temp_str[i]);
    }
    return str;
}

// Function to get RGB values from a color name.
vector<float> get_color_rgb(string colorName) {
    colorName = lower_string(colorName);

    for (const auto& color : colorMap) {
        if (color.first == colorName) {
            return color.second;
        }
    }
    // Return default color (white) if not found
    return {1.0f, 1.0f, 1.0f};
}
