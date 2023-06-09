#include "rent.cpp";


class mainFrame : public wxFrame {
public:
    mainFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)), money(0) {
       
        wxPanel* panel = new wxPanel(this, wxID_ANY);

        
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        int spacingTop = (GetSize().GetHeight() - 340) / 5;  
        vbox->AddSpacer(spacingTop);

        wxStaticText* shipyardName = new wxStaticText(panel, wxID_ANY, "Simon's Shipyard");
        shipyardName->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD)); 
        vbox->Add(shipyardName, 0, wxALIGN_CENTER | wxTOP, 20);  

        vbox->AddSpacer(spacingTop - 20);  

        wxGridSizer* gridSizer = new wxGridSizer(2, 2, 20, 20); 

        std::vector<std::string> buttonLabels = { "Buy", "Sell", "Rent", "Credits" };

        // 1
        wxButton* buyButton = new wxButton(panel, wxID_ANY, wxString::FromUTF8(buttonLabels[0].c_str()));
        buyButton->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD)); 
        buyButton->SetMinSize(wxSize(180, 60)); 

        gridSizer->Add(buyButton, 0, wxALIGN_CENTER | wxALL, 10);

        
        buyButton->Bind(wxEVT_BUTTON, &mainFrame::Buy, this);

        // 2
        wxButton* sellButton = new wxButton(panel, wxID_ANY, wxString::FromUTF8(buttonLabels[1].c_str()));
        sellButton->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD)); 
        sellButton->SetMinSize(wxSize(180, 60)); 

        gridSizer->Add(sellButton, 0, wxALIGN_CENTER | wxALL, 10);

        
        sellButton->Bind(wxEVT_BUTTON, &mainFrame::Sell, this);

        // 3
        wxButton* rentButton = new wxButton(panel, wxID_ANY, wxString::FromUTF8(buttonLabels[2].c_str()));
        rentButton->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD)); 
        rentButton->SetMinSize(wxSize(180, 60)); 

        gridSizer->Add(rentButton, 0, wxALIGN_CENTER | wxALL, 10);
        rentButton->Bind(wxEVT_BUTTON, &mainFrame::Rent, this);

        // 4
        wxButton* creditsButton = new wxButton(panel, wxID_ANY, wxString::FromUTF8(buttonLabels[3].c_str()));
        creditsButton->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD)); 
        creditsButton->SetMinSize(wxSize(180, 60)); 

        gridSizer->Add(creditsButton, 0, wxALIGN_CENTER | wxALL, 10);
        creditsButton->Bind(wxEVT_BUTTON, &mainFrame::Credits, this);   
        vbox->Add(gridSizer, 1, wxALIGN_CENTER);  


        int spacingBottom = (GetSize().GetHeight() - 340) / 5; 
        vbox->AddSpacer(spacingBottom);

        panel->SetSizer(vbox);
    }

    void Buy(wxCommandEvent& event) {
        buyFrame* buyFrame1 = new buyFrame("Buy", money);
        buyFrame1->Show(true);
    }
    void Sell(wxCommandEvent& event) {
        sellFrame* sellFrame1 = new sellFrame("Sell", money);
        sellFrame1->Show(true);
    }
    void Rent(wxCommandEvent& event) {
        rentFrame* rentFrame1 = new rentFrame("Rent", money);
        rentFrame1->Show(true);
    }
    void Credits(wxCommandEvent& event) {
        creditsFrame* creditsFrame1 = new creditsFrame("Credits", money);
        creditsFrame1->Show(true);
    }

private:
    int money;
};

class Morphism { // PRZYKLAD POLYMORPHISMU
public:
    void polymorphism() {
        std::cout << "This is an example of polymorphism" << std::endl;
    }
};

class Poly : public Morphism { // PRZYKLAD POLYMORPHISMU
public:
    void polymorphism() {
        std::cout << "Test" << std::endl;
    }
};

class Klasa { // KLASA WEWNETRZNA
public:
    class zagniezdzona {
    public:
        void func() {
            std::cout << "Nested class example" << std::endl;
        }
    };
};

class Abstract : public wxApp { // KLASA ABSTRAKCYJNA
public:
    virtual bool OnInit() = 0; 
};

class MyApp : public Abstract {
public:
    bool OnInit() {
        mainFrame* frame = new mainFrame("Shipyard App");
        frame->Show(true);
        return true;
    }
};

Poly* y = new Poly();
Morphism* x = new Morphism();

wxIMPLEMENT_APP(MyApp);