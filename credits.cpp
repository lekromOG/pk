#include "buy.cpp";

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