#include "global_basic.hpp"
#include "global_game.hpp"
#include "errors.hpp"
#include "property_helper.hpp"
#include "xml_attributes.hpp"
#include "../scripting/scriptable_object.hpp"
#include "../objects/actor.hpp"
#include "../scenes/scene.hpp"
#include "../scenes/menu_scene.hpp"
#include "scene_manager.hpp"
#include "filesystem/resource_manager.hpp"
#include "../video/img_manager.hpp"
#include "filesystem/package_manager.hpp"
#include "i18n.hpp"
#include "../user/preferences.hpp"
#include "tsc_app.hpp"

TSC::cApp* TSC::gp_app = NULL;

using namespace TSC;

cApp::cApp()
{
    debug_print("Initializing application.\n");

    // init random number generator
    srand(static_cast<unsigned int>(time(NULL)));

    Init_Managers();
    Init_User_Preferences();
    Init_SFML();
    Init_I18N();
    Init_CEGUI();
}

cApp::~cApp()
{
    delete mp_scene_manager;
    delete mp_package_manager;
    delete mp_resource_manager;
    delete mp_preferences;
    delete mp_renderwindow;
}

void cApp::Init_SFML()
{
    debug_print("Initializing SFML.\n");
    mp_renderwindow = new sf::RenderWindow(sf::VideoMode(mp_preferences->m_video_screen_w, mp_preferences->m_video_screen_h), CAPTION);
}

void cApp::Init_Managers()
{
    debug_print("Initializing manager classes.\n");

    mp_resource_manager = new cResource_Manager();
    mp_resource_manager->Init_User_Directory();

    mp_package_manager = new cPackage_Manager(*mp_resource_manager);
    mp_scene_manager = new cSceneManager();
}

void cApp::Init_User_Preferences()
{
    mp_preferences = cPreferences::Load_From_File(mp_resource_manager->Get_Preferences_File());
    debug_print("Configuration file is '%s'.\n", path_to_utf8(mp_preferences->m_config_filename).c_str());

    // mp_preferences->Apply();
}

void cApp::Init_I18N()
{
    debug_print("Initializing I18n.\n");

    // set game language
    I18N_Set_Language(mp_preferences->m_language);
    // init translation support
    I18N_Init(mp_resource_manager->Get_Game_Translation_Directory());
}

void cApp::Init_CEGUI()
{
    debug_print("Initializing CEGUI.\n");

    mp_cegui_renderer = &CEGUI::OpenGLRenderer::create(CEGUI::Size(mp_preferences->m_video_screen_w, mp_preferences->m_video_screen_h));
    mp_cegui_renderer->enableExtraStateSettings(true);

    std::string schemes_path    = path_to_utf8(mp_resource_manager->Get_Gui_Scheme_Directory());
    std::string imagesets_path  = path_to_utf8(mp_resource_manager->Get_Gui_Imageset_Directory());
    std::string fonts_path      = path_to_utf8(mp_resource_manager->Get_Gui_Font_Directory());
    std::string looknfeels_path = path_to_utf8(mp_resource_manager->Get_Gui_LookNFeel_Directory());
    std::string layouts_path    = path_to_utf8(mp_resource_manager->Get_Gui_Layout_Directory());

    std::cout << "CEGUI schemes path:    " << schemes_path << std::endl;
    std::cout << "CEGUI imagesets path:  " << imagesets_path << std::endl;
    std::cout << "CEGUI fonts path:      " << fonts_path << std::endl;
    std::cout << "CEGUI looknfeels path: " << looknfeels_path << std::endl;
    std::cout << "CEGUI layouts path:    " << layouts_path << std::endl;

    CEGUI::DefaultResourceProvider* p_provider = new CEGUI::DefaultResourceProvider();
    p_provider->setResourceGroupDirectory("schemes", schemes_path);
    p_provider->setResourceGroupDirectory("imagesets", imagesets_path);
    p_provider->setResourceGroupDirectory("fonts", fonts_path);
    p_provider->setResourceGroupDirectory("looknfeels", looknfeels_path);
    p_provider->setResourceGroupDirectory("layouts", layouts_path);

    sf::Vector2i mousepos = sf::Mouse::getPosition(*mp_renderwindow);
    CEGUI::MouseCursor::setInitialMousePosition(CEGUI::Point(mousepos.x, mousepos.y));

    // add custom widgets
    // CEGUI::WindowFactoryManager::addFactor<CEGUI::TSC_SpinnerFactory>();
    mp_cegui_system = &CEGUI::System::create(*mp_cegui_renderer, p_provider); // TODO: Logfile

    // Tell CEGUI which groups to use from those we defined above
    CEGUI::Scheme::setDefaultResourceGroup("schemes");
    CEGUI::Imageset::setDefaultResourceGroup("imagesets");
    CEGUI::Font::setDefaultResourceGroup("fonts");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
    CEGUI::WindowManager::setDefaultResourceGroup("layouts");

    // load the scheme file, which auto-loads the imageset
    CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");

    // default mouse cursor
    mp_cegui_system->setDefaultMouseCursor("TaharezLook", "MouseArrow");
    // force new mouse image
    CEGUI::MouseCursor::getSingleton().setImage(&CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage("MouseArrow"));
    // default tooltip
    mp_cegui_system->setDefaultTooltip("TaharezLook/Tooltip");

    // create default root window
    CEGUI::Window* p_rootwindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("default.layout");
    mp_cegui_system->setGUISheet(p_rootwindow);
    p_rootwindow->activate();
}

/**
 * Initiate the main loop.
 */
int cApp::Run()
{
    // Set default package
    mp_package_manager->Set_Current_Package(mp_preferences->m_package);

    // Always start with the start menu scene
    cMenuScene* p_menuscene = new cMenuScene();
    mp_scene_manager->Push_Scene(p_menuscene);

    // Start the main loop
    mp_scene_manager->Play(*mp_renderwindow);

    mp_preferences->Save();

    // TODO: Proper return value
    return 0;
}
