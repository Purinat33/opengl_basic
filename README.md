# Graphic Pipeline
<small>Need Revision</small>

Source: 
* [learnOpenGL](https://learnopengl.com/Getting-started/Hello-Triangle)
* [The Cherno](https://www.youtube.com/@TheCherno)

<u>Goal</u>: Transform 3D coordinates into 2D pixels.

1. Transform 3D into 2D coordinates.
2. Transform 2D coordinates into actual pixels.

![Pipeline](./img/pipeline.png)


### What we need:
1. Buffer (Memory that just holds data and bytes in the **GPU**)
    * We make data representing a triangle.
    * We push those into the GPU memory.
    * We tell the GPU how to interpret the data we pushed. 
2. Shaders (Programs on the GPU)

### How OpenGL work
As a **state machine**, OpenGL already knows how to draw, we just need to tell it what shader and what buffer to use (setting state).


### Buffer

**Vertex Buffer**

Note that vertex doesn't always equal position. (normal, color etc.)
We usually mean vertex to the entire data containing position, normal, textures etc.

```C++

float vertices[]{
    // Position vertex
    -0.5f, -0.5f, 0.f,
    0.f, 0.5f, 0.f,
    0.5f, -0.5f, 0.f
};

// Create a buffer and getting the ID of the generated buffer
unsigned int vBuffer{};     // unsigned int to hold the ID of the buffer that will be generated
glGenBuffers(1, &vBuffer);   // Now we have a unique ID (vBuffer) tied to the generated buffer here
                            // (We work with ID and not name)

// Now we need to select (BIND) the buffer to be the current one (via ID)
// Like telling the draw-command "Select buffer ID vBuffer" 
glBindBuffer(GL_ARRAY_BUFFER, vBuffer); // Binding vBuffer to be the current selected/used buffer

// Passing the data to the current buffer (ID vBuffer)
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// Now whenever we call a draw command, OpenGL will use the buffer with ID vBuffer
// Imagine of it like selecting a layer in PhotoShop
// We have to select that layer to be the current one before we can do things with it (draw etc.)
```

Note that the data passed to the buffer ID `vBuffer` are just data, we still need to tell OpenGL how to interpret those data (is this position? normal? color?).

**Vertex Attribute**

We have to tell OpenGL how to interpret our data in a buffer since data in memory is just blocks of bytes.

We might have more than just positions attribute.

To tell OpenGL the structures we use: [OpenGL Documentation](https://docs.gl/gl3/glVertexAttribPointer)

```C++
void glVertexAttribPointer(	GLuint	index,
                            GLint	size,
                            GLenum	type,
                            GLboolean	normalized,
                            GLsizei	stride,
                            const GLvoid *	pointer)
```

Consider the example vertices:

```C++
float vertices[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right (1 vertex point)
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};
```

From the above snippet: **1 vertex point** contains **3 attributes**: positions, colors, and texture coords. (Try not to confuse between vertex and attributes).

**Parameters**:
1. `index`: Assigning *index* to a specific vertex attribute: position, color and texture so we can access them by index later<br>
  e.g. index = 0 for *positions* attribute, 1 for *colors*, 2 for *texture*.
2. `size`: The amount of **elements** per attribute (Not in bytes, just elements)<br>
  e.g. *positions* attribute will have size of 3, *colors* 3, and 2 for *texture coords*
3. `type`: Data type. e.g. `GL_FLOAT`, `GLuint` etc.
4. `normalized`: Do we want to transform the vertices to be in range [-1, 1] or [0, 1]?
  e.g. Color attributes might contain raw RGB value of [0, 255] (Not in the example)
5. `stride`: The **amount of bytes** between the same attribute across two vertices. <br>
  e.g. For *positions* the `stride` will be `(3 + 3 + 2) * sizeof(float)` to get from the first vertex's *positions* to the 2nd vertex's *position* (3 for *positions*, 3 for *colors* and 2 for *texture coords*)
6. `pointer`: The **amount of bytes** in the vertex to get to the attribute. <br>
    * `pointer` for *position* will be `0 * sizeof(float)` since *position* is the first attribute to be accessed immediately in a vertex. 
    * For *colors* the `pointer` will be `3 * sizeof(float)` since we have 3 floats that come before *colors*.
    * For *texture coords* the `pointer` will be `6 * sizeof(float)` because we have to offset 3 (*positions*) + 3 (*colors*) to get to *texture coords* data. <br>

![img](./img/vertex_attribute.png)

Note: The picture assumed that 1 floating point values contain 4 bytes.

Example code (*texture coords* attribute):
```C++
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
glEnableVertexAttribArray(2);  
```

`glEnableVertexAttribArray(GLuint index)` basically enables attribute with index = `index` to be used.


### VBO and VAO **buffers**
As we can see from the above section: Vertex Buffer contains vertices data and we can just bind specific buffer before we perform draw calls. These types of buffers are called **Vertex Buffer Objects** (VBO).

We then use `glVertexAttribPointer()` to tell OpenGL how to layout/interpret the Vertex data (from the Buffer), but we need to call `glVertexAttribPointer()` everytime we bind a different vertex buffer. For reusability of `glVertexAttribPointer` we use **Vertex Array Object** or **VAO** to store those pointer data.

![VAO and VBO](./img/vertex_array_objects.png)

So in the future, we can just bind a different VAO if we want to use different settings.
```C++
// This must be called before we create any vertex buffer
unsigned int VAO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);
```

It is important that a Vertex Array Object be bound before we bound a Vertex Buffer. Since any `glVertexAttribPointer` called after binding a vertex buffer object will be stored in the currently bound VAO.

Overview of steps:
```C++
// After initializing OpenGL contexts and windows
unsigned int VAO_1;
glGenVertexArrays(1, &VAO_1); 
glBindVertexArray(VAO_1); // Any subsequent glVertexAttribPointer will be stored in this vertex array

float vertices[] = {
    1.f, 1.f, 1.f,
    1.f, 1.f, 1.f,
    1.f, 1.f, 1.f
};

unsigned int vBuffer{};   
glGenBuffers(1, &vBuffer); 
glBindBuffer(GL_ARRAY_BUFFER, vBuffer); // data will be stored in this buffer

// Passing the data to the current buffer (ID vBuffer)
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
glEnableVertexAttribArray(0);  // This configuration is now in VAO_1 vertex object
// ...
// Draw calls
// Set current setting (or binding of Vertex array)
glBindVertexArray(VAO_1); 
// Set current buffer 
glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
// draw()
```


### Shaders

A shader is a **program** that runs on the GPU. (Not to be confused with light and shadows).

Important Shaders:

1. **Vertex Shaders**: Called once for each **vertex** in the vertices. Tell OpenGL where you want the vertex to be on the screen. Will take in the data/pointers specify in `glVertexAttribPointer()` and accessed via the `index` we specified.

2. **Fragment/Pixels Shaders**: Called once for each **pixel** for coloring/rasterization (e.g. lighting).

There are more types, but we don't deal with them as often as these two.

Still work in the same way with OpenGL's state machine, we just select a specific shader to use. 

**Shader source code**
1. Hard coding in the C++ file as `string` or `const char*`
2. Read from a file.

The syntax are similar to C/C++ called `GLSL`

1. Vertex Shader

```c
#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0f);
}
```
* `#version 330 core` : what version of GLSL version and profile to use.
* `layout (location = 0) in vec3 aPos;`
    * `location = 0` refers to the index 0 from our `glVertexAttribPointer()`
    * `in` : This is input
    * `vec3` is a vector containing 3 values. (remember that we got x, y, z coordinates)
    * `aPos`: We basically passed those input values to this variable
* `main()`: is main()
* `gl_Position` : Where the pixels will be on-screen (we defined it in `float vertices[]` already so we just directly copy those values into `gl_Position`). [References](https://docs.gl/el3/gl_Position)

2. Fragment Shader

```c
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
```
* `out`: refers to this being output

We output RGBA values, as seen from the variable being `vec4`

#### Program 

Having *vertex* and *fragment* shaders defined doesn't mean that we can immediately use them. We have to make a **shader program** and *link* our shaders before we can use them.

Creating shaders and a program:

1. Vertex Shader
```C++
const char *vertexShaderSource = "#version 330 core\n"
                                    "layout (location = 0) in vec3 aPos;\n" // index 0
                                    "void main()\n"
                                    "{\n"
                                    "   gl_Position = vec4(aPos, 1.0f);\n"
                                    "}\0";

unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
glCompileShader(vertexShader);

// Error checking (optional but recommended)
int success;
char infoLog[512];
glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
if (!success)
{
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
}
```

2. Fragment Shader
```C++
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
```

3. Shader Program
```C++
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

// Now that we already got the shaders in the program we can just discard these
glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);
```

Now we have to select a shader program before we can do any draw calls:
```C++
glUseProgram(shaderProgram);
```

## Drawing
We usually use one of two commands for drawing:
`glDrawArrays` or `glDrawElements`

We use `glDrawElements` when we have **index buffer** specifying the order to draw vertices. (which we don't use right now).

For simple primitives:
```C++
void glDrawArrays(	GLenum mode,
 	GLint first,
 	GLsizei count);
```
* `first`: The starting index in the enabled arrays.
* `count`: The number of indexes to render.


## Summary:

For a shape to be rendered we need to:
1. Generate and bind a **Vertex Array Object**
```C++
unsigned int VAO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);
```

2. Generate and bind a **Vertex Buffer** (VBO)
```C++
unsigned int vBuffer;
glGenBuffers(1, &vBuffer);
glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
```

3. Pass vertices to the currently bound vertex buffer:
```C++
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```

4. Configure and tell OpenGL how to interpret the vertices:
```C++
glVertexAttribPointer(0, 3 , GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

5. Create shaders
```C++
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
```

6. Create program:
```C++
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
```

7. Draw (in the render loop):
```C++
// Use shader program
glUseProgram(shaderProgram);

// Make sure that the wanted vertex array and vertex buffer are correctly bound
glBindVertexArray(VAO);
glBindBuffer(GL_ARRAY_BUFFER, vBuffer);

// Draw the shape
glDrawArrays(GL_TRIANGLES, 0, 3); 
```
Note: We usually do the draw calls after `glClear()` but before `glfwPollEvents();` and `glfwSwapBuffers(window);`
