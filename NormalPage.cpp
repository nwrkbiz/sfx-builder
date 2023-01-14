#include <NormalPage.h>

#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Flow.H>
#include <FL/Fl_Text_Editor.H>

#include <dflt_banner.h>

#include <Base64.h>

NormalPage::~NormalPage() {

}

static std::unique_ptr<Fl_RGB_Image> s_default_banner = std::make_unique<Fl_RGB_Image>(dflt_banner_png, 135, 450, 4, 0);

void NormalPage::SetPage(const giri::json::JSON& p) {
        m_Page = p;
}

void NormalPage::Draw() {
        auto g = std::make_unique<Fl_Group>(10,10, 650, 460);
        auto f = std::make_unique<Fl_Flow>(10,10, 650, 460);

        // image
        auto img = std::make_unique<Fl_Box>(0,0,135,450);

        if(m_Page.hasKey("image") && !m_Page["image"].ToString().empty()){
                std::vector<char> t = giri::Base64::Decode(m_Page["image"].ToString());
                m_Image = std::make_unique<Fl_PNG_Image>(nullptr, (uchar*)t.data(), t.size());
                img->image(m_Image.get());
        } else {
                img->image(s_default_banner.get());
        }
        f->rule(img.get(), "<^");
        m_Widgets.push_back(std::move(img));

        // spacer left after image
        auto sep_left = std::make_unique<Fl_Box>(0, 0, 10, 0);
        f->rule(sep_left.get(), "=^<");
        m_Widgets.push_back(std::move(sep_left));

        // headline
        auto headLine = std::make_unique<Fl_Box>(0, 0, 50, 50);
        m_Title = m_Page["title"].ToString();
        headLine->label(m_Title.c_str());
        headLine->labelsize(30);
        headLine->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
        f->rule(headLine.get(), "^=<");
        m_Widgets.push_back(std::move(headLine));

        // textarea
        m_Text = std::make_unique<Fl_Text_Display>(0, 0, 0, 340);
        m_Text->box(FL_FLAT_BOX);
        m_TextBuf->text(m_Page["text"].ToUnescapedString().c_str());
        m_Text->buffer(m_TextBuf.get());
        m_Text->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
        f->rule(m_Text.get(), "=<^");

        // progressbar
        m_Progress = std::make_unique<Fl_Progress>(0,0, 1, 15);
        f->rule(m_Progress.get(), "^=<");
        m_Progress->color(FL_WHITE, FL_BLUE);
        m_Progress->maximum(100);

        // checkbox
        if(m_Page["checkbox"].ToBool()) {
                m_ChbText = m_Page["checkbox_text"].ToString();
                m_Chk = std::make_unique<Fl_Check_Button>(0,0,100,25, m_ChbText.c_str());
                m_Chk->value(false);
                f->rule(m_Chk.get(), "<^");
        }

        // prev btn
        if(m_HasPrev) {
                m_PrevText = m_Page["prev_text"].ToString();
                m_Prv = std::make_unique<Fl_Button>(0,0,100,25, m_PrevText.c_str()); 
                m_Prv->callback(back_cb, this);
                f->rule(m_Prv.get(), "<^");
        }

        // next btn
        m_NxtText = m_Page["next_text"].ToString();
        m_Nxt =std::make_unique<Fl_Button>(0,0,100,25, m_NxtText.c_str()); 
        m_Nxt->callback(next_cb, this);
        
        if(m_Page["checkbox"].ToBool()) {
                m_Nxt->deactivate();
                m_Chk->callback(chk_cb, m_Nxt.get());
        }

        f->rule(m_Nxt.get(), "<^");
        f->rule(m_Nxt.get(), ">");

        if(m_HasPrev){
                f->rule(m_Prv.get(), ">");
        }

        f->end();
        g->end();

        m_Widgets.push_back(std::move(f));
        m_Widgets.push_back(std::move(g));
}

void NormalPage::SetInstallCb(std::function<void(NormalPage*)> f) {
        m_Fun = f;
        m_Nxt->callback(inst_cb, this);
}

void NormalPage::chk_cb(Fl_Widget* chk,void* btn) { 
        if(static_cast<Fl_Check_Button*>(chk)->value()){
                static_cast<Fl_Button*>(btn)->activate();
        } else {
                static_cast<Fl_Button*>(btn)->deactivate();
        } 
}

void NormalPage::back_cb(Fl_Widget*,void* me) { 
        static_cast<NormalPage*>(me)->m_Wiz->prev(); 
}

void NormalPage::next_cb(Fl_Widget*,void* me) { 
        static_cast<NormalPage*>(me)->m_Wiz->next(); 
}

void NormalPage::inst_cb(Fl_Widget*,void* me) { 
        auto t = static_cast<NormalPage*>(me);
        t->m_Nxt->deactivate();
        if(t->m_HasPrev) {
                t->m_Prv->deactivate();
        }
        if(t->m_Page["checkbox"].ToBool()) {
                t->m_Chk->deactivate();
        }
        t->m_Fun(t); 
}

void NormalPage::AddText(const std::string& s) {
        m_TextBuf->insert(0, s.c_str());
        m_Text->redraw();
}

void NormalPage::SetPercentage(float p) {
        m_Progress->value(p);
        m_Progress->redraw();
}