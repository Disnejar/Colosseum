#include "App.h"

class TestLayer : public Colosseum::Layer
{
    void DrawLayer() override
    {
        float f;

        ImGui::Begin("TestLayer");
        ImGui::DragFloat2("HAHAFLOAT", &f);

        ImGui::End();
    }
};

int main(){
    Colosseum::App::AddLayer(std::make_shared<TestLayer>());
    Colosseum::App::Run();
}
