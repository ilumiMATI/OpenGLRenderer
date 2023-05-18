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
glm::vec3 viewTranslation = glm::vec3(0, 0, 0);
glm::vec3 viewRotation = glm::vec3(0, 0, 0);
glm::vec4 viewDirection = glm::vec4(0, 0, -1, 0);
glm::vec4 viewUpDirection = glm::vec4(0, 1, 0, 0);
glm::mat4 view = glm::translate(glm::mat4(1.0f), viewTranslation);
double xpos, ypos, ypos_old, xpos_old;

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
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
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
            1, 2, 6,
            2, 6, 5
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

        glm::mat4 proj = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -50.0f, 50.0f);
        proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
        //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 model = glm::translate(glm::mat4(1.0f),translationA);
        model = model * glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
        model = model * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0));
        model = model * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));
        glm::mat4 mvp = proj * view * model;

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 1.0, 0.5, 0.25, 1.0);
        shader.SetUniform1f("u_Time", 0.0f);
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

        float t = 0.0f;
        float dt = 0.00015f;
        float r = 1.0;
        float increment = 0.008f;
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

                shader.SetUniform4f("u_Color", 1.0, 0.5, 0.25, 1.0);
                renderer.Draw(va, ib, shader);
            }
            {
                model = glm::translate(glm::mat4(1.0f), translationB);
                model = model * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));
                model = model * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0));
                model = model * glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);

                shader.SetUniform4f("u_Color", 0.0, 0.5, 0.25, 1.0);
                renderer.Draw(va, ib, shader);
            }
            {
                model = glm::translate(glm::mat4(1.0f), translationC);
                model = model * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));
                model = model * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0));
                model = model * glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);

                shader.SetUniform4f("u_Color", 0.0, 0.1, 0.25, 1.0);
                renderer.Draw(va, ib, shader);
            }

            if (r >= 1.0f)
                increment = -increment;
            else if (r <= 0.0f)
                increment = -increment;

            r += increment;
            t += dt;

            // camera rotation
            glfwGetCursorPos(window, &xpos, &ypos);
            viewRotation.y += (xpos - xpos_old) * 0.01;
            viewRotation.x += (ypos - ypos_old) * 0.01;
            xpos_old = xpos;
            ypos_old = ypos;

            auto firstRot = glm::rotate(glm::mat4(1.0f), viewRotation.y, glm::vec3(0, 1, 0));
            auto secondRot = glm::rotate(glm::mat4(1.0f), viewRotation.x, glm::vec3(1, 0, 0));

            viewDirection = firstRot * secondRot * viewDirection;
            viewUpDirection = firstRot * secondRot * viewUpDirection;

            // camera translation
            if (glfwGetKey(window, GLFW_KEY_W))
            {
                viewTranslation.x += viewDirection.x;
                viewTranslation.y += viewDirection.y;
                viewTranslation.z += viewDirection.z;
            }
            if (glfwGetKey(window, GLFW_KEY_S))
            {
                viewTranslation.x -= viewDirection.x;
                viewTranslation.y -= viewDirection.y;
                viewTranslation.z -= viewDirection.z;
            }
            

            glm::mat4 trans = glm::translate(glm::mat4(1.0f), viewTranslation);
            glm::mat4 rotx = glm::rotate(glm::mat4(1.0f), viewRotation.x, glm::vec3(1, 0, 0));
            glm::mat4 roty = glm::rotate(glm::mat4(1.0f), viewRotation.y, glm::vec3(0, 1, 0));
            //glm::mat4 rotz = glm::rotate(glm::mat4(1.0f), viewRotation.z, glm::vec3(0, 0, 1));

            view = trans * roty * rotx;



            {
                ImGui::Begin("Debug"); // Create a window called "Hello, world!" and append into it.
                ImGui::SliderFloat3("TranslationA", &translationA.x, -800.0f, 800.0f); 
                ImGui::SliderFloat3("TranslationB", &translationB.x, -800.0f, 800.0f);
                ImGui::SliderFloat3("TranslationC", &translationC.x, -800.0f, 800.0f);
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