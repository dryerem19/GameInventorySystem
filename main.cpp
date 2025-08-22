#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "storage.h"

#define WIDTH   800
#define HEIGHT  600
#define TITLE   "Inventory System Demo"

#define INVENTORY_ROWS              5
#define INVENTORY_COLUMNS           10
#define INVENTORY_CELL_SIZE         32
#define INVENTORY_WIDTH             INVENTORY_COLUMNS * INVENTORY_CELL_SIZE
#define INVENTORY_HEIGHT            INVENTORY_ROWS * INVENTORY_CELL_SIZE
#define MAX_INVENTORY_WEIGHT        100.0f
#define INVENTORY_BORDER_COLOR      IM_COL32(200, 200, 200, 255)

float                           currentWeight = 0;
Inventory::Storage              storage(MAX_INVENTORY_WEIGHT);
std::vector<Inventory::Item>    availableItems = {
    {1, "Water", "Bottle of water", 0.6f},
    {2, "Sword", "Weapon", 3.5f},
    {3, "Apple", "Food", 0.2f}
};

void draw_inventory(void)
{
    ImGui::Begin("Inventory", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    {
        // Invisible widget so that autoresize works correctly
        ImGui::InvisibleButton("##inventory_grid",
                               ImVec2(INVENTORY_WIDTH, INVENTORY_HEIGHT));

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        cursorPos.y -= INVENTORY_HEIGHT; // Drawing on top of an invisible widget

        for (int y = 0; y < INVENTORY_ROWS; y++) {
            for (int x = 0; x < INVENTORY_COLUMNS; x++) {
                ImVec2 cellMin(cursorPos.x + x * INVENTORY_CELL_SIZE, cursorPos.y + y * INVENTORY_CELL_SIZE);
                ImVec2 cellMax(cellMin.x + INVENTORY_CELL_SIZE, cellMin.y + INVENTORY_CELL_SIZE);
                drawList->AddRect(cellMin, cellMax, INVENTORY_BORDER_COLOR);
            }
        }

        ImGui::SetCursorPosY(ImGui::GetCursorPosY());
        ImGui::Text("Weight: %.1f / %.1f", storage.currentWeight(), storage.maxWeight());
    }
    ImGui::End();
}

void draw_available_items(void)
{
    ImGui::Begin("Items", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    {
        if (ImGui::BeginTable("ItemsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Item");
            ImGui::TableSetupColumn("Weight");
            ImGui::TableSetupColumn("Action");
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < availableItems.size(); i++)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", availableItems[i].name().c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.1f", availableItems[i].weight());

                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button(("Append##" + std::to_string(i)).c_str())) {
                    storage.addItem(std::make_unique<Inventory::Item>(availableItems[i]));
                }
            }

            ImGui::EndTable();
        }
    }
    ImGui::End();
}

int main(void)
{
    if (!glfwInit()) {
        return -1;
    }

    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Ma

    float scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow((int)(WIDTH * scale), (int)(HEIGHT * scale), TITLE, nullptr, nullptr);
    if (window == nullptr) {
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        draw_available_items();
        draw_inventory();

        // Rendering
        ImGui::Render();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
