#include "essentials.cpp";

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