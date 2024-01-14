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



    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    // VAO 
    // You have to specify this (and before any buffers creation) to make it work
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Buffers
    unsigned int vBuffer{}; // Buffer that will contain the vertices
    glGenBuffers(1, &vBuffer); // generating a buffer and getting an ID associated with this buffer (value stored in vBuffer), so when we do drawing we can just specify "buffer no. 5" etc.
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer); // Selecting the buffer as current (binding), think of it as selecting 'buffer' layer in photoshop

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
    
    glVertexAttribPointer(0, 3 , GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Enable index 0
    glEnableVertexAttribArray(0);

    // Shader
    // Vertex Shader
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n" // layout (location = 0) means index 0 from glVertexAttribPointer() and copied it into aPos variable
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos, 1.0f);\n"
                                     "}\0";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Error checking
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // fragment shader
    const char *fragmentShaderSource = "#version 330 core\n"
                                        "out vec4 FragColor;\n"
                                        "void main()\n"
                                        "{\n"
                                        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                        "}\0";
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);

    // Validate program
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Now that we already got the shaders in the program we can
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // glClearColor(0.f, 0.f, 1.f, 1.f);
        glClearColor(0.1f, 0.4f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        

        // Use shader program
        glUseProgram(shaderProgram);

        // Drawing
        // Draw the current buffer bounded.
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 3); // If we don't have an Index Buffer
        // glDrawElements(GL_TRIANGLES, 3, NULL); // If we use index Buffer

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Unbinding buffer (no buffer selected)
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

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
