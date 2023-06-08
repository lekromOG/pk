#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/timer.h>

class Boat {
public:
    wxString model;
    wxString manufacturer;
    wxString creationDate;
    wxString price;
    wxString id;
};

class buyFrame : public wxFrame {
public:
    buyFrame(const wxString& title, int& money)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 400)), money(money) {
        
        wxPanel* panel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        listCtrl = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
        listCtrl->InsertColumn(0, "Model");
        listCtrl->InsertColumn(1, "Manufacturer");
        listCtrl->InsertColumn(2, "Creation Date");
        listCtrl->InsertColumn(3, "Price");
        listCtrl->InsertColumn(4, "ID");

        std::vector<Boat> boats = loadDataFromFile("boats.txt");

        for (size_t i = 0; i < boats.size(); i++) {
            const Boat& boat = boats[i];
            long itemIndex = listCtrl->InsertItem(i, boat.model);
            listCtrl->SetItem(itemIndex, 1, boat.manufacturer);
            listCtrl->SetItem(itemIndex, 2, boat.creationDate);
            listCtrl->SetItem(itemIndex, 3, boat.price);
            listCtrl->SetItem(itemIndex, 4, boat.id);
        }

        vbox->Add(listCtrl, 1, wxEXPAND | wxALL, 10);

        wxButton* buyButton = new wxButton(panel, wxID_ANY, "Buy");
        vbox->Add(buyButton, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);

        
        buyButton->Bind(wxEVT_BUTTON, &buyFrame::OnBuyButtonClicked, this);

        panel->SetSizer(vbox);
    }

    void OnBuyButtonClicked(wxCommandEvent& event) {
        long selectedItem = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (selectedItem != -1) {
            wxString priceStr = listCtrl->GetItemText(selectedItem, 3);
            long price = 0;
            if (priceStr.ToLong(&price)) {
                if (price <= money) {
                    
                    wxString model = listCtrl->GetItemText(selectedItem, 0);
                    wxString manufacturer = listCtrl->GetItemText(selectedItem, 1);
                    wxString creationDate = listCtrl->GetItemText(selectedItem, 2);
                    wxString id = listCtrl->GetItemText(selectedItem, 4);

                    money -= price;

                    wxString message = wxString::Format("You have successfully purchased:\nModel: %s\nManufacturer: %s\nCreation Date: %s\nID: %s",
                        model, manufacturer, creationDate, id);
                    wxMessageBox(message, "Purchase Successful", wxOK | wxICON_INFORMATION);

                    listCtrl->DeleteItem(selectedItem);

                    std::vector<Boat> boats = loadDataFromFile("boats.txt");
                    boats.erase(boats.begin() + selectedItem);
                    saveDataToFile2(boats, "boats.txt");

                    for (size_t i = selectedItem; i < boats.size(); i++) {
                        Boat& boat = boats[i];
                        boat.id = wxString::Format("%zu", i + 1);
                    }

                    if (!boatExistsInFile({ model, manufacturer, creationDate, priceStr, id }, "inventory.txt")) {
                        saveDataToFile({ {model, manufacturer, creationDate, priceStr, id} }, "inventory.txt");
                    }

                    wxCommandEvent updateEvent(wxEVT_COMMAND_BUTTON_CLICKED, ID_UPDATE_MONEY_LABEL);
                    wxPostEvent(this, updateEvent);
                }
                else {
                    wxMessageBox("Insufficient credits to buy this boat.", "Purchase Failed", wxOK | wxICON_WARNING);
                }
            }
            else {
                wxMessageBox("Invalid price for the selected boat.", "Purchase Failed", wxOK | wxICON_WARNING);
            }
        }
        else {
            wxMessageBox("Please select a boat to buy.", "No Boat Selected", wxOK | wxICON_WARNING);
        }
    }


    bool boatExistsInFile(const Boat& boat, const wxString& filename) {
        std::vector<Boat> boats = loadDataFromFile(filename);
        for (const auto& existingBoat : boats) {
            if (existingBoat.model == boat.model && existingBoat.manufacturer == boat.manufacturer &&
                existingBoat.creationDate == boat.creationDate && existingBoat.price == boat.price &&
                existingBoat.id == boat.id) {
                return true;
            }
        }
        return false;
    }

    void saveDataToFile(const std::vector<Boat>& boats, const wxString& filename) {
        std::ofstream file(filename.ToStdString(), std::ios::app); 
        if (file.is_open()) {
            for (const auto& boat : boats) {
                file << std::endl << boat.model.ToStdString() << " " << boat.manufacturer.ToStdString() << " "
                    << boat.creationDate.ToStdString() << " " << boat.price.ToStdString() << " " << boat.id.ToStdString();
            }
            file.close();
        }
    }

    void saveDataToFile2(const std::vector<Boat>& boats, const wxString& filename) {
        std::ofstream file(filename.ToStdString(), std::ios::trunc); 
        if (file.is_open()) {
            for (const auto& boat : boats) {
                file << std::endl << boat.model.ToStdString() << " " << boat.manufacturer.ToStdString() << " "
                    << boat.creationDate.ToStdString() << " " << boat.price.ToStdString() << " " << boat.id.ToStdString();
            }
            file.close();
        }
    }

    std::vector<Boat> loadDataFromFile(const wxString& filename) {
        std::vector<Boat> boats;

        std::ifstream file(filename.ToStdString());
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string model, manufacturer, creationDate, price, id;
                if (iss >> model >> manufacturer >> creationDate >> price >> id) {
                    Boat boat;
                    boat.model = wxString::FromUTF8(model.c_str());
                    boat.manufacturer = wxString::FromUTF8(manufacturer.c_str());
                    boat.creationDate = wxString::FromUTF8(creationDate.c_str());
                    boat.price = wxString::FromUTF8(price.c_str());
                    boat.id = wxString::FromUTF8(id.c_str());
                    boats.push_back(boat);
                }
            }
            file.close();
        }

        return boats;
    }

