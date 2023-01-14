#ifndef NORMAL_PAGE_H
#define NORMAL_PAGE_H
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <JSON.h>
#include <Object.h>

#include <FL/Fl_Widget.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Wizard.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Text_Display.H>

class NormalPage : public giri::Object<NormalPage>
{
public:
    NormalPage(const giri::json::JSON& p, bool hasPrv, const std::shared_ptr<Fl_Wizard>& w) : m_Page(p), m_HasPrev(hasPrv), m_Wiz(w) {
        m_TextBuf = std::make_unique<Fl_Text_Buffer>();
    };
    ~NormalPage();
    virtual void Draw();

    void SetPage(const giri::json::JSON& p);
    void AddText(const std::string& s);
    void SetPercentage(float p);
    void SetInstallCb(std::function<void(NormalPage*)> f);
private:
    static void back_cb(Fl_Widget*,void*);
    static void next_cb(Fl_Widget*,void*);
    static void chk_cb(Fl_Widget*,void*);
    static void inst_cb(Fl_Widget*,void*);

    giri::json::JSON m_Page;
    bool m_HasPrev = false;
    std::string m_Title = "";
    std::string m_ChbText = "";
    std::string m_PrevText = "";
    std::string m_NxtText = "";
    std::shared_ptr<Fl_Wizard> m_Wiz;
    std::unique_ptr<Fl_Text_Buffer> m_TextBuf;
    std::vector<std::unique_ptr<Fl_Widget>> m_Widgets;
    std::unique_ptr<Fl_Button> m_Nxt;
    std::unique_ptr<Fl_Button> m_Prv;
    std::unique_ptr<Fl_Check_Button> m_Chk;
    std::unique_ptr<Fl_Progress> m_Progress;
    std::unique_ptr<Fl_PNG_Image> m_Image;
    std::unique_ptr<Fl_Text_Display> m_Text;
    std::function<void(NormalPage*)> m_Fun;
};
#endif // NORMAL_PAGE_H