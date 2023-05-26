#include "includes.h"
#include "includes.h"
#include <chrono>
#include <thread>
#include <random>

template<typename Base, typename T>
bool instanceof(const T ptr) {
    return dynamic_cast<const Base>(ptr) != nullptr;
}

template<typename O, typename T>
T getFromObjectPos(CCObject* obj, CCPoint point) {
    CCObject* pObj = nullptr;

    CCARRAY_FOREACH(dynamic_cast<O>(obj)->getChildren(), pObj) {
        CCNode* currentNode = (CCNode*)pObj;
        CCPoint xy = currentNode->getPosition();

        T obj = dynamic_cast<T>(pObj);

        if (instanceof<T>(obj)) {
            if (point.x == xy.x && point.y == xy.y) {
                return obj;
            }
        }
    }
    return nullptr;
}

template<typename O, typename T>
T getFromObjectContentSizeWidth(CCObject* obj, float width) {
    CCObject* pObj = nullptr;

    CCARRAY_FOREACH(dynamic_cast<O>(obj)->getChildren(), pObj) {
        CCNode* currentNode = (CCNode*)pObj;
        float objWidth = currentNode->getContentSize().width;

        T obj = dynamic_cast<T>(pObj);

        if (instanceof<T>(obj)) {
            if (objWidth == width) {
                return obj;
            }
        }
    }
    return nullptr;
}

template<typename O, typename T>
T getFromObjectIndex(CCObject* obj, int index) {
    CCObject* pObj = nullptr;

    int idx = 0;

    CCARRAY_FOREACH(dynamic_cast<O>(obj)->getChildren(), pObj) {
        CCNode* currentNode = (CCNode*)pObj;

        T obj = dynamic_cast<T>(pObj);

        if (instanceof<T>(obj) ) {
            if (idx == index) return obj;
            idx++;
        }
    }
    return nullptr;
}

template<typename O, typename T>
T getFromObjectColor(CCObject* obj, ccColor3B& color) {
    CCObject* pObj = nullptr;

    CCARRAY_FOREACH(dynamic_cast<O>(obj)->getChildren(), pObj) {
        CCNode* currentNode = (CCNode*)pObj;

        T obj = dynamic_cast<T>(pObj);

        if (instanceof<T>(obj)) {
            if (obj->getColor().r == color.r &&
                obj->getColor().g == color.g &&
                obj->getColor().b == color.b) {
                return obj;
            }
        }
    }
    return nullptr;
}

std::string get_module_name(HMODULE mod) {
    char buffer[MAX_PATH];
    if (!mod || !GetModuleFileNameA(mod, buffer, MAX_PATH))
        return "Unknown";
    return std::filesystem::path(buffer).filename().string();
}

int getAddr(void* addr) {
    HMODULE mod;

    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<char*>(addr), &mod))
        mod = NULL;
    unsigned int x;
    std::stringstream stream;
    stream << std::hex << reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr) - reinterpret_cast<uintptr_t>(mod));
    stream >> x;

    return static_cast<int>(x);
}

std::string getDllName(void* addr) {
    HMODULE mod;

    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<char*>(addr), &mod))
        mod = NULL;

    std::stringstream stream;
    stream << get_module_name(mod);
    return stream.str();
}


#define public_cast(value, member) [](auto* v) { \
	class FriendClass__; \
	using T = std::remove_pointer<decltype(v)>::type; \
	class FriendeeClass__: public T { \
	protected: \
		friend FriendClass__; \
	}; \
	class FriendClass__ { \
	public: \
		auto& get(FriendeeClass__* v) { return v->member; } \
	} c; \
	return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

template <typename T, typename U>
T union_cast(U value) {
    union {
        T a;
        U b;
    } u;
    u.b = value;
    return u.a;
}

bool(__thiscall* CCMenuItemSpriteExtra_init)(gd::CCMenuItemSpriteExtra*, cocos2d::CCNode*);

bool __fastcall CCMenuItemSpriteExtra_init_H(gd::CCMenuItemSpriteExtra* self, void*, cocos2d::CCNode* a){

    if (!CCMenuItemSpriteExtra_init(self, a)) return false;

    auto menuItemNode = dynamic_cast<CCMenuItem*>(self);

    const auto selector = public_cast(menuItemNode, m_pfnSelector);
    const auto name = getDllName(union_cast<void*>(selector));
    const auto addr = getAddr(union_cast<void*>(selector));

    if (name == "hackpro.dll") {

        switch (addr) {
        case 0x0A04B0:
        case 0x0A12F0:
        case 0x0A1330:
        case 0x0A13C0:
        case 0x132880:
        case 0x1328A0:
            return true;
        }

        self->setVisible(false);
    }

    return true;
}

bool isCustomLevel = false;

bool(__thiscall* CCNode_setVisible)(CCNode*, bool);