private:
    wxListCtrl* listCtrl;
    int& money;
    const int ID_UPDATE_MONEY_LABEL = wxNewId();
};

class creditsFrame : public wxFrame {
public:
    creditsFrame(const wxString& title, int& money) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(400, 300)), money(money) {
        
        wxPanel* panel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        moneyLabel = new wxStaticText(panel, ID_MONEY_LABEL, wxString::Format("You have $%d", money));
        vbox->Add(moneyLabel, 0, wxALIGN_CENTER | wxTOP, 20);

        wxButton* earnButton = new wxButton(panel, wxID_ANY, "Earn Money");
        vbox->Add(earnButton, 0, wxALIGN_CENTER | wxALL, 10);
        earnButton->Bind(wxEVT_BUTTON, &creditsFrame::EarnMoney, this);

        panel->SetSizer(vbox);
    }

    void EarnMoney(wxCommandEvent& event) {
        money += 100000; 
        moneyLabel->SetLabel(wxString::Format("You have $%d", money));
    }

private:
    int& money;
    wxStaticText* moneyLabel;
    const int ID_MONEY_LABEL = wxNewId();
};

class sellFrame : public wxFrame {
public:
    sellFrame(const wxString& title, int& money) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 400)), money(money) {
        
        wxPanel* panel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        listCtrl = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
        listCtrl->InsertColumn(0, "Model");
        listCtrl->InsertColumn(1, "Manufacturer");
        listCtrl->InsertColumn(2, "Creation Date");
        listCtrl->InsertColumn(3, "Price");
        listCtrl->InsertColumn(4, "ID");

        std::vector<Boat> boats = loadDataFromFile("inventory.txt");

        for (size_t i = 0; i < boats.size(); i++) {
            const Boat& boat = boats[i];
            long itemIndex = listCtrl->InsertItem(i, boat.model);
            listCtrl->SetItem(itemIndex, 1, boat.manufacturer);
            listCtrl->SetItem(itemIndex, 2, boat.creationDate);
            listCtrl->SetItem(itemIndex, 3, boat.price);
            listCtrl->SetItem(itemIndex, 4, boat.id);
        }

        vbox->Add(listCtrl, 1, wxEXPAND | wxALL, 10);

        wxButton* sellButton = new wxButton(panel, wxID_ANY, "Sell");
        vbox->Add(sellButton, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);

        
        sellButton->Bind(wxEVT_BUTTON, &sellFrame::OnSellButtonClicked, this);

        panel->SetSizer(vbox);
    }

    void OnSellButtonClicked(wxCommandEvent& event) {
        long selectedItem = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (selectedItem != -1) {
            wxString priceStr = listCtrl->GetItemText(selectedItem, 3);
            long price = 0;
            if (priceStr.ToLong(&price)) {

                
                wxString model = listCtrl->GetItemText(selectedItem, 0);
                wxString manufacturer = listCtrl->GetItemText(selectedItem, 1);
                wxString creationDate = listCtrl->GetItemText(selectedItem, 2);
                wxString id = listCtrl->GetItemText(selectedItem, 4);

                
                money += price;

                
                wxString message = wxString::Format("You have successfully sold:\nModel: %s\nManufacturer: %s\nCreation Date: %s\nID: %s",
                    model, manufacturer, creationDate, id);
                wxMessageBox(message, "Sale Successful", wxOK | wxICON_INFORMATION);

                
                listCtrl->DeleteItem(selectedItem);

               
                std::vector<Boat> boats = loadDataFromFile("inventory.txt");
                boats.erase(boats.begin() + selectedItem);
                saveDataToFile(boats, "inventory.txt");

                
                for (size_t i = selectedItem; i < boats.size(); i++) {
                    Boat& boat = boats[i];
                    boat.id = wxString::Format("%zu", i + 1); 
                }

   
                std::vector<Boat> allBoats = loadDataFromFile("boats.txt");
                if (!boatExistsInFile({ model, manufacturer, creationDate, priceStr, id }, "boats.txt")) {
                    allBoats.push_back({ model, manufacturer, creationDate, priceStr, id }); 
                }
                saveDataToFile(allBoats, "boats.txt"); 

         
                wxCommandEvent updateEvent(wxEVT_COMMAND_BUTTON_CLICKED, ID_UPDATE_MONEY_LABEL);
                wxPostEvent(this, updateEvent);
            }
            else {
                wxMessageBox("Invalid price for the selected boat.", "Sale Failed", wxOK | wxICON_WARNING);
            }
        }
        else {
            wxMessageBox("Please select a boat to sell.", "No Boat Selected", wxOK | wxICON_WARNING);
        }
    }

    bool boatExistsInFile(const Boat& boat, const wxString& filename) {
        std::vector<Boat> boats = loadDataFromFile(filename);
        for (const auto& existingBoat : boats) {
            if (existingBoat.model == boat.model && existingBoat.manufacturer == boat.manufacturer &&
                existingBoat.creationDate == boat.creationDate && existingBoat.price == boat.price &&
                existingBoat.id == boat.id) {
                return true;
            }
        }
        return false;
    }

    void saveDataToFile(const std::vector<Boat>& boats, const wxString& filename) {
        std::ofstream file(filename.ToStdString(), std::ios::trunc); // Open the file in truncate mode
        if (file.is_open()) {
            for (const auto& boat : boats) {
                file << boat.model.ToStdString() << " " << boat.manufacturer.ToStdString() << " "
                    << boat.creationDate.ToStdString() << " " << boat.price.ToStdString() << " " << boat.id.ToStdString() << std::endl;
            }
            file.close();
        }
    }


    std::vector<Boat> loadDataFromFile(const wxString& filename) {
        std::vector<Boat> boats;

        std::ifstream file(filename.ToStdString());
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string model, manufacturer, creationDate, price, id;
                if (iss >> model >> manufacturer >> creationDate >> price >> id) {
                    Boat boat;
                    boat.model = wxString::FromUTF8(model.c_str());
                    boat.manufacturer = wxString::FromUTF8(manufacturer.c_str());
                    boat.creationDate = wxString::FromUTF8(creationDate.c_str());
                    boat.price = wxString::FromUTF8(price.c_str());
                    boat.id = wxString::FromUTF8(id.c_str());
                    boats.push_back(boat);
                }
            }
            file.close();
        }

        return boats;
    }

