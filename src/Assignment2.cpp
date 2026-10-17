#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Libs/stb_image.h>

const GLint WIDTH = 800, HEIGHT = 600;

Window mainWindow;
std::vector<Mesh *> meshList;
std::vector<Shader *> shaderList;

float yaw = -118.7f, pitch = -3.8f;

std::vector<unsigned int> texList;

// Vertex Shader
static const char *vShader = "Shaders/shader.vert";

// Fragment Shader
static const char *fShader = "Shaders/shader.frag";

//light shader
static const char *lightVShader = "Shaders/lightShader.vert";
static const char *lightFShader = "Shaders/lightShader.frag";

GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;

// light Colour
glm::vec3 lightColour = glm::vec3(1.0f, 0.85f, 0.6f);

// Light position
glm::vec3 lightPos = glm::vec3(-1.25f, 3.58f, -0.45f);

Mesh *sun;

void CreateTriangle()
{
    GLfloat vertices[] =
        {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 1.0f, 0.5f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.5f, 1.0f};

    unsigned int indices[] =
        {
            0, 3, 1,
            1, 3, 2,
            2, 3, 0,
            0, 1, 2};

    Mesh *obj1 = new Mesh();
    // obj1->CreateMesh(vertices, indices, 9, 9);
    obj1->CreateMesh(vertices, indices, 20, 12);
    meshList.push_back(obj1);

    for (int i = 0; i < 10; i++)
    {
        meshList.push_back(obj1);
    }
}

void checkMouse()
{
    double mouseX, mouseY;
    glfwGetCursorPos(mainWindow.getWindow(), &mouseX, &mouseY);
    static float lastX = mouseX;
    static float lastY = mouseY;

    float xOffset = mouseX - lastX;
    float yOffset = lastY - mouseY; // Pich -> up(+), down(-)

    lastX = mouseX;
    lastY = mouseY;

    float sensitivity = 0.1f; // Mouse sensitivity
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
    {
        pitch = 89.0f; // clamp pitch not more than 89
    }
    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }
}

void CreateShaders()
{
    Shader *shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(shader1);

    Shader *shader2 = new Shader();
    shader2->CreateFromFiles(lightVShader, lightFShader);
    shaderList.push_back(shader2);
}

unsigned int loadTexture2D(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;

    unsigned char *tex = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (!tex)
    {
        std::cout << "Failed to load texture " << stbi_failure_reason() << "\n";
        return 0;
    }
    else
    {
        // Handle 1/3/4 channel images correctly
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        // Gen texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Avoid row-alignment issues for non-4 byte widths
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, tex);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(tex);

        return texture;
    }
}

std::vector<glm::vec3> meshPositions;