bool __fastcall CCNode_setVisible_H(CCNode* self, void*, bool a) {

    if (!CCNode_setVisible(self, a)) return false;

    if (instanceof<CCMenuItem*>(self)) {
        auto menuItemNode = dynamic_cast<CCMenuItem*>(self);

        const auto selector = public_cast(menuItemNode, m_pfnSelector);
        const auto name = getDllName(union_cast<void*>(selector));
        const auto addr = getAddr(union_cast<void*>(selector));

        if (name == "hackpro.dll") {

            switch (addr) {
            case 0x0A04B0:
            case 0x0A12F0:
            case 0x0A1330:
            case 0x0A13C0:
            case 0x132880:
            case 0x1328A0:
                return true;
            }

            self->setVisible(false);
        }

        if (name == "GeometryDash.exe") {
            switch (addr)
            {
            case 0x0697B0:
                if(isCustomLevel) self->setVisible(false);
            }
        }

    }
    return true;
}

bool(__thiscall* LevelSearchLayer_init)(gd::CCMenuItemSpriteExtra*);

bool __fastcall LevelSearchLayer_init_H(gd::CCMenuItemSpriteExtra* self, void*) {

    if (!LevelSearchLayer_init(self)) return false;

    CCLabelBMFont* qSearch = getFromObjectIndex<CCLayer*, CCLabelBMFont*>(self, 0);
    qSearch->setVisible(true);

    CCLabelBMFont* filters = getFromObjectIndex<CCLayer*, CCLabelBMFont*>(self, 1);
    filters->setVisible(true);

    cocos2d::extension::CCScale9Sprite* buttonBG = getFromObjectColor<CCLayer*, cocos2d::extension::CCScale9Sprite*>(self, ccColor3B{0, 46, 117});
    buttonBG->setContentSize({ 365, 115 });

    CCMenu* buttonMenu = getFromObjectIndex<CCLayer*, CCMenu*>(self, 2);

    buttonMenu->setPosition(buttonMenu->getPositionX(), buttonMenu->getPositionY() + 18);

    return true;
}


bool(__thiscall* CustomSongWidget_init)(gd::CustomSongWidget*, gd::SongInfoObject*, gd::LevelSettingsObject*, bool, bool, bool, bool, bool);

bool __fastcall CustomSongWidget_init_H(gd::CustomSongWidget* self, void*, gd::SongInfoObject* a, gd::LevelSettingsObject* b, bool c, bool d, bool e, bool f, bool g) {

    if (e && !d) {
        isCustomLevel = true;
    }
    else {
        isCustomLevel = false;
    }

    if (!CustomSongWidget_init(self, a, b, c, d, e, f, g)) return false;

    return true;

}

void(__thiscall* CustomSongWidget_updateSongInfo)(gd::CustomSongWidget*);

void __fastcall CustomSongWidget_updateSongInfo_H(gd::CustomSongWidget* self, void*) {
    
    CustomSongWidget_updateSongInfo(self);

    CCMenu* menu = getFromObjectIndex<CCNode*, CCMenu*>(self, 0);

    menu->getChildByTag(1337)->setVisible(false);

    CCLabelBMFont* artistLabel = getFromObjectIndex<CCNode*, CCLabelBMFont*>(self, 1);
    artistLabel->limitLabelWidth(175, 0.8, 0.1);

    CCMenuItem* moreButton = getFromObjectIndex<CCMenu*, CCMenuItem*>(menu, 5);

    moreButton->setPositionX(artistLabel->getScaledContentSize().width - 384);

}

bool(__thiscall* PauseLayer_customSetup)(gd::PauseLayer* self);

bool __fastcall PauseLayer_customSetup_H(gd::PauseLayer* self, void*) {

    if(!PauseLayer_customSetup(self)) return false;

    CCMenu* menu = getFromObjectIndex<CCLayer*, CCMenu*>(self, 1);

    CCMenuItem* percentToggler = getFromObjectIndex<CCMenu*, CCMenuItem*>(menu, 3);

    percentToggler->setVisible(false);

    CCLabelBMFont* progressBarText = getFromObjectIndex<CCLayer*, CCLabelBMFont*>(self, 7);

    progressBarText->setString("Progress Bar");

    CCLabelBMFont* percentText = getFromObjectIndex<CCLayer*, CCLabelBMFont*>(self, 10);

    percentText->setVisible(false);


    return true;
}

CCNode*(__thiscall* EndLevelLayer_create)(CCNode* self);

