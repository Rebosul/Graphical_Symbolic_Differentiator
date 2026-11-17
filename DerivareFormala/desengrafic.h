#ifndef DESENGRAFIC_H
#define DESENGRAFIC_H

#include <graphics.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;


struct Nod {
    string info;
    Nod *stg, *dr;
    Nod(string val) : info(val), stg(nullptr), dr(nullptr) {}
};

int CalculeazaAdancime(Nod* nod) {
    if (nod == nullptr) return 0;
    int stg = CalculeazaAdancime(nod->stg);
    int dr = CalculeazaAdancime(nod->dr);
    return 1 + max(stg, dr);
}


#define NODE_COLOR LIGHTBLUE
#define TEXT_COLOR BLACK
#define LINE_COLOR BLACK
#define BUTTON_COLOR LIGHTGRAY


const char* imaginiFundal[4] = { "bg1.jpg", "bg2.jpg", "bg3.jpg", "bg4.jpg" };
int indexBackground = 0; // 0, 1 sau 2

#define INPUT_BG_COLOR LIGHTGRAY
#define INPUT_BOX_COLOR WHITE


enum { LANG_RO = 0, LANG_EN, LANG_FR, LANG_DE };
int limbaCurenta = LANG_RO;


int cameraX = 0;
int cameraY = 0;

struct Buton {
    int x, y, w, h;
    string text[4];
};


Buton butoane[5];

void DeseneazaBackground() {

    readimagefile(imaginiFundal[indexBackground], 0, 0, getmaxx(), getmaxy());
}

void InitializareButoane(int width, int height) {
    int btnW = 180;
    int btnH = 50;
    int spacing = 15;
    int startX = 20;
    int startY = 20;

    butoane[0] = {startX, startY, btnW, btnH};
    butoane[0].text[LANG_RO] = "Insereaza Expresie";
    butoane[0].text[LANG_EN] = "Insert Expression";
    butoane[0].text[LANG_FR] = "Inserer Expression";
    butoane[0].text[LANG_DE] = "Ausdruck Eingeben";

    butoane[1] = {startX + btnW + spacing, startY, btnW, btnH};
    butoane[1].text[LANG_RO] = "Deriveaza";
    butoane[1].text[LANG_EN] = "Derive";
    butoane[1].text[LANG_FR] = "Deriver";
    butoane[1].text[LANG_DE] = "Ableiten";

    butoane[2] = {startX + 2 * (btnW + spacing), startY, btnW, btnH};
    butoane[2].text[LANG_RO] = "Limba: RO";
    butoane[2].text[LANG_EN] = "Lang: EN";
    butoane[2].text[LANG_FR] = "Langue: FR";
    butoane[2].text[LANG_DE] = "Sprache: DE";

    butoane[3] = {startX + 3 * (btnW + spacing), startY, btnW, btnH};
    butoane[3].text[LANG_RO] = "Schimba Fundal";
    butoane[3].text[LANG_EN] = "Change BG";
    butoane[3].text[LANG_FR] = "Changer Fond";
    butoane[3].text[LANG_DE] = "Hintergrund";

    butoane[4] = {width - btnW - 20, startY, btnW, btnH};
    butoane[4].text[LANG_RO] = "Iesire";
    butoane[4].text[LANG_EN] = "Exit";
    butoane[4].text[LANG_FR] = "Quitter";
    butoane[4].text[LANG_DE] = "Beenden";
}

