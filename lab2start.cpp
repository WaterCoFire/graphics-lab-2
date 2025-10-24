/* Link to static libraries, could define these as linker inputs in the project settings instead
if you prefer */
#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif
#pragma comment(lib, "opengl32.lib")

/* Include the header to the GLFW wrapper class which
   also includes the OpenGL extension initialisation */
#include "wrapper_glfw.h"
#include <iostream>

/* GLM headers */
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

// For matrix stacking
#include <stack>

GLuint positionBufferObject, colourObject;
GLuint program;
GLuint vao;

/* Position and view globals */
GLfloat angle_x, angle_x_inc;

// Personal modification: More rotation angles and increments
GLfloat angle_y, angle_y_inc, angle_z, angle_z_inc;

// Personal modification: Variables for recording translation and scaling
GLfloat cube_scale, cube_scale_inc;
glm::vec3 translation;
GLfloat trans_inc;

// --- The above are "Cube A" (the original cube) globals ---
// Personal modification below: Cube B (a new one) globals
GLfloat b_auto_angle_y = 0.0f; // Rotation angle
GLfloat b_auto_pos_x = 0.0f; // Position

// Personal modification: Camera (View) control
glm::vec3 cameraPos(0.0f, 0.0f, 3.0f); // Initial position of the camera (z: distance from the object)
glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f); // Target the camera faces (origin now)
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f); // Which direction is "up"
GLuint viewID; // Correspond to the uniform view in shader

/* Uniforms */
GLuint modelID;

// Personal modification: Color mode switching
GLuint colorMode = 0; // Current color mode: 0 indicates using the input color, 1 indicates using position-based calculation
GLuint colorModeLoc;

using namespace std;
using namespace glm;