void CreateOBJ()
{
    Mesh *building = new Mesh();
    Mesh *personLaptop = new Mesh();
    Mesh *donationCup = new Mesh();
    Mesh *pizzaBox = new Mesh();
    Mesh *trashCan = new Mesh();
    Mesh *trashBag = new Mesh();
    Mesh *streetlight = new Mesh();
    Mesh *sidewalk = new Mesh();
    Mesh *roadGround = new Mesh();
    Mesh *pebbles = new Mesh();
    Mesh *rat = new Mesh();
    Mesh *cockroach1 = new Mesh();
    Mesh *cockroach2 = new Mesh();
    Mesh *cockroach3 = new Mesh();
    Mesh *dogColored = new Mesh();

    bool loadedBuilding = building->CreateMeshFromOBJ("Objects/building.obj");
    bool loadedPersonLaptop = personLaptop->CreateMeshFromOBJ("Objects/person_laptop_fixed.obj");
    bool loadedDonationCup = donationCup->CreateMeshFromOBJ("Objects/donation_cup_and_coins.obj");
    bool loadedPizzaBox = pizzaBox->CreateMeshFromOBJ("Objects/leftover_pizza_and_box.obj");
    bool loadedTrashCan = trashCan->CreateMeshFromOBJ("Objects/trash_can.obj");
    bool loadedTrashBag = trashBag->CreateMeshFromOBJ("Objects/trash_bag.obj");
    bool loadedStreetlight = streetlight->CreateMeshFromOBJ("Objects/streetlight.obj");
    bool loadedSidewalk = sidewalk->CreateMeshFromOBJ("Objects/sidewalk_and_kerb.obj");
    bool loadedRoadGround = roadGround->CreateMeshFromOBJ("Objects/road_ground.obj");
    bool loadedPebbles = pebbles->CreateMeshFromOBJ("Objects/ground_pebbles.obj");
    bool loadedRat = rat->CreateMeshFromOBJ("Objects/rat.obj");
    bool loadedCockroach1 = cockroach1->CreateMeshFromOBJ("Objects/cockroach_01.obj");
    bool loadedCockroach2 = cockroach2->CreateMeshFromOBJ("Objects/cockroach_02.obj");
    bool loadedCockroach3 = cockroach3->CreateMeshFromOBJ("Objects/cockroach_03.obj");
    bool loadedDogColored = dogColored->CreateMeshFromOBJ("Objects/dog_colored.obj");

    if (loadedBuilding)
    {
        texList.push_back(loadTexture2D("Textures/building_baseColor.png"));
        meshList.push_back(building);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(building);
    }

    if (loadedPersonLaptop)
    {
        texList.push_back(loadTexture2D("Textures/person_base_color.png"));
        meshList.push_back(personLaptop);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(personLaptop);
    }

    if (loadedDonationCup)
    {
        texList.push_back(loadTexture2D("Textures/donation_cup_and_coins_baseColor.png"));
        meshList.push_back(donationCup);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(donationCup);
    }

    if (loadedPizzaBox)
    {
        texList.push_back(loadTexture2D("Textures/leftover_pizza_and_box_baseColor.png"));
        meshList.push_back(pizzaBox);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(pizzaBox);
    }

    if (loadedTrashCan)
    {
        texList.push_back(loadTexture2D("Textures/trash_can_baseColor.png"));
        meshList.push_back(trashCan);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(trashCan);
    }

    if (loadedTrashBag)
    {
        texList.push_back(loadTexture2D("Textures/trash_bag_base_color.png"));
        meshList.push_back(trashBag);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(trashBag);
    }

    if (loadedStreetlight)
    {
        texList.push_back(loadTexture2D("Textures/streetlight_baseColor.png"));
        meshList.push_back(streetlight);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(streetlight);
    }

    if (loadedSidewalk)
    {
        texList.push_back(loadTexture2D("Textures/sidewalk_and_kerb_baseColor.png"));
        meshList.push_back(sidewalk);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(sidewalk);
    }

    if (loadedRoadGround)
    {
        texList.push_back(loadTexture2D("Textures/road_ground_baseColor.png"));
        meshList.push_back(roadGround);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(roadGround);
    }

    if (loadedPebbles)
    {
        texList.push_back(loadTexture2D("Textures/ground_pebbles_baseColor.png"));
        meshList.push_back(pebbles);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(pebbles);
    }

    if (loadedRat)
    {
        texList.push_back(loadTexture2D("Textures/rat_base_color.png"));
        meshList.push_back(rat);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(rat);
    }

    if (loadedCockroach1)
    {
        texList.push_back(loadTexture2D("Textures/cockroach_01_baseColor.png"));
        meshList.push_back(cockroach1);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(cockroach1);
    }

    if (loadedCockroach2)
    {
        texList.push_back(loadTexture2D("Textures/cockroach_02_baseColor.png"));
        meshList.push_back(cockroach2);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(cockroach2);
    }

    if (loadedCockroach3)
    {
        texList.push_back(loadTexture2D("Textures/cockroach_03_baseColor.png"));
        meshList.push_back(cockroach3);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(cockroach3);
    }

    if (loadedDogColored)
    {
        texList.push_back(loadTexture2D("Textures/dog_base_color.png"));
        meshList.push_back(dogColored);
        meshPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        delete(dogColored);
    }

    sun = new Mesh();
    bool sunLoaded = sun->CreateMeshFromOBJ("Objects/sphere.obj");
    if (!sunLoaded)
    {
        std::cout << "Failed to load model" << std::endl;
        delete(sun);
    }
}

