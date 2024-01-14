# Graphic Pipeline
<small>Need Revision</small>

Source: 
* [learnOpenGL](https://learnopengl.com/Getting-started/Hello-Triangle)
* [The Cherno](https://www.youtube.com/@TheCherno)

<u>Goal</u>: Transform 3D coordinates into 2D pixels.

1. Transform 3D into 2D coordinates.
2. Transform 2D coordinates into actual pixels.

These are done using programs within OpenGL called **shaders** using `GLSL` language.

We can configure some shaders to our own liking (Blue).
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

### Shaders

A shader is a **program** that runs on the GPU. (Not to be confused with light and shadows).

Important Shaders:

1. **Vertex Shaders**: Called once for each **vertex** in the vertices. Tell OpenGL where you want the vertex to be on the screen. Will take in the data/pointers specify in `glVertexAttribPointer()` and accessed via the `index` we specified.

2. **Fragment/Pixels Shaders**: Called once for each **pixel** for coloring/rasterization (e.g. lighting).

There are more types, but we don't deal with them as often as these two.

Still work in the same way with OpenGL's state machine, we just select a specific shader to use. 

**Shader source code**
1. Hard coding in the C++ file (using `string`)
2. Read from a file.