/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(GLWrapper *glw) {
    angle_x = 0;
    angle_x_inc = 0;

    // Personal modification below: rotation angles & increments initialization
    angle_y = 0;
    angle_y_inc = 0;
    angle_z = 0;
    angle_z_inc = 0;

    // Personal modification below: translation/scaling angles & increments initialization
    cube_scale = 1.0f;
    cube_scale_inc = 0.05f;
    translation = vec3(0.0f, 0.0f, 0.0f);
    trans_inc = 0.05f; // translation step size

    // Generate index (name) for one vertex array object
    glGenVertexArrays(1, &vao);

    // Create the vertex array object and make it current
    glBindVertexArray(vao);

    /* Define vertices for a cube in 12 triangles */
    GLfloat vertexPositions[] =
            {
                    -0.25f, 0.25f, -0.25f, 1.f,
                    -0.25f, -0.25f, -0.25f, 1.f,
                    0.25f, -0.25f, -0.25f, 1.f,

                    0.25f, -0.25f, -0.25f, 1.f,
                    0.25f, 0.25f, -0.25f, 1.f,
                    -0.25f, 0.25f, -0.25f, 1.f,

                    0.25f, -0.25f, -0.25f, 1.f,
                    0.25f, -0.25f, 0.25f, 1.f,
                    0.25f, 0.25f, -0.25f, 1.f,

                    0.25f, -0.25f, 0.25f, 1.f,
                    0.25f, 0.25f, 0.25f, 1.f,
                    0.25f, 0.25f, -0.25f, 1.f,

                    0.25f, -0.25f, 0.25f, 1.f,
                    -0.25f, -0.25f, 0.25f, 1.f,
                    0.25f, 0.25f, 0.25f, 1.f,

                    -0.25f, -0.25f, 0.25f, 1.f,
                    -0.25f, 0.25f, 0.25f, 1.f,
                    0.25f, 0.25f, 0.25f, 1.f,

                    -0.25f, -0.25f, 0.25f, 1.f,
                    -0.25f, -0.25f, -0.25f, 1.f,
                    -0.25f, 0.25f, 0.25f, 1.f,

                    -0.25f, -0.25f, -0.25f, 1.f,
                    -0.25f, 0.25f, -0.25f, 1.f,
                    -0.25f, 0.25f, 0.25f, 1.f,

                    -0.25f, -0.25f, 0.25f, 1.f,
                    0.25f, -0.25f, 0.25f, 1.f,
                    0.25f, -0.25f, -0.25f, 1.f,

                    0.25f, -0.25f, -0.25f, 1.f,
                    -0.25f, -0.25f, -0.25f, 1.f,
                    -0.25f, -0.25f, 0.25f, 1.f,

                    -0.25f, 0.25f, -0.25f, 1.f,
                    0.25f, 0.25f, -0.25f, 1.f,
                    0.25f, 0.25f, 0.25f, 1.f,

                    0.25f, 0.25f, 0.25f, 1.f,
                    -0.25f, 0.25f, 0.25f, 1.f,
                    -0.25f, 0.25f, -0.25f, 1.f
            };

    /* Define an array of colours */
    float vertexColours[] = {
            0.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,

            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,

            1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,

            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,

            1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 1.0f,

            0.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f, 1.0f,
    };

    /* Create a vertex buffer object to store vertices */
    glGenBuffers(1, &positionBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Create a vertex buffer object to store vertex colours */
    glGenBuffers(1, &colourObject);
    glBindBuffer(GL_ARRAY_BUFFER, colourObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColours), vertexColours, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    try {
        program = glw->LoadShader("lab2.vert", "lab2.frag");
    }
    catch (exception &e) {
        cout << "Caught exception: " << e.what() << endl;
        cin.ignore();
        exit(0);
    }

    /* Define uniforms to send to vertex shader */
    modelID = glGetUniformLocation(program, "model");
    viewID = glGetUniformLocation(program, "view"); // Personal modification

    // Personal modification below
    colorModeLoc = glGetUniformLocation(program, "colormode");

    // Personal modification below - Creating projection matrix and pass it to shader
    glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),   // Perspective (FOV), usually 45 degree
            800.0f / 600.0f,       // Width / Height ratio
            0.1f,                  // Near Clipping Plane
            100.0f                 // Far Clipping Plane
    );
    // Pass to shader
    glUseProgram(program);
    GLuint projectionLoc = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUseProgram(0);

    // Personal modification
    glEnable(GL_DEPTH_TEST); // The code that solves the original "problem"
}

// Called to update the display.
// You should call glfwSwapBuffers() after all of your rendering to display what you rendered.
void display() {
    /* Define the background colour */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* Clear the colour and frame buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    // Personal modification: Set the view matrix (camera position)
    mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glEnableVertexAttribArray(0);

    /* glVertexAttribPointer(index, size, type, normalised, stride, pointer)
       index relates to the layout qualifier in the vertex shader and in
       glEnableVertexAttribArray() and glDisableVertexAttribArray() */
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, colourObject);
    glEnableVertexAttribArray(1);

    /* glVertexAttribPointer(index, size, type, normalised, stride, pointer)
    index relates to the layout qualifier in the vertex shader and in
    glEnableVertexAttribArray() and glDisableVertexAttribArray() */
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Personal modification
    // Simulating hierarchical transformations using stack
    stack<mat4> ms;

    // Model matrix: an identity matrix (model will be at the origin)
    mat4 model = mat4(1.0f);

    // ----- Cube A (User controlled) -----

    // 1. translation (Personal modification)
    model = translate(model, translation);

    // 2. rotation
    model = rotate(model, -angle_x, vec3(1, 0, 0)); // Rotating in clockwise direction around x-axis
    model = rotate(model, -angle_y, vec3(0, 1, 0)); // Personal modification: Rotate around y-axis
    model = rotate(model, -angle_z, vec3(0, 0, 1)); // Personal modification: Rotate around z-axis

    // Personal modification
    // Push model to stack
    ms.push(model); // "PUSH" IS DONE HERE (BEFORE SCALING) SO CUBE B WON'T SCALE WITH CUBE A

    // 3. scaling
    model = scale(model, vec3(cube_scale));

    // Send our transformations to the currently bound shader
    // (THE FOLLOWING TWO LINES ARE FOR DRAWING)
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // ----- Cube B (Personal modification, automated drawing) -----

    model = ms.top(); // Based on the current status of Cube A
    model = translate(model, vec3(b_auto_pos_x + 1.0f, 0.0f, 0.0f));
    model = rotate(model, b_auto_angle_y, vec3(0, 1, 0));
    model = glm::scale(model, vec3(0.8f));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDisableVertexAttribArray(0);

    // Personal modification - Update color mode
    glUniform1ui(colorModeLoc, colorMode); // Pass to shader

    glUseProgram(0);

    /* Modify our animation variables */
    angle_x += angle_x_inc;
    angle_y += angle_y_inc; // Personal modification for Y-axis rotation
    angle_z += angle_z_inc; // Personal modification for Z-axis rotation
}


