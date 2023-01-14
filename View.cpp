#include <iostream>

#include <View.h>

#include <FL/fl_ask.H>

void showErrorMsg(const std::string& message) {
    std::cerr << message << std::endl;
    std::string printMsg = message;
    fl_message_title("Error!");
    printMsg.append("\n\nExit application?");
    switch ( fl_choice(printMsg.c_str(), "Yes", "No", 0) ) {
      case 0: exit(EXIT_FAILURE); break;
    }
}

View::~View() {

}

void View::Draw() {

    for(auto & i : m_View["pages"].ArrayRange()) {
        auto p = std::make_unique<NormalPage>(i, m_Pages.size() > 0, m_Wizard);
        p->Draw();
        if(i.hasKey("percentage"))
            p->SetPercentage(i["percentage"].ToFloat());
        m_Pages.push_back(std::move(p));
    }

    m_Window->show();
}

void View::SetInstallCb(std::function<void(NormalPage*)> f) {
    m_Pages[m_Pages.size()-1]->SetInstallCb(f);
}