void DeseneazaInterfata() {
    DeseneazaBackground();

    setcolor(TEXT_COLOR);
    settextstyle(BOLD_FONT, HORIZ_DIR, 1);

    for(int i = 0; i < 5; i++) {
        setfillstyle(SOLID_FILL, BUTTON_COLOR);
        bar(butoane[i].x, butoane[i].y, butoane[i].x + butoane[i].w, butoane[i].y + butoane[i].h);
        rectangle(butoane[i].x, butoane[i].y, butoane[i].x + butoane[i].w, butoane[i].y + butoane[i].h);

        string text = butoane[i].text[limbaCurenta];
        setbkcolor(BUTTON_COLOR);

        if ((i == 0 || i == 3) && text.find(' ') != string::npos) {
            size_t pos = text.find(' ');
            string rand1 = text.substr(0, pos);
            string rand2 = text.substr(pos + 1);

            int h = textheight((char*)"H");

            int w1 = textwidth((char*)rand1.c_str());
            outtextxy(butoane[i].x + (butoane[i].w - w1) / 2,
                      butoane[i].y + (butoane[i].h / 2) - h + 2,
                      (char*)rand1.c_str());

            int w2 = textwidth((char*)rand2.c_str());
            outtextxy(butoane[i].x + (butoane[i].w - w2) / 2,
                      butoane[i].y + (butoane[i].h / 2) + 2,
                      (char*)rand2.c_str());
        }
        else {
            int textW = textwidth((char*)text.c_str());
            int textH = textheight((char*)text.c_str());

            outtextxy(butoane[i].x + (butoane[i].w - textW) / 2,
                      butoane[i].y + (butoane[i].h - textH) / 2,
                      (char*)text.c_str());
        }
    }
}

int VerificaClick(int x, int y) {

    for(int i = 0; i < 5; i++) {
        if (x >= butoane[i].x && x <= butoane[i].x + butoane[i].w &&
            y >= butoane[i].y && y <= butoane[i].y + butoane[i].h) {
            return i;
        }
    }
    return -1;
}
//Scroll
void DeseneazaNod(Nod* nod, int x, int y, int distantaX) {
    if (!nod) return;

    // Aplicam offset-ul camerei
    int screenX = x - cameraX;
    int screenY = y - cameraY;

    // Coordonata Y pentru copii este aceeasi indiferent de tip
    int childY = (y + 80) - cameraY;
    int raza = 28;

    setcolor(LINE_COLOR);
    setlinestyle(SOLID_LINE, 0, 3);

    // CAZ 1: Functie Unara (Are doar stanga, nu are dreapta)
    if (nod->stg != nullptr && nod->dr == nullptr) {

        // Copilul va fi desenat exact sub parinte (acelasi X)
        int childX = x - cameraX;

        // Tragem linie verticala
        line(screenX, screenY + raza, childX, childY - raza);

        DeseneazaNod(nod->stg, x, y + 80, distantaX);
    }
    // CAZ 2: Operator Binar (Are ambii copii sau doar un copil dar nu e tratat ca unar strict)
    // Ex: +, *, /
    else {
        // Calculam coordonatele stanga/dreapta standard
        int childLeftX = (x - distantaX) - cameraX;
        int childRightX = (x + distantaX) - cameraX;

        if (nod->stg) {
            line(screenX, screenY + raza, childLeftX, childY - raza);
            DeseneazaNod(nod->stg, x - distantaX, y + 80, distantaX / 2);
        }
        if (nod->dr) {
            line(screenX, screenY + raza, childRightX, childY - raza);
            DeseneazaNod(nod->dr, x + distantaX, y + 80, distantaX / 2);
        }
    }

    setlinestyle(SOLID_LINE, 0, 1);

    // Verificam daca nodul este vizibil pe ecran
    if (screenX + raza > 0 && screenX - raza < getmaxx() &&
        screenY + raza > 100 && screenY - raza < getmaxy())
    {
        setfillstyle(SOLID_FILL, NODE_COLOR);
        fillellipse(screenX, screenY, raza, raza);

        setcolor(TEXT_COLOR);
        setbkcolor(NODE_COLOR);
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);

        int tw = textwidth((char*)nod->info.c_str());
        int th = textheight((char*)"H");
        outtextxy(screenX - tw / 2, screenY - th / 2 + 2, (char*)nod->info.c_str());
    }
}

string GetLabelExpresie() {
    if (limbaCurenta == LANG_RO) return "Expresie curenta: ";
    if (limbaCurenta == LANG_EN) return "Current expression: ";
    if (limbaCurenta == LANG_FR) return "Expression actuelle: ";
    if (limbaCurenta == LANG_DE) return "Aktueller Ausdruck: ";
    return "";
}

string GetLabelVid() {
    if (limbaCurenta == LANG_RO) return "Arbore vid.";
    if (limbaCurenta == LANG_EN) return "Empty tree.";
    if (limbaCurenta == LANG_FR) return "Arbre vide.";
    if (limbaCurenta == LANG_DE) return "Leerer Baum.";
    return "";
}

