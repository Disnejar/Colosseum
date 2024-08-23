#include "App.h"
#include "Layer.h"

#ifdef COLOSSEUM_DEBUG_MODE
#define APP_USE_VULKAN_DEBUG_REPORT
#endif

int main()
{
    Colosseum::App::Initialise();
    std::shared_ptr<Colosseum::MainLayer> mainLayer = std::make_shared<Colosseum::MainLayer>();
    Colosseum::App::AddLayer(mainLayer);
    Colosseum::App::Run();
    Colosseum::App::Cleanup();
}
