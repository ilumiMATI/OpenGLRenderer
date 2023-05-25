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
            -50.0f, +50.0f, +50.0f, // front upper left
            +50.0f, +50.0f, +50.0f, // front upper right
            +50.0f, -50.0f, +50.0f, // front lower right
            -50.0f, -50.0f, +50.0f, // front lower left
            -50.0f, +50.0f, -50.0f, // back upper left
            +50.0f, +50.0f, -50.0f, // back upper right
            +50.0f, -50.0f, -50.0f, // back lower right
            -50.0f, -50.0f, -50.0f, // back lower left
        };
        unsigned int indices[] = {
            // front face
            0, 1, 3,
            1, 2, 3,
            // back face
            4, 5, 7,
            5, 7, 6,
            // upper face
            0, 1, 4,
            1, 4, 5,
            // lower face
            3, 2, 7,
            2, 7, 6,
            // left face
            0, 3, 4,
            3, 4, 7,
            // right face
            1, 6, 2,
            1, 6, 5
        };

        VertexArray va;
        VertexBuffer vb(positions, 2 * 4 * 3 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(3);
        va.AddBuffer(vb, layout);
        IndexBuffer ib(indices, 6 * 6);

        glm::vec3 translationA = glm::vec3(0, 0, 0);
        glm::vec3 translationB = glm::vec3(0, 0, 0);
        glm::vec3 translationC = glm::vec3(0, 0, 0);
        glm::vec3 rotation = glm::vec3(0, 0, 0);

        glm::mat4 proj = glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT), -50.0f, 50.0f);
        proj = glm::perspective(glm::radians(45.0f), static_cast<float>(WIDTH) / HEIGHT, 10.0f, 1000.0f);
        glm::mat4 model = glm::translate(glm::mat4(1.0f),translationA);
        model = model * glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
        model = model * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0));
        model = model * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));
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

            shader.Bind();
            {
                model = glm::translate(glm::mat4(1.0f), translationA);
                model = model * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));
                model = model * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0));
                model = model * glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);

                shader.SetUniform4f("u_Color", 1.0, 0.5, 0.25, 0.7);
                renderer.Draw(va, ib, shader);
            }
            {
                model = glm::translate(glm::mat4(1.0f), translationB);
                model = model * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));
                model = model * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0));
                model = model * glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);

                shader.SetUniform4f("u_Color", 0.0, 0.5, 0.25, 0.7);
                renderer.Draw(va, ib, shader);
            }
            {
                model = glm::translate(glm::mat4(1.0f), translationC);
                model = model * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));
                model = model * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0));
                model = model * glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);

                shader.SetUniform4f("u_Color", 0.0, 0.1, 0.25, 0.7);
                renderer.Draw(va, ib, shader);
            }
            static bool cameraRotationActive = 0;
            static bool lastCheck = 0;
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
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
                ImGui::SliderFloat3("Rotation", &rotation.x, 0.0f, 3.14*2);

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