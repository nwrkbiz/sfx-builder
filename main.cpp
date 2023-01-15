#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <optional>
#include <string>
#include <cstdlib>

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Double_Window.H>

#include <boost/program_options.hpp>


#include <Zip.h>
#include <View.h>
#include <NormalPage.h>
#include <PrivEscalate.h>
#include <Icon.h>

std::optional<boost::program_options::variables_map> parseCmdLine(int argc, char** argv){
    // commandline options
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "Print help.")
            ("zip", boost::program_options::value<std::string>(), "Path to zip file for packing.")
            ("sfx", boost::program_options::value<std::string>(), "Path to SFX file for packing. (defaults to this executable)")
            ("outfile", boost::program_options::value<std::string>(), "Output file for packing. (defaults to ./Install.exe or ./Install.run)")
            ("silent", boost::program_options::value<bool>(), "Silent install");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return {};
    }
    return vm;
}


struct r_val {
    std::filesystem::path sfx;
    std::filesystem::path zip;
    std::filesystem::path outfile;
    bool pack;
    bool silent;
};

r_val getFromCmdLine(const boost::program_options::variables_map& vm, int argc, char** argv){
    std::filesystem::path sfx = argv[0];
    std::filesystem::path zip = "";
#ifdef _WIN32
    std::filesystem::path outfile = "Installer.exe";
#else
    std::filesystem::path outfile = "Installer.run";
#endif
    bool pack = false;
    bool silent = false;

    if(vm.count("zip")){
        pack = true;
        zip = vm["zip"].as<std::string>();
    }

    if(vm.count("sfx")){
        sfx = vm["sfx"].as<std::string>();
    }

    if(vm.count("outfile")){
        outfile = vm["outfile"].as<std::string>();
    }

    if(vm.count("silent")){
        silent = vm["silent"].as<bool>();
    }

    return {sfx, zip, outfile, pack, silent};
}

int main(int argc, char **argv) {

    auto vm_b = parseCmdLine(argc, argv);
    if(vm_b == std::nullopt){
        return EXIT_SUCCESS;
    }
    auto vm = vm_b.value();
    auto config = getFromCmdLine(vm, argc, argv);

    if(config.pack) {
        std::ofstream outFile(config.outfile, std::ios::ate | std::ios::binary);
        std::ifstream sfx(config.sfx);
        std::ifstream zip(config.zip);

        // installer file
        std::copy( 
            (std::istreambuf_iterator<char>(sfx)),
            std::istreambuf_iterator<char>(),
            std::ostreambuf_iterator<char>(outFile)
        );
        // zip archive
        std::copy( 
            (std::istreambuf_iterator<char>(zip)),
            std::istreambuf_iterator<char>(),
            std::ostreambuf_iterator<char>(outFile)
        );

        // size of installer file for extracting
        std::uintmax_t size = std::filesystem::file_size(config.sfx);
        outFile.write((char*)(&size), sizeof(std::uintmax_t));

        return EXIT_SUCCESS;
    } else {

        std::ifstream me(argv[0], std::ios::ate | std::ios::binary);

        std::uintmax_t size = std::filesystem::file_size(argv[0]);
        me.seekg(size - sizeof(std::uintmax_t), std::ios_base::beg);
        std::uintmax_t offset = 0; 
        me.read(reinterpret_cast<char*>(&offset), sizeof(std::uintmax_t));

        Zip::UPtr z = std::make_unique<Zip>(argv[0]);
        z->seek(offset);

        giri::json::JSON s = giri::json::JSON::Load(z->getTextFileContent(".sfx.manifest.json"));

        // silentinstall
        if(config.silent) {
            z->setProgressCb([&](const std::string& f, double pct){
                std::ostringstream ss;
                ss << std::fixed;
                ss << std::setprecision(2);
                ss << pct;
                std::cout << f + " ("+ ss.str() +"%)" << std::endl;
            });
            
            if(s.hasKey("pre_install") && !s["pre_install"].ToUnescapedString().empty()) {
                    system(s["pre_install"].ToUnescapedString().c_str());
            }
            std::cout << "Extract to: " << s["extract_to"].ToString() << std::endl;
            z->extract(s["extract_to"].ToString());
            if(s.hasKey("post_install") && !s["post_install"].ToUnescapedString().empty()) {
                    system(s["post_install"].ToUnescapedString().c_str());
            }
            std::cout << s["finish_text"].ToString() << std::endl;
            return EXIT_SUCCESS;

        // graphical install
        } else {

            if(s["need_root"].ToBool()) {
                if(!isElevated()) {
                    elevatePrivilege(argc, argv);
                    return EXIT_SUCCESS;
                }
            }

            View::UPtr v = std::make_unique<View>(s);
            v->Draw();

            v->SetInstallCb(
                [&](NormalPage* p){
                    auto files = z->getFileList();
                    double num = 0;
                    z->setProgressCb([&](const std::string& f, double pct){
                        std::ostringstream ss;
                        ss << std::fixed;
                        ss << std::setprecision(2);
                        ss << pct;
                        p->AddText(f + " ("+ ss.str() +"%)\n");
                        double val = (pct + num) / files.size();
                        p->SetPercentage(val);
                        if(pct == 100) {
                            num = num + 100;
                        }
                        Fl::check();
                    });

                    if(s.hasKey("pre_install") && !s["pre_install"].ToUnescapedString().empty()) {
                            system(s["pre_install"].ToUnescapedString().c_str());
                    }

                    z->extract(s["extract_to"].ToString());
                    p->SetPercentage(100);

                    if(s.hasKey("icon") && !s["icon"].ToString().empty()){
                            std::vector<char> t = giri::Base64::Decode(s["icon"].ToString());
                            Fl_PNG_Image i(nullptr, (uchar*)t.data(), t.size());
                            ((Fl_Double_Window*)fl_message_icon()->parent())->icon(&i);
                    } else {
                            Fl_RGB_Image i(Icon_png, 54, 54, 4, 0);
                            ((Fl_Double_Window*)fl_message_icon()->parent())->icon(&i);
                    }

                    if(s.hasKey("post_install") && !s["post_install"].ToUnescapedString().empty()) {
                            system(s["post_install"].ToUnescapedString().c_str());
                    }

                    fl_message_title(s["finish_title"].ToString().c_str());
                    fl_message(s["finish_text"].ToString().c_str());
                    exit(EXIT_SUCCESS);
                }
            );
            return Fl::run();
        }
    }
}