private:
    wxListCtrl* listCtrl;
    int& money;
    const int ID_UPDATE_MONEY_LABEL = wxNewId();
};

class rentFrame : public wxFrame {
public:
    rentFrame(const wxString& title, int& money)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 400)), money(money) {
        
        wxPanel* panel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
        wxTimer creditTimer;

        
        listCtrl = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
        listCtrl->InsertColumn(0, "Model");
        listCtrl->InsertColumn(1, "Manufacturer");
        listCtrl->InsertColumn(2, "Creation Date");
        listCtrl->InsertColumn(3, "Price");
        listCtrl->InsertColumn(4, "ID");

        
        std::vector<Boat> boats = loadDataFromFile("boats.txt");

        
        for (size_t i = 0; i < boats.size(); i++) {
            const Boat& boat = boats[i];
            long itemIndex = listCtrl->InsertItem(i, boat.model);
            listCtrl->SetItem(itemIndex, 1, boat.manufacturer);
            listCtrl->SetItem(itemIndex, 2, boat.creationDate);
            listCtrl->SetItem(itemIndex, 3, boat.price);
            listCtrl->SetItem(itemIndex, 4, boat.id);
        }

        vbox->Add(listCtrl, 1, wxEXPAND | wxALL, 10);

        
        wxButton* rentButton = new wxButton(panel, wxID_ANY, "Rent");
        vbox->Add(rentButton, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);

        wxString creditLabel = wxString::Format("Credits: %d", money);
        wxStaticText* creditText = new wxStaticText(panel, wxID_ANY, creditLabel);
        vbox->Add(creditText, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);

       
        rentButton->Bind(wxEVT_BUTTON, &rentFrame::OnRentButtonClicked, this, wxID_ANY, wxID_ANY);

        panel->SetSizer(vbox);
    }


    void OnRentButtonClicked(wxCommandEvent& event) {
        long selectedItem = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (selectedItem != -1) {
            wxString priceStr = listCtrl->GetItemText(selectedItem, 3);
            long price = 0;
            if (priceStr.ToLong(&price)) {
                if (price <= money || price >= money && money != 0) {
                    
                    wxString model = listCtrl->GetItemText(selectedItem, 0);
                    wxString manufacturer = listCtrl->GetItemText(selectedItem, 1);
                    wxString creationDate = listCtrl->GetItemText(selectedItem, 2);
                    wxString id = listCtrl->GetItemText(selectedItem, 4);

                   
                    
                    wxString message = wxString::Format("You have successfully rented:\nModel: %s\nManufacturer: %s\nCreation Date: %s\nID: %s",
                        model, manufacturer, creationDate, id);
                    wxMessageBox(message, "Rent Successful", wxOK | wxICON_INFORMATION);

                    
                    listCtrl->DeleteItem(selectedItem);

                   
                    std::vector<Boat> boats = loadDataFromFile("boats.txt");
                    boats.erase(boats.begin() + selectedItem);
                    saveDataToFile2(boats, "boats.txt");

                   
                    for (size_t i = selectedItem; i < boats.size(); i++) {
                        Boat& boat = boats[i];
                        boat.id = wxString::Format("%zu", i + 1); // Update the ID
                    }

                    
                    if (!boatExistsInFile({ model, manufacturer, creationDate, priceStr, id }, "rented.txt")) {
                        saveDataToFile({ {model, manufacturer, creationDate, priceStr, id} }, "rented.txt");
                    }

                   
                    wxCommandEvent updateEvent(wxEVT_COMMAND_BUTTON_CLICKED, ID_UPDATE_MONEY_LABEL);
                    wxPostEvent(this, updateEvent);
                }
                else {
                    wxMessageBox("Insufficient credits to rent this boat.", "Rent Failed", wxOK | wxICON_WARNING);
                }
            }
            else {
                wxMessageBox("Invalid price for the selected boat.", "Rent Failed", wxOK | wxICON_WARNING);
            }
        }
        else {
            wxMessageBox("Please select a boat to rent.", "No Boat Selected", wxOK | wxICON_WARNING);
        }
    }

    bool boatExistsInFile(const Boat& boat, const wxString& filename) {
        std::vector<Boat> boats = loadDataFromFile(filename);
        for (const auto& existingBoat : boats) {
            if (existingBoat.model == boat.model && existingBoat.manufacturer == boat.manufacturer &&
                existingBoat.creationDate == boat.creationDate && existingBoat.price == boat.price &&
                existingBoat.id == boat.id) {
                return true;
            }
        }
        return false;
    }

    void saveDataToFile(const std::vector<Boat>& boats, const wxString& filename) {
        std::ofstream file(filename.ToStdString(), std::ios::app); // Open the file in truncate mode
        if (file.is_open()) {
            for (const auto& boat : boats) {
                file << boat.model.ToStdString() << " " << boat.manufacturer.ToStdString() << " "
                    << boat.creationDate.ToStdString() << " " << boat.price.ToStdString() << " " << boat.id.ToStdString() << std::endl;
            }
            file.close();
        }
    }

    void saveDataToFile2(const std::vector<Boat>& boats, const wxString& filename) {
        std::ofstream file(filename.ToStdString(), std::ios::trunc); // Open the file in truncate mode
        if (file.is_open()) {
            for (const auto& boat : boats) {
                file << boat.model.ToStdString() << " " << boat.manufacturer.ToStdString() << " "
                    << boat.creationDate.ToStdString() << " " << boat.price.ToStdString() << " " << boat.id.ToStdString() << std::endl;
            }
            file.close();
        }
    }

    std::vector<Boat> loadDataFromFile(const wxString& filename) {
        std::vector<Boat> boats;

        std::ifstream file(filename.ToStdString());
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string model, manufacturer, creationDate, price, id;
                if (iss >> model >> manufacturer >> creationDate >> price >> id) {
                    Boat boat;
                    boat.model = wxString::FromUTF8(model.c_str());
                    boat.manufacturer = wxString::FromUTF8(manufacturer.c_str());
                    boat.creationDate = wxString::FromUTF8(creationDate.c_str());
                    boat.price = wxString::FromUTF8(price.c_str());
                    boat.id = wxString::FromUTF8(id.c_str());
                    boats.push_back(boat);
                }
            }
            file.close();
        }

        return boats;
    }

private:
    wxListCtrl* listCtrl;
    int& money;
    const int ID_UPDATE_MONEY_LABEL = wxNewId();
};



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