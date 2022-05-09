#include "SceneFactory.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "EndScene.h" 
#include "BaseScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
    //éüÇÃÉVÅ[ÉìÇê∂ê¨
    BaseScene* newScene = nullptr;
    if (sceneName == "TITLE")
    {
        newScene = new TitleScene();
    } 
    else if (sceneName == "GAME")
    {
        newScene = new GameScene();
    }
    else if (sceneName == "END")
    {
        newScene = new EndScene();
    }

    return newScene;
}