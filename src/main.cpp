#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1080, 720, "Triangle", NULL, NULL);
    if(window == NULL){
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glViewport(0,0,1080,720);

    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n" // vec3 because 3d coordinate
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
                                     "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n" // vec3 because 3d coordinate
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(1.0f, 0.7f, 0.3f, 1.0f);\n"
                                       "}\0";

    GLuint vBuffer{}; // Buffer that will contain the vertices
    glGenBuffers(1, &vBuffer); // generating a buffer and getting an ID associated with this buffer (value stored in vBuffer), so when we do drawing we can just specify "buffer no. 5" etc.
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer); // Selecting the buffer as current (binding), think of it as selecting 'buffer' layer in photoshop
 
    float vertices[]{
        // Position vertex
        -0.5f, -0.5f, 0.f,
        0.f, 0.5f, 0.f,
        0.5f, -0.5f, 0.f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // We have given the data to OpenGL
    // But OpenGL doesn't know how to intepret this data (normal? shader? color?)
    // Do this with:
    // void glVertexAttribPointer(GLuint index,
    //                            GLint size,
    //                            GLenum type,
    //                            GLboolean normalized,
    //                            GLsizei stride,
    //                            const GLvoid *pointer);
    // @param
    // index:       pecifies the index of the generic vertex attribute to be modified.
    //              e.g. (index 0 for position, 1 for texture coordinate, 2 is color etc).
    // size:        how many data per vertex (size is 3 for our vertices[] because 3 per vertex)
    // type:        data type
    // normalized:  do we want the value to be reduced to between -1 to 1 ? (color 0-255)
    // stride:      The amount of bytes between each vertex of the same type (3 floats of position) 
    //              Important in case our vertices[] contain vertex of other type as well (color etc.)
    // pointer:     offset to the start of the vertex of same type (0 bytes in is position immediately)

    glVertexAttribPointer(0, 3 , GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Enable index 0
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Drawing
        // Draw the current buffer bounded.
        glDrawArrays(GL_TRIANGLES, 0, 3); // If we don't have an Index Buffer
        // glDrawElements(GL_TRIANGLES, 3, NULL); // If we use index Buffer

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Unbinding buffer (no buffer selected)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
}

void processInput(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