/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow *window, int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow *window, int k, int s, int action, int mods) {
    if (action != GLFW_PRESS) return;

    if (k == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (k == 'Q') angle_x_inc += 0.001f;
    if (k == 'W') angle_x_inc -= 0.001f;

    // Personal modification below
    // For Y-/Z- axis rotation control
    if (k == 'A') angle_y_inc += 0.001f;
    if (k == 'S') angle_y_inc -= 0.001f;
    if (k == 'Z') angle_z_inc += 0.001f;
    if (k == 'X') angle_z_inc -= 0.001f;

    // Personal modification below
    // For scaling/translation control
    // Scaling control:
    if (k == GLFW_KEY_UP) cube_scale += cube_scale_inc;
    if (k == GLFW_KEY_DOWN) cube_scale -= cube_scale_inc;
    if (cube_scale < 0.1f) cube_scale = 0.1f; // Prevent it from being too small
    // Translation control:
    if (k == 'O') translation.x -= trans_inc;
    if (k == 'P') translation.x += trans_inc;
    if (k == 'K') translation.y += trans_inc;
    if (k == 'L') translation.y -= trans_inc;
    if (k == 'N') translation.z += trans_inc;
    if (k == 'M') translation.z -= trans_inc;

    // Personal modification below
    // For camera control
    if (k == GLFW_KEY_KP_4) cameraPos.x -= 0.1f;            // Move camera left
    if (k == GLFW_KEY_KP_6) cameraPos.x += 0.1f;            // Move camera left
    if (k == GLFW_KEY_KP_ADD) cameraPos.z -= 0.1f;          // Forward (towards the object)
    if (k == GLFW_KEY_KP_SUBTRACT) cameraPos.z += 0.1f;     // Backward (away from the object)
    if (k == GLFW_KEY_KP_8) cameraPos.y += 0.1f;            // Up
    if (k == GLFW_KEY_KP_5) cameraPos.y -= 0.1f;            // Down

    // Personal modification below
    // For color mode switching
    if (k == 'C') {
        colorMode = 1 - colorMode; // Switching
        printf("Colour mode switched to %u\n", colorMode);
    }
}

/* An error callback function to output GLFW errors*/
static void error_callback(int error, const char *description) {
    fputs(description, stderr);
}

/* Entry point of program */
int main(int argc, char *argv[]) {
    GLWrapper *glw = new GLWrapper(1024, 768, "Lab2: Hello 3D");;

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD. Exiting." << endl;
        return -1;
    }

    /* Note it you might want to move this call to the wrapper class */
    glw->setErrorCallback(error_callback);

    glw->setRenderer(display);
    glw->setKeyCallback(keyCallback);
    glw->setReshapeCallback(reshape);

    // Output version
    glw->DisplayVersion();

    init(glw);

    glw->eventLoop();

    delete (glw);
    return 0;
}