// Helper pentru instructiuni scroll
string GetLabelScroll() {
    if (limbaCurenta == LANG_RO) return "(Folositi sagetile pentru a muta arborele)";
    if (limbaCurenta == LANG_EN) return "(Use arrow keys to scroll)";
    if (limbaCurenta == LANG_FR) return "(Utilisez les fleches pour faire defiler)";
    if (limbaCurenta == LANG_DE) return "(Verwenden Sie die Pfeiltasten zum Scrollen)";
    return "";
}

void AfiseazaArboreGrafic(Nod* radacina, string expresieText) {
    DeseneazaInterfata();

    setcolor(TEXT_COLOR);
    settextstyle(BOLD_FONT, HORIZ_DIR, 1);

    string label = GetLabelExpresie();
    string textFinal = label + expresieText;

    setbkcolor(WHITE);
    outtextxy(20, 110, (char*)textFinal.c_str());

    string scrollHelp = GetLabelScroll();
    int helpW = textwidth((char*)scrollHelp.c_str());
    outtextxy(getmaxx() - helpW - 20, 110, (char*)scrollHelp.c_str());

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);

    if (radacina == nullptr) {
        setbkcolor(WHITE);
        outtextxy(50, 150, (char*)GetLabelVid().c_str());
        return;
    }


    // 1. Calculam cat de adanc este arborele
    int adancime = CalculeazaAdancime(radacina);

    // 2. Definim spatiul minim necesar intre doua noduri la cel mai de jos nivel
    int unitateMinima = 25;

    // 3. Calculam distanta initiala (pentru nivelul radacinii)
    int distantaInitiala = unitateMinima * pow(2, adancime - 1);

    // Siguranta pentru arbori mici (sa nu fie prea inghesuiti nici daca sunt mici)
    if (distantaInitiala < 70) distantaInitiala = 70;

    // 4. Desenam arborele
    // Pozitia Y este fixa (160), dar X si distanta sunt dinamice
    DeseneazaNod(radacina, getmaxx() / 2, 160, distantaInitiala);
}

string GetLabelInput() {
    if (limbaCurenta == LANG_RO) return "Tastati expresia si apasati ENTER:";
    if (limbaCurenta == LANG_EN) return "Type expression and press ENTER:";
    if (limbaCurenta == LANG_FR) return "Entrez l'expression et appuyez sur ENTER:";
    if (limbaCurenta == LANG_DE) return "Ausdruck eingeben und ENTER drucken:";
    return "";
}

string CitesteExpresieGrafic() {
    string input = "";
    char c;

    int dialogW = 600;
    int dialogH = 150;
    int dialogX = (getmaxx() - dialogW) / 2;
    int dialogY = 150;

    int boxX = dialogX + 50;
    int boxY = dialogY + 80;
    int boxW = dialogW - 100;
    int boxH = 40;

    setfillstyle(SOLID_FILL, INPUT_BG_COLOR);
    bar(dialogX, dialogY, dialogX + dialogW, dialogY + dialogH);
    setcolor(BLACK);
    rectangle(dialogX, dialogY, dialogX + dialogW, dialogY + dialogH);

    setcolor(TEXT_COLOR);
    setbkcolor(INPUT_BG_COLOR);
    settextstyle(BOLD_FONT, HORIZ_DIR, 1);

    string msg = GetLabelInput();
    int msgW = textwidth((char*)msg.c_str());
    outtextxy(dialogX + (dialogW - msgW)/2, dialogY + 30, (char*)msg.c_str());

    do {
        setfillstyle(SOLID_FILL, INPUT_BOX_COLOR);
        bar(boxX, boxY, boxX + boxW, boxY + boxH);
        setcolor(BLACK);
        rectangle(boxX, boxY, boxX + boxW, boxY + boxH);

        setbkcolor(INPUT_BOX_COLOR);
        setcolor(BLACK);
        settextstyle(BOLD_FONT, HORIZ_DIR, 1);
        outtextxy(boxX + 10, boxY + 10, (char*)input.c_str());

        c = getch();

        if (c == 8) {
            if (input.length() > 0) input.pop_back();
        }
        else if (c == 13) {
            break;
        }
        else if (c >= 32 && c <= 126) {
            input += c;
        }

    } while (c != 13);

    return input;
}

#endif