CCNode* __fastcall EndLevelLayer_create_H(CCNode* self, void*) {

    CCNode* ret = EndLevelLayer_create(self);    

    CCLayer* layer = getFromObjectIndex<CCNode*, CCLayer*>(ret, 0);

    CCMenu* menu = getFromObjectIndex<CCLayer*, CCMenu*>(layer, 1);

    CCNode* editButton = getFromObjectIndex<CCMenu*, CCNode*>(menu, 2);

    editButton->setPosition({ 0, editButton->getPositionY() });

    CCLabelBMFont* extraText = getFromObjectIndex<CCLayer*, CCLabelBMFont*>(layer, 3);
    CCLabelBMFont* extraText2 = getFromObjectIndex<CCLayer*, CCLabelBMFont*>(layer, 4);

    if (extraText != nullptr) {

        std::string text = extraText->getString();

        if (text.rfind("Checkpoint", 0) == 0 || text.rfind("StartPos", 0) == 0) {
            extraText->setVisible(false);
        }
    }
    if (extraText2 != nullptr) {

        std::string text = extraText2->getString();

        if (text.rfind("Checkpoint", 0) == 0 || text.rfind("StartPos", 0) == 0) {
            extraText2->setVisible(false);
        }
    }

    CCNode* textArea = getFromObjectContentSizeWidth<CCLayer*, CCNode*>(layer, 2480);

    gd::PlayLayer* playLayer = gd::PlayLayer::get();

    bool isTestMode = playLayer->m_isTestMode;
    bool isPracticeMode = playLayer->m_isPracticeMode;
    bool isNormal = true;

    if (isTestMode || isPracticeMode) isNormal = false;

    if (!(isTestMode && isPracticeMode)) {

        gd::TextArea* area = dynamic_cast<gd::TextArea*>(textArea);

        if (textArea != nullptr) {
            if (isPracticeMode || isTestMode) {
                textArea->setPosition(textArea->getPositionX(), textArea->getPositionY() + 8);
            }
        }
    }


    CCLabelBMFont* attText = getFromObjectIndex<CCLayer*, CCLabelBMFont*>(layer, 0);
    CCLabelBMFont* jumpText = getFromObjectIndex<CCLayer*, CCLabelBMFont*>(layer, 1);
    CCLabelBMFont* timeText = getFromObjectIndex<CCLayer*, CCLabelBMFont*>(layer, 2);


    if (!isNormal) {
        attText->setPosition({ attText->getPositionX() + 0.5f, attText->getPositionY() - 8 });
        jumpText->setPosition({ jumpText->getPositionX() + 0.5f, jumpText->getPositionY() - 8 });
        timeText->setPosition({ timeText->getPositionX() + 0.5f, timeText->getPositionY() - 8 });
    }

    CCNode* restartButton = getFromObjectIndex<CCMenu*, CCNode*>(menu, 0);
    CCNode* menuButton = getFromObjectIndex<CCMenu*, CCNode*>(menu, 3);

    if (isPracticeMode) {
        restartButton->setPosition({ restartButton->getPositionX() + 25, restartButton->getPositionY() });
        menuButton->setPosition({ menuButton->getPositionX() - 25, menuButton->getPositionY() });
    }

    if (isTestMode && isPracticeMode) {
        attText->setPosition({ attText->getPositionX() + 80, attText->getPositionY() + 8});
        jumpText->setPosition({ jumpText->getPositionX() - 80, jumpText->getPositionY() - 16});
        timeText->setPosition({ timeText->getPositionX() + 80, timeText->getPositionY() - 16});
    }

    return ret;
}

DWORD WINAPI thread_func(void* hModule) {
    MH_Initialize();

    /*AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);*/

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(100, 2000);

    int random = distr(gen);

    std::this_thread::sleep_for(std::chrono::milliseconds(random));

    auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

    auto addr = GetProcAddress(GetModuleHandleA("libcocos2d.dll"), "?setVisible@CCNode@cocos2d@@UAEX_N@Z");
    MH_CreateHook(
        reinterpret_cast<void*>(addr),
        CCNode_setVisible_H,
        reinterpret_cast<void**>(&CCNode_setVisible)
    );

    MH_CreateHook(
        reinterpret_cast<void*>(base + 0x18FA0),
        CCMenuItemSpriteExtra_init_H,
        reinterpret_cast<void**>(&CCMenuItemSpriteExtra_init)
    );

    MH_CreateHook(
        reinterpret_cast<void*>(base + 0x17DA60),
        LevelSearchLayer_init_H,
        reinterpret_cast<void**>(&LevelSearchLayer_init)
    );

    MH_CreateHook(
        reinterpret_cast<void*>(base + 0x685B0),
        CustomSongWidget_init_H,
        reinterpret_cast<void**>(&CustomSongWidget_init)
    );

    MH_CreateHook(
        reinterpret_cast<void*>(base + 0x69BF0),
        CustomSongWidget_updateSongInfo_H,
        reinterpret_cast<void**>(&CustomSongWidget_updateSongInfo)
    );

    MH_CreateHook(
        reinterpret_cast<void*>(base + 0x1e4620),
        PauseLayer_customSetup_H,
        reinterpret_cast<void**>(&PauseLayer_customSetup)
    );

    MH_CreateHook(
        reinterpret_cast<void*>(base + 0x94B50),
        EndLevelLayer_create_H,
        reinterpret_cast<void**>(&EndLevelLayer_create)
    );
    MH_EnableHook(MH_ALL_HOOKS);
    
    return 0;
}



BOOL APIENTRY DllMain(HMODULE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(0, 0x100, thread_func, handle, 0, 0);
        
    }
    return TRUE;
}