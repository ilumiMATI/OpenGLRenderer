#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#include "glm/glm.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);  // Camera position in world space
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f); // Point the camera is looking at
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);        // Up vector
glm::mat4 view = glm::mat4(1.0f);

glm::vec2 mouseInput;

// Define camera movement variables
float cameraSpeed = 0.5f;
float cameraSensitivity = 0.5f;
float yaw, pitch;

double xpos, ypos, ypos_old, xpos_old;

constexpr unsigned WIDTH = 1280;
constexpr unsigned HEIGHT = 720;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "Camera Movement", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error: GLEW init failed" << std::endl;
    else
        std::cout << "Success: GLEW init completed" << std::endl;

    /* Information about the graphics card */
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl
        << "Graphics vendor: " << glGetString(GL_VENDOR) << std::endl
        << "Graphics card: " << glGetString(GL_RENDERER) << std::endl;
    {

        float positions[] = {
            -50.0f, +50.0f, +50.0f, //1.0f, 1.0f, 1.0f, 1.0f, // front upper left 0
            +50.0f, +50.0f, +50.0f, //1.0f, 1.0f, 0.0f, 1.0f, // front upper right 1
            +50.0f, -50.0f, +50.0f, //1.0f, 0.0f, 1.0f, 1.0f, // front lower right 2
            -50.0f, -50.0f, +50.0f, //0.0f, 1.0f, 1.0f, 1.0f, // front lower left 3
            -50.0f, +50.0f, -50.0f, //1.0f, 0.0f, 0.0f, 1.0f, // back upper left 4
            +50.0f, +50.0f, -50.0f, //0.0f, 1.0f, 0.0f, 1.0f, // back upper right 5
            +50.0f, -50.0f, -50.0f, //0.0f, 0.0f, 1.0f, 1.0f, // back lower right 6
            -50.0f, -50.0f, -50.0f //0.0f, 0.0f, 0.0f, 1.0f, // back lower left 7
        };

        float positions2[] = {
            -50.0f, +50.0f, +50.0f, 1.0f, 1.0f, 1.0f, 1.0f, // tr 1
            +50.0f, +50.0f, +50.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -50.0f, -50.0f, +50.0f, 1.0f, 1.0f, 1.0f, 1.0f,

            +50.0f, +50.0f, +50.0f, 1.0f, 1.0f, 0.0f, 1.0f, // tr 2
            +50.0f, -50.0f, +50.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -50.0f, -50.0f, +50.0f, 1.0f, 1.0f, 0.0f, 1.0f,

            -50.0f, +50.0f, -50.0f, 1.0f, 0.0f, 1.0f, 1.0f, // tr 3
            +50.0f, +50.0f, -50.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -50.0f, -50.0f, -50.0f, 1.0f, 0.0f, 1.0f, 1.0f,

            +50.0f, +50.0f, -50.0f, 0.0f, 1.0f, 1.0f, 1.0f, // tr 4
            -50.0f, -50.0f, -50.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            +50.0f, -50.0f, -50.0f, 0.0f, 1.0f, 1.0f, 1.0f,

            -50.0f, +50.0f, +50.0f, 1.0f, 0.0f, 0.0f, 1.0f, // tr 5
            +50.0f, +50.0f, +50.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -50.0f, +50.0f, -50.0f, 1.0f, 0.0f, 0.0f, 1.0f,

            +50.0f, +50.0f, +50.0f, 0.0f, 1.0f, 0.0f, 1.0f, // tr 6
            -50.0f, +50.0f, -50.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            +50.0f, +50.0f, -50.0f, 0.0f, 1.0f, 0.0f, 1.0f,

            -50.0f, -50.0f, +50.0f, 0.0f, 0.0f, 1.0f, 1.0f,// tr 7
            +50.0f, -50.0f, +50.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -50.0f, -50.0f, -50.0f, 0.0f, 0.0f, 1.0f, 1.0f,

            +50.0f, -50.0f, +50.0f, 0.5f, 0.5f, 0.5f, 1.0f,// tr 8
            -50.0f, -50.0f, -50.0f, 0.5f, 0.5f, 0.5f, 1.0f,
            +50.0f, -50.0f, -50.0f, 0.5f, 0.5f, 0.5f, 1.0f,

            -50.0f, +50.0f, +50.0f, 0.5f, 0.5f, 0.0f, 1.0f, // tr 9
            -50.0f, -50.0f, +50.0f, 0.5f, 0.5f, 0.0f, 1.0f,
            -50.0f, +50.0f, -50.0f, 0.5f, 0.5f, 0.0f, 1.0f,

            -50.0f, -50.0f, +50.0f, 0.5f, 0.0f, 0.5f, 1.0f,// tr 10
            -50.0f, +50.0f, -50.0f, 0.5f, 0.0f, 0.5f, 1.0f,
            -50.0f, -50.0f, -50.0f, 0.5f, 0.0f, 0.5f, 1.0f,

            +50.0f, +50.0f, +50.0f, 0.0f, 0.5f, 0.5f, 1.0f,// tr 11
            +50.0f, -50.0f, -50.0f, 0.0f, 0.5f, 0.5f, 1.0f,
            +50.0f, -50.0f, +50.0f, 0.0f, 0.5f, 0.5f, 1.0f,

            +50.0f, +50.0f, +50.0f, 0.5f, 0.0f, 0.0f, 1.0f,// tr 12
            +50.0f, -50.0f, -50.0f, 0.5f, 0.0f, 0.0f, 1.0f,
            +50.0f, +50.0f, -50.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        };
        unsigned int indices[] = {
            // front face
            0, 1, 3, // tr 1
            1, 2, 3, // tr 2
            // back face
            4, 5, 7, // tr 3
            5, 7, 6, // tr 4
            // upper face
            0, 1, 4, // tr 5
            1, 4, 5, // tr 6
            // lower face
            3, 2, 7, // tr 7
            2, 7, 6, // tr 8
            // left face
            0, 3, 4, // tr 9
            3, 4, 7, // tr 10
            // right face
            1, 6, 2, // tr 11
            1, 6, 5 // tr 12
        };

        float colors[12 * 4] =
        {
            1.0f, 1.0f, 1.0f, 1.0f, // tr1
            1.0f, 1.0f, 0.0f, 1.0f, // tr2
            1.0f, 0.0f, 1.0f, 1.0f, // tr3
            0.0f, 1.0f, 1.0f, 1.0f, // tr4
            1.0f, 0.0f, 0.0f, 1.0f, // tr5
            0.0f, 1.0f, 0.0f, 1.0f, // tr6
            0.0f, 0.0f, 1.0f, 1.0f, // tr7
            0.5f, 0.5f, 0.5f, 1.0f, // tr8
            0.5f, 0.5f, 0.0f, 1.0f, // tr9
            0.5f, 0.0f, 0.5f, 1.0f, // tr10
            0.0f, 0.5f, 0.5f, 1.0f, // tr11
            0.5f, 0.0f, 0.0f, 1.0f, // tr12
        };

        unsigned int elements[12*3];
        for (unsigned short i = 0; i < 12 * 3; i++)
            elements[i] = i;

        VertexArray va;
        //VertexBuffer vb(positions2, 7 * 36 * sizeof(float)); // different color of triangles variant ( use frag_color in fragment shader to work )
        VertexBuffer vb(positions, 7 * 8 * sizeof(float)); // different color of triangles variant ( use u_Color in fragment shader to work )
        VertexBufferLayout layout;
        layout.Push<float>(3); // position
        //layout.Push<float>(4); // color // comment if you use 3 cubes variant
        va.AddBuffer(vb, layout);
        //IndexBuffer ib(elements, 6 * 6); // different colors of triangles variant ( use frag_color in fragment shader to work )
        IndexBuffer ib(indices, 6 * 6); // different colors of 3 cubes variant ( use u_Color in fragment shader to work )

        float znear = 10.0f;
        float zfar = 1000.0f;

        glm::vec3 translationA = glm::vec3(-115.0f, 0, -300.0f);
        glm::vec3 translationB = glm::vec3(0, 0, -300.0f);
        glm::vec3 translationC = glm::vec3(115.0f, 0, -300.0f);
        glm::vec3 rotation = glm::vec3(0, 0, 0);
        glm::mat4 projOrtho = glm::ortho(-static_cast<float>(WIDTH) / 2, static_cast<float>(WIDTH)/2, -static_cast<float>(HEIGHT) / 2, static_cast<float>(HEIGHT)/2, znear, zfar);
        glm::mat4 projPersp = glm::perspective(glm::radians(45.0f), static_cast<float>(WIDTH) / HEIGHT, znear, zfar);
        glm::mat4 proj = projPersp;
        glm::mat4 model = glm::translate(glm::mat4(1.0f),translationA);
        model = model * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));
        model = model * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0));
        model = model * glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
        glm::mat4 mvp = proj * view * model;

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 1.0, 0.5, 0.25, 1.0);
        shader.SetUniformMat4f("u_MVP", mvp);

        // unbinding
        va.Unbind();
        shader.Unbind();
        vb.Unbind();

        Renderer renderer;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui::StyleColorsDark();
        ImGui_ImplOpenGL3_Init((char*)glGetString(330));
        bool isPersp = 1;

        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glEnable(GL_DEPTH_TEST);
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            projOrtho = glm::ortho(-static_cast<float>(WIDTH) / 2, static_cast<float>(WIDTH) / 2, -static_cast<float>(HEIGHT) / 2, static_cast<float>(HEIGHT) / 2, znear, zfar);
            projPersp = glm::perspective(glm::radians(45.0f), static_cast<float>(WIDTH) / HEIGHT, znear, zfar);
            if (isPersp)
                proj = projPersp;
            else
                proj = projOrtho;


            shader.Bind();
            {
                model = glm::translate(glm::mat4(1.0f), translationA);
                model = model * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
                model = model * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0));
                model = model * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0));
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);

                shader.SetUniform4f("u_Color", 1.0, 0.5, 0.25, 0.7);
                renderer.Draw(va, ib, shader);
            }
            {
                model = glm::translate(glm::mat4(1.0f), translationB);
                model = model * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
                model = model * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0));
                model = model * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0));
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);

                shader.SetUniform4f("u_Color", 0.0, 0.5, 0.25, 0.7);
                renderer.Draw(va, ib, shader);
            }
            {
                model = glm::translate(glm::mat4(1.0f), translationC);
                model = model * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
                model = model * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0));
                model = model * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0));
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);

                shader.SetUniform4f("u_Color", 0.0, 0.1, 0.25, 0.7);
                renderer.Draw(va, ib, shader);
            }
            static bool cameraRotationActive = 0;
            static bool lastCheck = 0;
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
            {
                if(lastCheck == 0)
                    cameraRotationActive = !cameraRotationActive;
                lastCheck = 1;
            }
            else lastCheck = 0;


            // camera rotation
            glfwGetCursorPos(window, &xpos, &ypos);
            if (cameraRotationActive) 
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                yaw += (xpos - xpos_old) * cameraSensitivity;
                pitch += (ypos - ypos_old) * cameraSensitivity;
            }
            else
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            xpos_old = xpos;
            ypos_old = ypos;

            // Clamp pitch to avoid flipping the camera
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            // if vec3 rotation worked
            // glm::vec3 newCameraDirection = glm::rotate(cameraDirection, glm::radians(-yaw), glm::normalize(cameraUp));

            glm::mat4 yawRot = glm::rotate(glm::mat4(1), glm::radians(-yaw), glm::normalize(cameraUp));
            glm::vec3 newCameraDirection = glm::vec3(yawRot * glm::vec4(cameraDirection,0.0));

            glm::mat4 pitchRot = glm::rotate(glm::mat4(1), glm::radians(-pitch), glm::normalize(glm::cross(newCameraDirection, cameraUp)));
            newCameraDirection = glm::vec3(pitchRot * glm::vec4(newCameraDirection, 0.0));
            glm::vec3 newCameraUp = glm::vec3(pitchRot * glm::vec4(cameraUp, 0.0));
            static float speedMultiplier = 1.0f;
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
                speedMultiplier = 2.0f;
            else
                speedMultiplier = 1.0f;
            // camera translation
            if (glfwGetKey(window, GLFW_KEY_W))
            {
                cameraPosition += glm::normalize(newCameraDirection) * cameraSpeed * speedMultiplier;

            }
            if (glfwGetKey(window, GLFW_KEY_S))
            {
                cameraPosition -= glm::normalize(newCameraDirection) * cameraSpeed * speedMultiplier;
            }
            if (glfwGetKey(window, GLFW_KEY_A))
            {
                cameraPosition -= glm::normalize(glm::cross(newCameraDirection, newCameraUp)) * cameraSpeed * speedMultiplier;
            }
            if (glfwGetKey(window, GLFW_KEY_D))
            {
                cameraPosition += glm::normalize(glm::cross(newCameraDirection, newCameraUp)) * cameraSpeed * speedMultiplier;
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE))
            {
                cameraPosition += glm::normalize(newCameraUp) * cameraSpeed * speedMultiplier;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
            {
                cameraPosition -= glm::normalize(newCameraUp) * cameraSpeed * speedMultiplier;
            }


            /*glm::mat4 viewMatrix(1.0f);
            viewMatrix = glm::rotate(viewMatrix, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
            viewMatrix = glm::rotate(viewMatrix, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
            viewMatrix = glm::translate(viewMatrix, -cameraPosition);*/

            view = glm::lookAt(cameraPosition,cameraPosition + newCameraDirection, newCameraUp);

            {
                ImGui::Begin("Debug"); // Create a window called "Hello, world!" and append into it.
                ImGui::SliderFloat3("TranslationA", &translationA.x, -200.0f, 200.0f); 
                ImGui::SliderFloat3("TranslationB", &translationB.x, -200.0f, 200.0f);
                ImGui::SliderFloat3("TranslationC", &translationC.x, -200.0f, 200.0f);
                ImGui::SliderFloat3("Rotation", &rotation.x, 0.0f, 360.0f);
                ImGui::Checkbox("Perspective", &isPersp);
                ImGui::InputFloat("znear", &znear, 0.1f, 0.5f);
                ImGui::InputFloat("zfar", &zfar, 0.1f, 0.5f);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}