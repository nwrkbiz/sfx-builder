#ifndef VIEW_H
#define VIEW_H
#include <memory>
#include <string>
#include <vector>

#include <Object.h>
#include <JSON.h>
#include <Base64.h>

#include <FL/Fl_Window.H>
#include <FL/Fl_Wizard.H>
#include <FL/Fl_PNG_Image.H>

#include <NormalPage.h>
#include <Icon.h>

#ifdef _WIN32
#include <windows.h>
#endif

void showErrorMsg(const std::string& message);

class View : public giri::Object<View> {

public:
    View(const giri::json::JSON& v) : m_View(v) {
#ifdef _WIN32
        FreeConsole();
#endif
        m_Title = m_View["title"].ToString();

        m_Window = std::make_unique<Fl_Window>(670,480, m_Title.c_str());
        m_Wizard = std::make_shared<Fl_Wizard>(0,0,670,480);

        if(m_View.hasKey("icon") && !m_View["icon"].ToString().empty()){
                std::vector<char> t = giri::Base64::Decode(m_View["icon"].ToString());
                Fl_PNG_Image i(nullptr, (uchar*)t.data(), t.size());
                m_Window->icon(&i);
        } else {
                Fl_RGB_Image i(Icon_png, 54, 54, 4, 0);
                m_Window->icon(&i);
        }
    };

    ~View();

    void Draw();

    void SetInstallCb(std::function<void(NormalPage*)> f);

private:
    std::string m_Title = "";

    std::unique_ptr<Fl_Window> m_Window;
    std::shared_ptr<Fl_Wizard> m_Wizard;

    giri::json::JSON m_View;
    std::vector<NormalPage::UPtr> m_Pages;
};

#endif // VIEW_H
