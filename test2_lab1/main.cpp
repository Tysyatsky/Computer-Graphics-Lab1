

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>

void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void processInput(GLFWwindow * window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos, 1.0);\n"
"   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n" // vertex shader for positioning
"}\0";

const char* fragmentShaderSource = "#version 330 core\n" // fragment shader for color
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.0f, 0.6f, 0.0f, 1.0f);\n"
"}\n\0";

float def_x = -0.1f; // variables for movement
float def_y = 0.1f;

float x = 0.0f;
float y = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //initialize

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab1", NULL, NULL); // glfw window creation
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl; // error with window creation catch
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // make window current
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // call back for window

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // load all function pointers
    {
        std::cout << "Failed to initialize GLAD" << std::endl; // check loading
        return -1;
    }


    // creating shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // vertex shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // adding code to shader
    glCompileShader(vertexShader); // compiling shader

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // checking errors
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);     // fragment shader
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); // check for errors
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram(); // linking shaders to shader program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // check for errors
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader); // after linking we can delete all shaders
    glDeleteShader(fragmentShader);

    // creating vertices for our animal
    float vertices[] = {
         -0.4f + x, 0.2f + y, 0.0f,  // head 2 0
        -0.4f + x, -0.2f + y, 0.0f,  // head 3 1
        -0.7f + x,  -0.2f + y, 0.0f, // head 1 2
        -0.7f + x,  0.2f + y, 0.0f, // head 4 3

        -0.7f + x, 0.6f + y, 0.0f, // head piece left 4
        -0.5f + x, 0.4f + y, 0.0f, // 5

        -0.7f + x,  0.2f + y, 0.0f, // head piece right 6
        -0.4f + x, 0.5f + y, 0.0f, // 7
        -0.4f + x, 0.2f + y, 0.0f, // 8

        -0.4f + x, -0.6f + y, 0.0f, // 9
        -0.4f + x, 0.0f + y, 0.0f, // 10
        0.0f + x, 0.0f + y, 0.0f, // 11

        0.0f + x, 0.0f + y, 0.0f, // 12
        -0.3f + x, -0.3f + y, 0.0f,// 13
        0.3f + x, -0.3f + y, 0.0f, // 14

        0.0f + x, 0.0f + y, 0.0f, // 15
        0.4f + x, -0.6f + y, 0.0f, // 16
        0.4f + x, 0.0f + y, 0.0f, // 17
        
        0.5f + x, 0.3f + y, 0.0f, // 18
        0.8f + x, 0.5f + y, 0.0f, // 19
        0.7f + x, 0.2f + y, 0.0f, // 20
        0.4f + x, 0.0f + y, 0.0f  // 21

    };
    unsigned int indices[] = { 
        0, 1, 3,  // head  1
        1, 2, 3,   // head 2
        18, 19, 20,
        18, 21, 20
    };
    unsigned int VBO, VAO, EBO; // creating buffers
    glGenVertexArrays(1, &VAO); // generating vertex arrays
    glGenBuffers(1, &VBO); 
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO); // binding buffers

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // connecting data

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // connecting data for squares 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // memory for vertex
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) // render
    {
        processInput(window); // getting input

        glClearColor(0.8f, 0.8f, 0.0f, 1.0f); // background color
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram); // using shaders
        glBindVertexArray(VAO);

        glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        transform = glm::translate(transform, glm::vec3(x, y, 0.0f)); // translate position

        glUseProgram(shaderProgram);
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform"); // init transform
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform)); // setting matrix

        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0); // drawing
        glDrawArrays(GL_TRIANGLES, 0, 18);

        glfwSwapBuffers(window); // swap buffers 
        glfwPollEvents(); // poll IO events(keys pressed / released, mouse moved etc.)
    }

    
    glDeleteVertexArrays(1, &VAO); // de-allocate all resources once they've outlived their purpose
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow * window) // processing all input
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        y += def_y;
        std::cout << "Y: " << y << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        y -= def_y;
        std::cout << "Y: " << y << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        x += def_x;
        std::cout << "X: " << x << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        x -= def_x;
        std::cout << "X: " << x << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}