void renderScene(glm::mat4 view, glm::mat4 projection)
{
    // Object
    for (int i = 0; i < meshList.size(); i++)
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, meshPositions[i]);
        model = glm::scale(model, glm::vec3(1.0f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        GLint unitformTexture1 = shaderList[0]->GetUniformLocation("texture1");
        glUniform1i(unitformTexture1, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texList[i]);
        meshList[i]->RenderMesh();
    }
}

int main()
{
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    mainWindow.initialise();

    CreateOBJ();
    CreateShaders();
    // About delta time

    float deltaTime = 0.0f;
    float lastTime = 0.0f;

    glm::vec3 cameraPos = glm::vec3(0.92f, 1.01f, 0.68f);
    glm::vec3 cameraTarget = glm::vec3(-0.35f, 0.82f, -1.65f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);

    // Loop until window closed
    while (!mainWindow.getShouldClose())
    {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        // Get + Handle user input events
        glfwPollEvents();
        checkMouse();

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraDirection = glm::normalize(direction);

        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraPos += cameraDirection * 5.0f * deltaTime;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPos -= cameraDirection * 5.0f * deltaTime;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraPos -= cameraRight * 5.0f * deltaTime;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraPos += cameraRight * 5.0f * deltaTime;
        }

        // Calculate cameraRight and up again after calculate newly cameraDirection
        cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

        // Clear window
        glClearColor(0.02f, 0.03f, 0.06f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view(1.0f);

        glm::mat4 cameraPosMat(1.0f);

        cameraPosMat[3][0] = -cameraPos.x;
        cameraPosMat[3][1] = -cameraPos.y;
        cameraPosMat[3][2] = -cameraPos.z;

        glm::mat4 cameraRotateMat(1.0f);
        cameraRotateMat[0] = glm::vec4(cameraRight.x, cameraUp.x, -cameraDirection.x, 0.0f);
        cameraRotateMat[1] = glm::vec4(cameraRight.y, cameraUp.y, -cameraDirection.y, 0.0f);
        cameraRotateMat[2] = glm::vec4(cameraRight.z, cameraUp.z, -cameraDirection.z, 0.0f);
        // view = cameraRotateMat * cameraPosMat;
        view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

        // draw here
        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetUniformLocation("model");
        uniformProjection = shaderList[0]->GetUniformLocation("projection");
        uniformView = shaderList[0]->GetUniformLocation("view");

        //light
        glUniform3fv(shaderList[0]->GetUniformLocation("lightColour"), 1, (GLfloat *)&lightColour);
        glUniform3fv(shaderList[0]->GetUniformLocation("lightPos"), 1, (GLfloat *)&lightPos);
        glUniform3fv(shaderList[0]->GetUniformLocation("viewPos"), 1, (GLfloat *)&cameraPos);

        renderScene(view, projection);

        //light
        shaderList[1]->UseShader();
        uniformModel = shaderList[1]->GetUniformLocation("model");
        uniformView = shaderList[1]->GetUniformLocation("view");
        uniformProjection = shaderList[1]->GetUniformLocation("projection");

        glm::mat4 model(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.15f, 0.05f, 0.12f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        //light colour
        glUniform3fv(shaderList[1]->GetUniformLocation("lightColour"), 1, (GLfloat *)&lightColour);
        sun->RenderMesh();

        glUseProgram(0);
        // end draw

        mainWindow.swapBuffers();
    }
    return 0;
}
