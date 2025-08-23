#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "IconsFontAwesome7.h"

#include "storage.h"

#define WIDTH   800
#define HEIGHT  600
#define TITLE   "Inventory System Demo"

#define INVENTORY_ROWS              5
#define INVENTORY_COLUMNS           10
#define INVENTORY_CELL_SIZE         40
#define INVENTORY_BORDER_COLOR      IM_COL32(200, 200, 200, 255)

float                           currentWeight = 0;
Inventory::Storage              storage(5, 10, 100);
std::vector<Inventory::Item>    availableItems = {
    {1, "Water", "Bottle of water", 0.6f},
    {2, "Gun", "Weapon", 3.5f},
    {3, "Burger", "Food", 0.2f}
};
std::vector<std::string>        itemIcons = {
    ICON_FA_BOTTLE_WATER,
    ICON_FA_GUN,
    ICON_FA_BURGER
};

void draw_inventory(void)
{
    int width   = storage.cols() * INVENTORY_CELL_SIZE;
    int height  = storage.rows() * INVENTORY_CELL_SIZE;

    ImGui::Begin("Inventory", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    {
        // Invisible widget so that autoresize works correctly
        ImGui::InvisibleButton("##inventory_grid", ImVec2(width, height));

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        cursorPos.y -= height; // Drawing on top of an invisible widget

        for (int y = 0; y < storage.rows(); y++) {
            for (int x = 0; x < storage.cols(); x++)
            {
                ImVec2 cellMin(cursorPos.x + x * INVENTORY_CELL_SIZE, cursorPos.y + y * INVENTORY_CELL_SIZE);
                ImVec2 cellMax(cellMin.x + INVENTORY_CELL_SIZE, cellMin.y + INVENTORY_CELL_SIZE);
                drawList->AddRect(cellMin, cellMax, INVENTORY_BORDER_COLOR);                

                auto item = storage.getItem(y, x);
                if (item)
                {
                    int         iconIndex   = y * storage.cols() + x;
                    const char* iconFont    = itemIcons[iconIndex].c_str();
                    ImVec2      iconSize    = ImGui::CalcTextSize(iconFont);
                    ImVec2      iconPos(
                        cellMin.x + (INVENTORY_CELL_SIZE - iconSize.x) * 0.5f,
                        cellMin.y + (INVENTORY_CELL_SIZE - iconSize.y) * 0.5f
                    );

                    drawList->AddText(iconPos, IM_COL32(255, 255, 255, 255), iconFont);

                    char buffer[16];
                    snprintf(buffer, sizeof(buffer), "%.1f", item->weight() * item->stackCount());

                    std::string weightText = buffer;
                    ImVec2      weightSize = ImGui::CalcTextSize(weightText.c_str());
                    ImVec2      weightPos(
                        cellMax.x - weightSize.x - 2,
                        cellMax.y - weightSize.y - 2
                    );

                    drawList->AddText(weightPos, IM_COL32(180, 180, 180, 255), weightText.c_str());
                }
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
        if (ImGui::BeginTable("ItemsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Item");
            ImGui::TableSetupColumn("Icon");
            ImGui::TableSetupColumn("Weight");
            ImGui::TableSetupColumn("Action");
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < availableItems.size(); i++)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", availableItems[i].name().c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", itemIcons[i].c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.1f", availableItems[i].weight());

                ImGui::TableSetColumnIndex(3);
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

    io.Fonts->AddFontDefault();
    float baseFontSize = 13.0f; // 13.0f is the size of the default font. Change to the font size you use.
    float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = iconFontSize;
    io.Fonts->AddFontFromFileTTF( "fa-solid-900.ttf", iconFontSize, &icons_config, icons_ranges );

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
