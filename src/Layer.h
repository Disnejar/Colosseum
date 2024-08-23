#pragma once

#include "imgui.h"
#include "Data/Image.h"

namespace Colosseum 
{
    class Layer
    {
    public:
        virtual void DrawLayer() { };
    };

    class MainLayer : public Layer
    {
    private:

    public : 
        void DrawLayer() override
        {
            ImGui::Begin("ImageTest");
            std::shared_ptr<Image> _image = std::make_shared<Image>("/home/disnejar/Downloads/hms_serifikat.png");
            ImGui::Image((ImTextureID)_image->GetDescriptor(), ImVec2{_image->Width, _image->Height});
            ImGui::End();
        }
    };
}