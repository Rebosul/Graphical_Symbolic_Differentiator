#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cctype>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <graphics.h>
#include "desengrafic.h"

using namespace std;


bool este_operator(Nod* nod) {
    if (!nod) return false;
    if (nod->stg || nod->dr) return true;
    return false;
}

bool EsteFunctie(const string& s) {
    return (s == "sin" || s == "cos" || s == "ln" || s == "tg" ||
            s == "tan" || s == "ctg" || s == "cot" || s == "sqrt" ||
            s == "radical" || s == "arcsin" || s == "arccos" ||
            s == "arctg" || s == "arctan" || s == "arcctg" || s == "arccot");
}

bool EsteOperand(const string& s) {
    if (s == "x") return true;
    if (s == "e") return true;
    if (isdigit(s[0]) || (s.length() > 1 && s[0] == '-')) return true;
    return false;
}

bool EsteOperatorStrictBinar(const string& s) {
    // Minusul poate fi unar, deci il tratam separat
    return (s == "+" || s == "*" || s == "/" || s == ":" || s == "^");
}

bool EsteNumar(const string& s) {
    if (s.empty()) return false;

    if (s[0] == '.') return false;

    // Daca incepe cu minus urmat imediat de punct (ex: "-.5")
    if (s.length() > 1 && s[0] == '-' && s[1] == '.') return false;
    int puncte = 0;
    bool areCifre = false; // Verificam daca are macar o cifra

    for (int i = 0; i < s.length(); ++i) {
        char c = s[i];
        if (c == '-') {
            // Minusul e permis doar pe prima pozitie
            if (i > 0) return false;
        }
        else if (c == '.') {
            puncte++;
        }
        else if (isdigit(c)) {
            areCifre = true;
        }
        else {
            return false; // Orice alt caracter
        }
    }

    if (puncte > 1) return false;
    if (!areCifre) return false; // daca e doar "-", ".", "-."

    // Verificari extra pentru formare corecta (ex: nu 05, dar permitem 0.5)
    if (s.length() > 1 && s[0] == '0' && s[1] != '.') return false;
    if (s.length() > 2 && s[0] == '-' && s[1] == '0' && s[2] != '.') return false;

    return true;
}

// Determinarea prioritatii operatorilor
int prioritate(char op) {
    if (op == '(' || op == ')') return 0;
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == ':') return 2;
    if (op == '^') return 3;
    if (op == 's' || op == 'c' || op == 'l' || op == 'a' || op == 't' || op == 'r') return 4; // sin, cos, ln, exp
    return -1;
}

vector<string> Vectorizeaza(const string& expr)
{
    vector<string> rezultat;
    int i = 0;
    while (i < expr.length())
    {
        if (expr[i] == ' ') {
            i++; // ignora spatiile
            continue;
        }

        //pentru cazurile: 2x, x(..), )(..), 2sin(..)
        bool posibilFactorStanga = false;
        if (!rezultat.empty()) {
            string last = rezultat.back();
            // Daca ultimul element e numar, 'x' sau ')', poate fi un factor
            if (isdigit(last.back()) || last == "x" || last == ")") {
                posibilFactorStanga = true;
            }
        }
        if (i + 4 <= expr.length() && (expr.substr(i, 4) == "sqrt"))
        {
            if (posibilFactorStanga) {
                    rezultat.push_back("*");
                }
            rezultat.push_back(expr.substr(i, 4));
            i += 4;
        }
        else if (i + 7 <= expr.length() && (expr.substr(i, 7) == "radical"))
        {
            if (posibilFactorStanga) {
                    rezultat.push_back("*");
                }
            rezultat.push_back("sqrt");
            i += 7;
        }
        else if (i + 3 <= expr.length() && (expr.substr(i, 3) == "sin" ||
                expr.substr(i, 3) == "cos" || expr.substr(i, 3) == "tan"
                || expr.substr(i, 3) == "ctg"|| expr.substr(i, 3) == "cot"))
        {

                if (posibilFactorStanga) {
                    rezultat.push_back("*");
                }
            rezultat.push_back(expr.substr(i, 3));
            i += 3;
        }
        else if (i + 2 <= expr.length() && (expr.substr(i, 2) == "tg"))
        {
            if (posibilFactorStanga) {
                    rezultat.push_back("*");
                }
            rezultat.push_back(expr.substr(i, 2));
            i += 2;
        }
        else if (i + 5 <= expr.length() && (expr.substr(i, 5) == "arctg"))
        {
            if (posibilFactorStanga) {
                    rezultat.push_back("*");
                }
            rezultat.push_back(expr.substr(i, 5));
            i += 5;
        }
        else if (i + 6 <= expr.length() && (expr.substr(i, 6) == "arcsin" ||
                    expr.substr(i, 6) == "arccos" || expr.substr(i, 6) == "arctan"
                    || expr.substr(i, 6) == "arccot"|| expr.substr(i, 6) == "arcctg"))
        {
            if (posibilFactorStanga) {
                    rezultat.push_back("*");
                }
            rezultat.push_back(expr.substr(i, 6));
            i += 6;
        }
        else if (i + 2 <= expr.length() && expr.substr(i, 2) == "ln")
        {
            if (posibilFactorStanga) {
                    rezultat.push_back("*");
                }
            rezultat.push_back(expr.substr(i, 2));
            i += 2;
        }
        else if (strchr("()+-*/^xe", expr[i])) // tratam operatorii si variabilele simple
        {
            string token(1, expr[i]);

            //  2x sau )x -> adaugam * inainte de x
            if (token == "(" || token == "x") {
                if (posibilFactorStanga) {
                    rezultat.push_back("*");
                }
            }
            rezultat.push_back(string(1, expr[i]));
            i++;
        }
        else if (strchr(":", expr[i])) // tratam operatorii si variabilele simple
        {

            rezultat.push_back("/");
            i++;
        }
        else if (isdigit(expr[i])) // tratam numerele
        {
            if (!rezultat.empty()) {
                string last = rezultat.back();
                if (last == "x" || last == ")") {
                    rezultat.push_back("*");
                }
            }
            string numar;
            while (i < expr.length() && (isdigit(expr[i]) || expr[i] == '.'))
            {
                numar += expr[i];
                i++;
            }
            rezultat.push_back(numar);
        }
        else
        {
            cout << "Input invalid la caracterul: " << expr[i] << endl;
            i++;
        }
    }
    return rezultat;
}

bool ValideazaExpresie(const vector<string>& tokeni) {
    if (tokeni.empty()) return false;

    int paranteze = 0;

    // 1. Verificari inceput/sfarsit
    if (EsteOperatorStrictBinar(tokeni[0]) || tokeni[0] == ")") {
        cout << "Eroare: Expresia nu poate incepe cu: " << tokeni[0] << endl;
        return false;
    }
    // O functie nu poate fi la final (ex: "x + sin")
    if (EsteOperatorStrictBinar(tokeni.back()) || tokeni.back() == "(" || tokeni.back() == "-" || EsteFunctie(tokeni.back())) {
        cout << "Eroare: Expresia nu se poate termina cu un operator sau functie deschisa." << endl;
        return false;
    }

    for (int i = 0; i < tokeni.size(); ++i) {
        string t = tokeni[i];

        // Paranteze
        if (t == "(") paranteze++;
        else if (t == ")") paranteze--;

        if (paranteze < 0) {
            cout << "Eroare: Paranteza inchisa in plus la pozitia " << i << endl;
            return false;
        }

        //  Numere Invalide
        if (isdigit(t[0]) || (t.length() > 1 && t[0] == '-' && isdigit(t[1]))) {
            if (!EsteNumar(t)) {
                cout << "Eroare: Numar invalid detectat: " << t << endl;
                return false;
            }
        }

        // Verificari Operatori
        if (i < tokeni.size() - 1) {
            string urmator = tokeni[i+1];

            //  Operator Binar urmat de Operator Binar (ex: + *)
            if ((EsteOperatorStrictBinar(t) || t == "-") && EsteOperatorStrictBinar(urmator)) {
                 cout << "Eroare: Operatori consecutivi: " << t << " " << urmator << endl;
                 return false;
            }

            //  Functie urmata de Operator Binar sau ')' (ex: sin * x, sin ) )
            // O functie trebuie urmata de '(' sau de un operand.
            if (EsteFunctie(t)) {
                if (EsteOperatorStrictBinar(urmator) || urmator == ")") {
                    cout << "Eroare: Functia '" << t << "' nu are argument valid." << endl;
                    return false;
                }
            }

            //  Impartire la zero (ex: / 0)
            if ((t == "/" ) && urmator == "0") {
                cout << "Eroare: Impartire la zero." << endl;
                return false;
            }

            //  Operanzi Adiacenti (ex: "5 5")
            if (EsteOperand(t) && EsteOperand(urmator)) {
                cout << "Eroare: Lipseste operator intre " << t << " si " << urmator << endl;
                return false;
            }


            //  Verificari Matematice Statice (Radical/Logaritm din numere negative explicite)

            if ((t == "sqrt" || t == "radical") && urmator.length() > 1 && urmator[0] == '-') {
                 cout << "Eroare: Radical din numar negativ: " << urmator << endl;
                 return false;
            }
            if ((t == "ln") && (urmator == "0" || (urmator.length() > 1 && urmator[0] == '-'))) {
                 cout << "Eroare: Logaritm natural invalid (din 0 sau negativ): " << urmator << endl;
                 return false;
            }
        }
    }

    if (paranteze != 0) {
        cout << "Eroare: Numar inegal de paranteze." << endl;
        return false;
    }

    return true;
}

// Construieste Arborele de Expresie din vectorul de token-uri
Nod* Arborizeaza(vector<string>& vec) {
    stack<Nod*> operand;
    stack<string> operatori;

    // Daca asteptam un operand si primim un '-', atunci acel minus e unar.
    bool asteaptaOperand = true;

    for (const string& token : vec) {
        char c = token[0];

        // Daca e operand (numar sau variabila)
        if (isdigit(c) || c == 'x' || c == 'e' || (token.length() > 1 && isdigit(token[1]))) {
            operand.push(new Nod(token));
            asteaptaOperand = false; // Dupa un numar, ne asteptam la un operator
        }
        //  Daca e paranteza deschisa
        else if (c == '(') {
            operatori.push(token);
            asteaptaOperand = true; // Dupa '(' ne asteptam la un operand (sau un minus unar)
        }
        // Daca e paranteza inchisa
        else if (c == ')') {
            while (!operatori.empty() && operatori.top() != "(") {
                string op = operatori.top(); operatori.pop();
                Nod* nodNou = new Nod(op);

                // Verificam daca e functie unara (sin, cos etc)
                 if (op == "sin" || op == "cos" || op == "ln" || op == "e" || op == "tg" || op =="tan" || op == "ctg" || op == "cot" || op == "sqrt" ||
                     op == "radical" || op == "arcsin" || op == "arccos" || op == "arctg" || op == "arctan" || op == "arcctg" || op == "arccot") {
                    if(!operand.empty()) { nodNou->stg = operand.top(); operand.pop(); }
                } else {
                    if(!operand.empty()) { nodNou->dr = operand.top(); operand.pop(); }
                    if(!operand.empty()) { nodNou->stg = operand.top(); operand.pop(); }
                }
                operand.push(nodNou);
            }
            if (!operatori.empty()) operatori.pop(); // Scoate '('
            asteaptaOperand = false; // Dupa ')' ne asteptam la un operator (ex: (a+b)*...)
        }
        //  Daca e operator sau functie
        else {
            // Tratare Minus Unar
            // Daca intalnim "-" si suntem intr-un context unde asteptam un operand
            // (adica la inceputul expresiei sau dupa o paranteza deschisa sau dupa alt operator)
            if (token == "-" && asteaptaOperand) {
                // Introduce un 0 virtual in stiva de operanzi
                operand.push(new Nod("0"));
                // Acum minusul va fi tratat ca scadere binara: 0 - x
            }

            while (!operatori.empty() && operatori.top() != "(" &&
                   prioritate(operatori.top()[0]) >= prioritate(c)) {

                string op = operatori.top(); operatori.pop();
                Nod* nodNou = new Nod(op);

                if (op == "sin" || op == "cos" || op == "ln" || op == "tg" || op =="tan" || op == "ctg" || op == "cot" || op == "sqrt" ||
                     op == "radical" || op == "arcsin" || op == "arccos" || op == "arctg" || op == "arctan" || op == "arcctg" || op == "arccot") {
                    if(!operand.empty()) { nodNou->stg = operand.top(); operand.pop(); }
                } else {
                    if(!operand.empty()) { nodNou->dr = operand.top(); operand.pop(); }
                    if(!operand.empty()) { nodNou->stg = operand.top(); operand.pop(); }
                }
                operand.push(nodNou);
            }
            operatori.push(token);
            asteaptaOperand = true; // Dupa un operator (+, -, *, sin), ne asteptam la un operand
        }
    }

    // Goleste stiva ramasa
    while (!operatori.empty()) {
       string op = operatori.top(); operatori.pop();
       Nod* nodNou = new Nod(op);

       if (operand.empty()) break;

       if (op == "sin" || op == "cos" || op == "ln" || op == "tg" || op =="tan" || op == "ctg" || op == "cot" || op == "sqrt" ||
            op == "radical" || op == "arcsin" || op == "arccos" || op == "arctg" || op == "arctan" || op == "arcctg" || op == "arccot") {
            nodNou->stg = operand.top(); operand.pop();
       } else {
            nodNou->dr = operand.top(); operand.pop();
            if(!operand.empty()) { nodNou->stg = operand.top(); operand.pop(); }
       }
       operand.push(nodNou);
    }

    return operand.empty() ? nullptr : operand.top();
}

Nod* Copie(Nod* radacina) {
    if (radacina == nullptr) return nullptr;
    Nod* nou = new Nod(radacina->info);
    nou->stg = Copie(radacina->stg);
    nou->dr = Copie(radacina->dr);
    return nou;
}

Nod* Deriveaza(Nod* nod) {
    if (!nod) return nullptr;

    string info = nod->info;
    char c = info[0];

    if (isdigit(c) || (c=='-' && info.length()>1) || info == "e") return new Nod("0");

    if (info == "x") return new Nod("1");

    Nod* rezultat = new Nod("");

    if (info == "+" || info == "-") {
        rezultat->info = info;
        rezultat->stg = Deriveaza(nod->stg);
        rezultat->dr = Deriveaza(nod->dr);
    }
    else if (info == "*") {
        rezultat->info = "+";
        Nod* stgTerm = new Nod("*");
        stgTerm->stg = Deriveaza(nod->stg);
        stgTerm->dr = Copie(nod->dr);
        Nod* drTerm = new Nod("*");
        drTerm->stg = Copie(nod->stg);
        drTerm->dr = Deriveaza(nod->dr);

        rezultat->stg = stgTerm;
        rezultat->dr = drTerm;
    }
    else if (info == "/" || info == ":") {
        rezultat->info = "/";
        Nod* numarator = new Nod("-");
        Nod* term1 = new Nod("*");
        term1->stg = Deriveaza(nod->stg);
        term1->dr = Copie(nod->dr);

        Nod* term2 = new Nod("*");
        term2->stg = Copie(nod->stg);
        term2->dr = Deriveaza(nod->dr);

        numarator->stg = term1;
        numarator->dr = term2;

        Nod* numitor = new Nod("^");
        numitor->stg = Copie(nod->dr);
        numitor->dr = new Nod("2");

        rezultat->stg = numarator;
        rezultat->dr = numitor;
    }
    else if (info == "^") {
        if (nod->stg->info == "e") {
            rezultat->info = "*";

            Nod* e_la_u = new Nod("^");
            e_la_u->stg = new Nod("e");
            e_la_u->dr = Copie(nod->dr);

            rezultat->stg = e_la_u;

            rezultat->dr = Deriveaza(nod->dr);
        }
        else
        {
        rezultat->info = "*";

        Nod* n_ori_u_la_n_minus_1 = new Nod("*");
        n_ori_u_la_n_minus_1->stg = Copie(nod->dr);

        Nod* putere = new Nod("^");
        putere->stg = Copie(nod->stg);

        Nod* minusul = new Nod("-");
        minusul->stg = Copie(nod->dr);
        minusul->dr = new Nod("1");

        putere->dr = minusul;
        n_ori_u_la_n_minus_1->dr = putere;

        rezultat->stg = n_ori_u_la_n_minus_1;
        rezultat->dr = Deriveaza(nod->stg);
    }
    }
    else if (info == "sqrt" || info == "radical") {
        rezultat->info =  "*";
        Nod* impartire = new Nod("/");
        Nod* One = new Nod("1");
        Nod* numitor = new Nod("*");
        Nod* Two = new Nod("2");
        Nod* radicalNou = new Nod(nod->info);

        numitor->stg = Two;
        numitor->dr = radicalNou;
        radicalNou->stg = Copie(nod->stg);
        impartire->stg = One;
        impartire->dr = numitor;
        rezultat->stg = impartire;
        rezultat->dr = Deriveaza(nod->stg);
    }
    else if (info == "sin") {
        rezultat->info = "*";
        Nod* cosNode = new Nod("cos");
        cosNode->stg = Copie(nod->stg);
        rezultat->stg = cosNode;
        rezultat->dr = Deriveaza(nod->stg);
    }
    else if (info == "cos") {
        rezultat->info = "*";
        Nod* minusOne = new Nod("-1");
        Nod* sinPart = new Nod("*");
        Nod* sinNode = new Nod("sin");
        sinNode->stg = Copie(nod->stg);
        sinPart->stg = sinNode;
        sinPart->dr = Deriveaza(nod->stg);

        rezultat->stg = minusOne;
        rezultat->dr = sinPart;
    }
    else if (info == "tg" || info == "tan")
    {
        rezultat->info = "*";
        Nod* impartire = new Nod("/");
        Nod* One = new Nod("1");
        Nod* putere = new Nod("^");
        Nod* stgputere = new Nod("cos");
        Nod* drputere = new Nod("2");

        putere->stg = stgputere;
        putere->dr = drputere;
        stgputere->stg = Copie(nod->stg);
        impartire->stg = One;
        impartire->dr = putere;
        rezultat->stg = impartire;
        rezultat->dr = Deriveaza(nod->stg);
    }
    else if (info == "ctg" || info == "cot")
    {
        rezultat->info = "*";
        Nod* impartire = new Nod("/");
        Nod* One = new Nod("-1");
        Nod* putere = new Nod("^");
        Nod* stgputere = new Nod("sin");
        Nod* drputere = new Nod("2");

        putere->stg = stgputere;
        putere->dr = drputere;
        stgputere->stg = nod->stg;
        impartire->stg = One;
        impartire->dr = putere;
        rezultat->stg = impartire;
        rezultat->dr = Deriveaza(nod->stg);
    }
    else if (info == "arcsin") {
        rezultat->info =  "*";
        Nod* impartire = new Nod("/");
        Nod* One = new Nod("1");
        Nod* numitor = new Nod("sqrt");
        Nod* minusul = new Nod("-");
        Nod* One_Two = new Nod("1");
        Nod* putere = new Nod("^");
        Nod* Two = new Nod("2");

        putere->stg = Copie(nod->stg);
        putere->dr = Two;
        minusul->stg = One_Two;
        minusul->dr = putere;
        numitor->stg = minusul;
        impartire->stg = One;
        impartire->dr = numitor;
        rezultat->stg = impartire;
        rezultat->dr = Deriveaza(nod->stg);
    }
    else if (info == "arccos") {
        rezultat->info =  "*";
        Nod* impartire = new Nod("/");
        Nod* Minus_One = new Nod("-1");
        Nod* numitor = new Nod("sqrt");
        Nod* minusul = new Nod("-");
        Nod* One_Two = new Nod("1");
        Nod* putere = new Nod("^");
        Nod* Two = new Nod("2");

        putere->stg = Copie(nod->stg);
        putere->dr = Two;
        minusul->stg = One_Two;
        minusul->dr = putere;
        numitor->stg = minusul;
        impartire->stg = Minus_One;
        impartire->dr = numitor;
        rezultat->stg = impartire;
        rezultat->dr = Deriveaza(nod->stg);
    }
    else if (info == "arctg" || info == "arctan") {
        rezultat->info =  "*";
        Nod* impartire = new Nod("/");
        Nod* One = new Nod("1");
        Nod* numitor = new Nod("+");
        Nod* putere = new Nod("^");
        Nod* One_Two = new Nod("1");
        Nod* Two = new Nod("2");

        putere->stg = Copie(nod->stg);
        putere->dr = Two;
        numitor->stg = putere;
        numitor->dr = One_Two;
        impartire->stg = One;
        impartire->dr = numitor;
        rezultat->stg = impartire;
        rezultat->dr = Deriveaza(nod->stg);
    }
    else if (info == "arcctg" || info == "arccot") {
        rezultat->info =  "*";
        Nod* impartire = new Nod("/");
        Nod* Minus_One = new Nod("-1");
        Nod* numitor = new Nod("+");
        Nod* putere = new Nod("^");
        Nod* One_Two = new Nod("1");
        Nod* Two = new Nod("2");

        putere->stg = Copie(nod->stg);
        putere->dr = Two;
        numitor->stg = putere;
        numitor->dr = One_Two;
        impartire->stg = Minus_One;
        impartire->dr = numitor;
        rezultat->stg = impartire;
        rezultat->dr = Deriveaza(nod->stg);
    }
    else if (info == "ln") {
        rezultat->info = "/";
        rezultat->stg = Deriveaza(nod->stg);
        rezultat->dr = Copie(nod->stg);
    }
    else if (info == "e") {
         rezultat->info = "*";
         Nod* eNode = new Nod("e");
         eNode->stg = Copie(nod->stg);
         rezultat->stg = eNode;
         rezultat->dr = Deriveaza(nod->stg);
    }

    return rezultat;
}

string Calculeaza(string op, string val1, string val2) {
    double v1 = stod(val1);
    double v2 = stod(val2);
    double rez = 0;

    if (op == "+") rez = v1 + v2;
    else if (op == "-") rez = v1 - v2;
    else if (op == "*") rez = v1 * v2;
    else if (op == "/" || op == ":") rez = v1 / v2;
    else if (op == "^") rez = pow(v1, v2);

    // In cazul in care rez = -0
    if (rez == 0.0) rez = 0.0;


    string s = to_string(rez);

    // Curatare zerouri inutile
    size_t decimalPos = s.find('.');
    if (decimalPos != string::npos) {
        s.erase(s.find_last_not_of('0') + 1, string::npos);
        if (s.back() == '.') s.pop_back();
    }

    return s;
}

bool EsteZeroLogic(const string& s) {
    if (s == "0" || s == "-0" || s == "0." || s == "-0." || s == "0.0")
        return true;
    return false;
}

bool EsteUnuLogic(const string& s) {
    return (s == "1" || s == "1.0" || s == "1.");
}

bool SuntIdentice(Nod* a, Nod* b) {
    if (!a && !b) return true;
    if (!a || !b) return false;
    if (a->info != b->info) return false;
    return SuntIdentice(a->stg, b->stg) && SuntIdentice(a->dr, b->dr);
}

double GetCoeficientSmart(Nod* n, Nod*& variabila) {
    // Cazul 1: Nodul nu e înmulțire, e doar variabila simplă (ex: "x")
    // Coeficientul implicit este 1.0
    if (n->info != "*") {
        variabila = n;
        return 1.0;
    }

    // Cazul 2: Este înmulțire ("*")
    // Sub-cazul A: Numărul e în STÂNGA (ex: 2*x)
    if (EsteNumar(n->stg->info)) {
        variabila = n->dr; // Variabila e în dreapta
        return stod(n->stg->info);
    }

    // Sub-cazul B: Numărul e în DREAPTA (ex: x*2)
    if (EsteNumar(n->dr->info)) {
        variabila = n->stg; // Variabila e în stânga
        return stod(n->dr->info);
    }

    // Cazul de siguranță: ex: x*y (niciunul nu e număr)
    variabila = nullptr;
    return 0.0;
}

// Functie pentru simplificarea arborelui
Nod* Simplifica(Nod* nod) {
    if (nod == nullptr) return nullptr;

    // 1. Simplificare recursivă (post-order)
    nod->stg = Simplifica(nod->stg);
    nod->dr = Simplifica(nod->dr);

    string info = nod->info;

    // A. CALCULARE CONSTANTE (Constant Folding)
    if (nod->stg && nod->dr && EsteNumar(nod->stg->info) && EsteNumar(nod->dr->info)) {
        if ((info == "/" || info == ":") && EsteZeroLogic(nod->dr->info)) {
            // Împărțire la zero: nu facem nimic (sau aruncăm eroare)
        } else {
            string rezultat = Calculeaza(info, nod->stg->info, nod->dr->info);
            // Curățăm memoria veche (opțional, dar recomandat)
            // delete nod->stg; delete nod->dr;
            return new Nod(rezultat);
        }
    }

    // B. ADUNARE (+)
    if (info == "+") {
        bool stgZero = (nod->stg && EsteZeroLogic(nod->stg->info));
        bool drZero = (nod->dr && EsteZeroLogic(nod->dr->info));

        if (stgZero && drZero) return new Nod("0");
        if (stgZero) return nod->dr;
        if (drZero) return nod->stg;

        // Cazul simplu: x + x = 2*x
        if (SuntIdentice(nod->stg, nod->dr)) {
            Nod* inmultire = new Nod("*");
            inmultire->stg = new Nod("2");
            inmultire->dr = Copie(nod->stg);
            return Simplifica(inmultire);
        }

        // Cazul complex: 2*x + 3*x sau x + 5*x
        Nod* varStg = nullptr;
        Nod* varDr = nullptr;

        double coef1 = GetCoeficientSmart(nod->stg, varStg);
        double coef2 = GetCoeficientSmart(nod->dr, varDr);

        if (varStg && varDr && SuntIdentice(varStg, varDr)) {
            double suma = coef1 + coef2;

            if (abs(suma) < 0.000001) return new Nod("0"); // Ex: x + (-x)
            if (abs(suma - 1.0) < 0.000001) return Copie(varStg); // Ex: 0.5x + 0.5x = x

            Nod* nou = new Nod("*");
            string calc = to_string((int)suma);
            // Eliminăm zerourile inutile (ex: "3.0000" -> "3")
            if (calc.find('.') != string::npos) {
                calc.erase(calc.find_last_not_of('0') + 1, string::npos);
                if (calc.back() == '.') calc.pop_back();
            }

            nou->stg = new Nod(calc);
            nou->dr = Copie(varStg);
            return nou;
        }
    }

    // C. SCĂDERE (-)
    else if (info == "-") {
        bool stgZero = (nod->stg && EsteZeroLogic(nod->stg->info));
        bool drZero = (nod->dr && EsteZeroLogic(nod->dr->info));

        if (stgZero && drZero) return new Nod("0");
        if (drZero) return nod->stg;
        if (stgZero) {
            // Transformam nodul curent din binar (scadere) in unar (negare)
            nod->stg = nod->dr; // Mutam x-ul in stanga
            nod->dr = nullptr;  // Stergem dreapta
        }
        //  -(-x) = x
        if (nod->dr == nullptr) { // Daca e minus unar
            if (nod->stg && nod->stg->info == "-" && nod->stg->dr == nullptr) { // Si fiul e minus unar
                return nod->stg->stg; // Returnez nepotul (x)
            }
        }

        //  Regula: x - (-y) = x + y
        if (nod->dr && nod->dr->info == "-" && nod->dr->dr == nullptr) {
            nod->info = "+";
            nod->dr = nod->dr->stg; // Luam y-ul din minusul unar
            return Simplifica(nod); // Resimplificam noua adunare
        }
    }

    // D. ÎNMULȚIRE (*)
    else if (info == "*") {
        // 0 * x = 0
        if ((nod->stg && EsteZeroLogic(nod->stg->info)) || (nod->dr && EsteZeroLogic(nod->dr->info))) {
            return new Nod("0");
        }
        // 1 * x = x
        if (nod->stg && EsteUnuLogic(nod->stg->info)) return nod->dr;
        if (nod->dr && EsteUnuLogic(nod->dr->info)) return nod->stg;

        // -1 * (-1 * x) -> x
        if (nod->stg && nod->stg->info == "-1" &&
            nod->dr && nod->dr->info == "*" &&
            nod->dr->stg && nod->dr->stg->info == "-1") {
            return nod->dr->dr;
        }

        // -1 * x -> -x (Minus Unar)
        if (nod->stg && nod->stg->info == "-1") {
            nod->info = "-";     // Schimbăm operatorul în minus
            nod->stg = nod->dr;  // Mutăm x în stânga (formatul unar)
            nod->dr = nullptr;   // Anulăm dreapta
            return nod;
        }

        // x * -1 -> -x (Minus Unar)
        if (nod->dr && nod->dr->info == "-1") {
            nod->info = "-";
            nod->dr = nullptr; // x era deja în stânga
            return nod;
        }
        //(A / B) * B -> A
        if (nod->stg && (nod->stg->info == "/" || nod->stg->info == ":")) {
            // Verificam daca numitorul (stg->dr) este identic cu factorul din dreapta (nod->dr)
            if (SuntIdentice(nod->stg->dr, nod->dr)) {
                 return nod->stg->stg; // Returnam A
            }
        }

        //B * (A / B) -> A
        if (nod->dr && (nod->dr->info == "/" || nod->dr->info == ":")) {
             // Verificam daca numitorul (dr->dr) este identic cu factorul din stanga (nod->stg)
            if (SuntIdentice(nod->dr->dr, nod->stg)) {
                return nod->dr->stg; // Returnam A
            }
        }





    // 2. Logaritmi: ln(e) -> 1 si ln(e^x) -> x
    // ln(1/x) -> -ln(x)
    if (info == "ln" && nod->stg) {
        if (nod->stg->info == "/" || nod->stg->info == ":") {
             // Verificam daca numaratorul este 1
             if (EsteUnuLogic(nod->stg->stg->info)) {

                 // Cream structura: -1 * ln(numitor)
                 Nod* minusUnu = new Nod("-1");
                 Nod* lnNou = new Nod("ln");
                 lnNou->stg = Copie(nod->stg->dr); // Luam numitorul (x)

                 Nod* rezultat = new Nod("*");
                 rezultat->stg = minusUnu;
                 rezultat->dr = lnNou;

                 return Simplifica(rezultat);
             }
        }
    }
    if (info == "ln") {
        if (nod->stg) {
            if (nod->stg->info == "e") return new Nod("1");

            // ln(e^x) -> x
            if (nod->stg->info == "^") {
                if (nod->stg->stg && nod->stg->stg->info == "e") {
                    return Copie(nod->stg->dr); // Returnam exponentul
                }
            }
        }
    }

        // --- OPTIMIZARE: GRUPARE CONSTANTE (ASOCIATIVITATE) ---

        // CAZUL A: Arbore extins spre STANGA -> ( ... ) * NUMAR
        // Ex: (e^x * 4) * 4   SAU   (4 * e^x) * 4
        if (nod->dr && EsteNumar(nod->dr->info)) {
            if (nod->stg && nod->stg->info == "*") {
                string val2 = nod->dr->info; // Numarul din dreapta de tot (ex: ultimul 4)

                // Subcaz A1: (Ceva * NUMAR) * NUMAR -> Ceva * (NUMAR * NUMAR)
                if (nod->stg->dr && EsteNumar(nod->stg->dr->info)) {
                    string val1 = nod->stg->dr->info;
                    string produs = Calculeaza("*", val1, val2);

                    nod->dr = new Nod(produs);    // Actualizam dreapta
                    nod->stg = nod->stg->stg;     // Eliminam nodul intermediar
                    return Simplifica(nod);       // Recursivitate
                }

                // Subcaz A2: (NUMAR * Ceva) * NUMAR -> (NUMAR * NUMAR) * Ceva
                // Asta e ceea ce ai cerut acum!
                else if (nod->stg->stg && EsteNumar(nod->stg->stg->info)) {
                    string val1 = nod->stg->stg->info;
                    string produs = Calculeaza("*", val1, val2);

                    // Construim noul arbore: Produs * Ceva
                    Nod* nodNou = new Nod("*");
                    nodNou->stg = new Nod(produs);
                    nodNou->dr = nod->stg->dr; // Ceva-ul (ex: e^x)

                    return Simplifica(nodNou);
                }
            }
        }

        // CAZUL B: Arbore extins spre DREAPTA -> NUMAR * ( ... )
        // Ex: 4 * (4 * e^x)   SAU   4 * (e^x * 4)
        if (nod->stg && EsteNumar(nod->stg->info)) {
            if (nod->dr && nod->dr->info == "*") {
                string val1 = nod->stg->info; // Numarul din stanga de tot

                // Subcaz B1: NUMAR * (NUMAR * Ceva) -> (NUMAR * NUMAR) * Ceva
                if (nod->dr->stg && EsteNumar(nod->dr->stg->info)) {
                    string val2 = nod->dr->stg->info;
                    string produs = Calculeaza("*", val1, val2);

                    Nod* nodNou = new Nod("*");
                    nodNou->stg = new Nod(produs);
                    nodNou->dr = nod->dr->dr;
                    return Simplifica(nodNou);
                }

                // Subcaz B2: NUMAR * (Ceva * NUMAR) -> (NUMAR * NUMAR) * Ceva
                if (nod->dr->dr && EsteNumar(nod->dr->dr->info)) {
                    string val2 = nod->dr->dr->info;
                    string produs = Calculeaza("*", val1, val2);

                    Nod* nodNou = new Nod("*");
                    nodNou->stg = new Nod(produs);
                    nodNou->dr = nod->dr->stg;
                    return Simplifica(nodNou);
                }
            }
        }

        // ------TERMINARE ASOCIATIVITATE------

        Nod* bazaStg = nod->stg;
        Nod* expStg = nullptr;

        Nod* bazaDr = nod->dr;
        Nod* expDr = nullptr;

        // 1. Identificam Baza si Exponentul pentru stanga
        if (bazaStg->info == "^") {
            expStg = Copie(bazaStg->dr); // Exponent existent
            bazaStg = bazaStg->stg;      // Baza reala
        } else {
            expStg = new Nod("1");       // Exponent implicit 1
        }

        // 2. Identificam Baza si Exponentul pentru dreapta
        if (bazaDr->info == "^") {
            expDr = Copie(bazaDr->dr);
            bazaDr = bazaDr->stg;
        } else {
            expDr = new Nod("1");
        }

        // 3. Verificam daca bazele sunt identice
        if (SuntIdentice(bazaStg, bazaDr)) {
            // Construim noul exponent: expStg + expDr
            Nod* sumaExp = new Nod("+");
            sumaExp->stg = expStg;
            sumaExp->dr = expDr;

            // Simplificam suma (ex: 1+1 devine 2, sau 2+3 devine 5)
            sumaExp = Simplifica(sumaExp);

            // Construim noul nod Putere
            Nod* putereNoua = new Nod("^");
            putereNoua->stg = Copie(bazaStg); // Copiem baza comuna
            putereNoua->dr = sumaExp;

            return putereNoua;
        }
    }

    // E. ÎMPĂRȚIRE (/)
    else if (info == "/" || info == ":") {



        if (nod->stg && EsteZeroLogic(nod->stg->info)) return new Nod("0");
        if (nod->dr && EsteUnuLogic(nod->dr->info)) return nod->stg;
        if (SuntIdentice(nod->stg, nod->dr)) return new Nod("1");
    // SCOATERE COEFICIENTI DIN NUMARATOR: (K * x) / y -> K * (x / y)

    // Cazul 1: Numărătorul este o înmulțire cu o constantă (ex: -1 * x)
    if (nod->stg && nod->stg->info == "*" && EsteNumar(nod->stg->stg->info)) {
        Nod* constanta = nod->stg->stg; // K (ex: -1)
        Nod* variabila = nod->stg->dr;  // x
        Nod* numitor = nod->dr;         // y (ex: x^2)

        // Construim structura: K * (x / y)
        Nod* inmultireNoua = new Nod("*");
        inmultireNoua->stg = Copie(constanta);

        Nod* fractieNoua = new Nod("/");
        fractieNoua->stg = Copie(variabila);
        fractieNoua->dr = Copie(numitor);

        // Simplificam fractia interioara (aici se va face x / x^2 -> 1/x)
        inmultireNoua->dr = Simplifica(fractieNoua);

        // Returnam totul simplificat (va rezulta -1 * 1/x -> -1/x)
        return Simplifica(inmultireNoua);
    }

    // Cazul 2: Numărătorul este Minus Unar (ex: -x)
    if (nod->stg && nod->stg->info == "-" && nod->stg->dr == nullptr) {
        Nod* variabila = nod->stg->stg; // x-ul de sub minus
        Nod* numitor = nod->dr;

        // Transformam in: -1 * (x / numitor)
        Nod* inmultireNoua = new Nod("*");
        inmultireNoua->stg = new Nod("-1");

        Nod* fractieNoua = new Nod("/");
        fractieNoua->stg = Copie(variabila);
        fractieNoua->dr = Copie(numitor);

        inmultireNoua->dr = Simplifica(fractieNoua);
        return Simplifica(inmultireNoua);
    }

        // ---------------------------------------------------------
    // REGULA FRACTIILOR SUPRAPUSE (A / B) / (C / D)
    // ---------------------------------------------------------
    if (nod->stg && (nod->stg->info == "/" || nod->stg->info == ":") &&
        nod->dr && (nod->dr->info == "/" || nod->dr->info == ":")) {

        // Identificam: (A / B) / (C / D)
        Nod* A = nod->stg->stg;
        Nod* B = nod->stg->dr;
        Nod* C = nod->dr->stg;
        Nod* D = nod->dr->dr;

        // Construim noul numarator: A * D
        Nod* numaratorNou = new Nod("*");
        numaratorNou->stg = Copie(A);
        numaratorNou->dr = Copie(D);

        // Construim noul numitor: B * C
        Nod* numitorNou = new Nod("*");
        numitorNou->stg = Copie(B);
        numitorNou->dr = Copie(C);

        // Construim noua fractie
        Nod* fractieNoua = new Nod("/");
        fractieNoua->stg = Simplifica(numaratorNou);
        fractieNoua->dr = Simplifica(numitorNou);

        // Returnam rezultatul simplificat (va rezolva x / x^2 automat mai jos)
        return Simplifica(fractieNoua);
    }

        // Regula Puterilor: A^m / A^n
        Nod* bazaStg = nod->stg;
        Nod* expStg = nullptr;
        Nod* bazaDr = nod->dr;
        Nod* expDr = nullptr;

        // 1 / (1 / x) -> x
        if (nod->dr && (nod->dr->info == "/" || nod->dr->info == ":")) {
            // Verificam forma A / (B / C)
            if (nod->stg && EsteUnuLogic(nod->stg->info) &&       // Sus e 1
                nod->dr->stg && EsteUnuLogic(nod->dr->stg->info)) // Jos e 1/...
            {
                return Copie(nod->dr->dr); // Returnam C (adica x)
            }
        }

        // Normalizare Stânga (găsire bază și exponent)
        if (bazaStg->info == "^") {
            expStg = bazaStg->dr;
            bazaStg = bazaStg->stg;
        } else {
            expStg = new Nod("1");
        }

        // Normalizare Dreapta
        if (bazaDr->info == "^") {
            expDr = bazaDr->dr;
            bazaDr = bazaDr->stg;
        } else {
            expDr = new Nod("1");
        }

        // Dacă bazele sunt identice, aplicăm regulile
        if (SuntIdentice(bazaStg, bazaDr)) {

            bool stgE_Nr = EsteNumar(expStg->info);
            bool drE_Nr = EsteNumar(expDr->info);

            // Cazul numeric: x^2 / x^5 -> 1/x^3
            if (stgE_Nr && drE_Nr) {
                double vStg = stod(expStg->info);
                double vDr = stod(expDr->info);

                if (vDr > vStg) {
                    double dif = vDr - vStg;
                    Nod* numitorPutere = new Nod("^");
                    numitorPutere->stg = Copie(bazaStg);

                    // Formatare număr fără zecimale inutile
                    string sDif = to_string(dif);
                    if (sDif.find('.') != string::npos) {
                        sDif.erase(sDif.find_last_not_of('0') + 1, string::npos);
                        if (sDif.back() == '.') sDif.pop_back();
                    }
                    numitorPutere->dr = new Nod(sDif);

                    Nod* fractie = new Nod("/");
                    fractie->stg = new Nod("1");
                    fractie->dr = Simplifica(numitorPutere);
                    return fractie;
                }
            }

            // Cazul standard: x^5 / x^2 -> x^3  SAU  x^n / x^m
            Nod* scadereExp = new Nod("-");
            scadereExp->stg = Copie(expStg);
            scadereExp->dr = Copie(expDr);

            scadereExp = Simplifica(scadereExp);

            Nod* putereNoua = new Nod("^");
            putereNoua->stg = Copie(bazaStg);
            putereNoua->dr = scadereExp;

            return Simplifica(putereNoua);
        }

    }

    // F. PUTERE (^)
    else if (info == "^") {
        if (nod->dr && EsteZeroLogic(nod->dr->info)) return new Nod("1"); // x^0 = 1
        if (nod->dr && EsteUnuLogic(nod->dr->info)) return nod->stg;      // x^1 = x
        if (nod->stg && EsteZeroLogic(nod->stg->info)) return new Nod("0"); // 0^x = 0
        if (nod->stg && EsteUnuLogic(nod->stg->info)) return new Nod("1");  // 1^x = 1

        //Regula (A * B)^n = A^n * B^n
        if (nod->stg && nod->stg->info == "*") {
            Nod* A = nod->stg->stg; // Primul factor (ex: 2)
            Nod* B = nod->stg->dr;  // Al doilea factor (ex: x)
            Nod* n = nod->dr;       // Puterea (ex: 3)

            // Construim A^n
            Nod* term1 = new Nod("^");
            term1->stg = Copie(A);
            term1->dr = Copie(n);

            // Construim B^n
            Nod* term2 = new Nod("^");
            term2->stg = Copie(B);
            term2->dr = Copie(n);

            // Le unim prin inmultire: A^n * B^n
            Nod* rezultatNou = new Nod("*");
            rezultatNou->stg = term1;
            rezultatNou->dr = term2;

            // Apelam recursiv Simplifica pentru a calcula eventualele constante
            // (ex: 2^3 va deveni 8)
            return Simplifica(rezultatNou);
        }

        // (x^a)^b -> x^(a*b)
        if (nod->stg && nod->stg->info == "^") {
            Nod* bazaReala = nod->stg->stg;
            Nod* expA = nod->stg->dr;
            Nod* expB = nod->dr;

            // Cream noua putere: a * b
            Nod* inmultireExp = new Nod("*");
            inmultireExp->stg = Copie(expA);
            inmultireExp->dr = Copie(expB);

            // Reconstruim arborele: x ^ (noua putere)
            nod->stg = Copie(bazaReala);
            nod->dr = Simplifica(inmultireExp); // Simplificam inmultirea (ex: 2*3 -> 6)

            return Simplifica(nod); // Mai simplificam o data totul
        }




        //Regula (A / B)^n = A^n / B^n
        if (nod->stg && (nod->stg->info == "/" || nod->stg->info == ":")) {
            Nod* A = nod->stg->stg;
            Nod* B = nod->stg->dr;
            Nod* n = nod->dr;

            Nod* term1 = new Nod("^");
            term1->stg = Copie(A);
            term1->dr = Copie(n);

            Nod* term2 = new Nod("^");
            term2->stg = Copie(B);
            term2->dr = Copie(n);

            Nod* rezultatNou = new Nod("/");
            rezultatNou->stg = term1;
            rezultatNou->dr = term2;

            return Simplifica(rezultatNou);
        }

        // e^ln(x) -> x
        if (nod->stg && nod->stg->info == "e") { // baza e
            if (nod->dr && nod->dr->info == "ln") { // exponentul ln
                 return Copie(nod->dr->stg); // returnam argumentul lui ln
            }
        }
    }

    return nod;
}
// Afisare Arbore (Textual, indentat)
void Tipareste(Nod* nod, int nivel = 0) {
    if (nod) {
        Tipareste(nod->dr, nivel + 1);
        for (int i = 0; i < nivel; i++) cout << "    ";
        cout << nod->info << endl;
        Tipareste(nod->stg, nivel + 1);
    }
}

// Generare Forma Poloneza Inversa (Postfix)
void FPI(Nod* nod) {
    if (nod) {
        FPI(nod->stg);
        FPI(nod->dr);
        cout << nod->info << " ";
    }
}

void expresie_finala(Nod* nod, vector<string>& rezultat) {
    if (!nod) return;

    //  Frunza
    if (!nod->stg && !nod->dr) {
        rezultat.push_back(nod->info);
        return;
    }

    int p_curent = prioritate(nod->info[0]);

    //  Functii
    if (p_curent == 4) {
        rezultat.push_back(nod->info);
        rezultat.push_back("(");
        expresie_finala(nod->stg, rezultat);
        rezultat.push_back(")");
        return;
    }

    //  Operator Unar
    if (nod->info == "-" && nod->dr == nullptr) {
        rezultat.push_back("-");
        bool paranteze = false;
        if (este_operator(nod->stg) && prioritate(nod->stg->info[0]) < 4) paranteze = true;

        if (paranteze) rezultat.push_back("(");
        expresie_finala(nod->stg, rezultat);
        if (paranteze) rezultat.push_back(")");
        return;
    }

    //  Operatori Binari

    // Parcurgere stanga
    bool paranteze_stg = false;
    if (este_operator(nod->stg)) {
        int p_stg = prioritate(nod->stg->info[0]);
        if (p_stg < p_curent) {
            paranteze_stg = true;
        }
        // Cazul (a^b)^c. Daca avem putere in stanga si putere sus, trebuie paranteza
        else if (nod->info == "^" && p_stg == p_curent) {
            paranteze_stg = true;
        }
    }

    if (paranteze_stg) rezultat.push_back("(");
    expresie_finala(nod->stg, rezultat);
    if (paranteze_stg) rezultat.push_back(")");

    //  OPERATOR
    rezultat.push_back(nod->info);

    // Parcurgere dreapta
    bool paranteze_dr = false;
    if (este_operator(nod->dr)) {
        int p_dr = prioritate(nod->dr->info[0]);

        if (p_dr < p_curent) {
            paranteze_dr = true;
        }
        // Cazuri speciale non-asociative (-, /)
        // Ex: a - (b - c) sau a / (b * c)
        else if ((nod->info == "-" || nod->info == "/") && p_dr == p_curent) {
            paranteze_dr = true;
        }
    }

    if (paranteze_dr) rezultat.push_back("(");
    expresie_finala(nod->dr, rezultat);
    if (paranteze_dr) rezultat.push_back(")");
}

/*int main() {
    string expresie;
    string raspuns;
    cout << "Introduceti expresia: ";
    getline(cin, expresie);

    // 1. Vectorizare
    vector<string> tokeni = Vectorizeaza(expresie);

    if (!ValideazaExpresie(tokeni)) {
        cout << "Expresia este invalida. Programul se va opri." << endl;
        return 0;
    }

    // 2. Construire Arbore
    Nod* radacina = Arborizeaza(tokeni);

    cout << "\n--- Arborele Original Simplificat---\n";
    radacina = Simplifica(radacina);
    Tipareste(radacina);

    Nod* derivata;
    if(radacina) {
        cout << "FPI Original: "; FPI(radacina); cout << endl;

        // 3. Derivare
        derivata = Deriveaza(radacina);

        cout << "\n--- Arborele Derivat (Brut) ---\n";
        Tipareste(derivata);

        // 4. Simplificare
        derivata = Simplifica(derivata);

        cout << "\n--- Arborele Derivat (Simplificat) ---\n";
        Tipareste(derivata);
        cout << "FPI Derivata Simplificata: "; FPI(derivata); cout << endl;
    } else {
        cout << "Eroare la construirea arborelui." << endl;
    }
    cout << "\n--- Rezultatul derivat: ---\n";

        // 1. Declaram vectorul rezultat
        vector<string> rezultat_final;

        // 2. Apelam functia care creeaza vectorul
        expresie_finala(derivata, rezultat_final);

        // 3. Afisam continutul vectorului pentru verificare
        for (const string& s : rezultat_final) {
            cout << s;
        }
        cout << endl;



    cout<< "\n--- Vrei urmatoarea derivata din expresia rezultata? Da/Nu ---\n";



    cin>>raspuns;
    while(raspuns == "Da" || raspuns == "da")
    {
    if (!ValideazaExpresie(rezultat_final)) {
        cout << "Expresia este invalida. Programul se va opri." << endl;
        return 0;
    }
    radacina = Arborizeaza(rezultat_final);

    cout << "\n--- Arborele Original Simplificat---\n";
    radacina = Simplifica(radacina);
    Tipareste(radacina);

    if(radacina) {
        cout << "FPI Original: "; FPI(radacina); cout << endl;

        // 3. Derivare
        derivata = Deriveaza(radacina);

        cout << "\n--- Arborele Derivat (Brut) ---\n";
        Tipareste(derivata);

        // 4. Simplificare
        derivata = Simplifica(derivata);

        cout << "\n--- Arborele Derivat (Simplificat) ---\n";
        Tipareste(derivata);
        cout << "FPI Derivata Simplificata: "; FPI(derivata); cout << endl;
    } else {
        cout << "Eroare la construirea arborelui." << endl;
    }
    cout << "\n--- Rezultatul derivat: ---\n";


        rezultat_final.clear();
        // 2. Apelam functia care creeaza vectorul
        expresie_finala(derivata, rezultat_final);

        // 3. Afisam continutul vectorului pentru verificare
        for (const string& s : rezultat_final) {
            cout << s;
        }
        cout << endl;
    cout<< "\n--- Vrei urmatoarea derivata din expresia rezultata? Da/Nu ---\n";
    cin>>raspuns;
    }

        cout<<"Programul s-a finalizat.";

    return 0;
}*/


string ReconstruiesteStringDinArbore(Nod* radacina) {
    if (!radacina) return "";

    vector<string> vec;
    // Declara antetul functiei expresie_finala daca nu este vizibil sau asigura-te ca e definita mai sus
    expresie_finala(radacina, vec);

    string rezultat = "";
    for (const string& s : vec) {
        rezultat += s; // Concatenam token-urile
    }
    return rezultat;
}
string GetErrorInvalid(int limba) {
    if (limba == 0) return "Expresie Invalida!";
    if (limba == 1) return "Invalid Expression!";
    if (limba == 2) return "Expression Invalide!";
    if (limba == 3) return "Ungultiger Ausdruck!";
    return "Error";
}

string GetErrorNoTree(int limba) {
    if (limba == 0) return "Nu exista arbore de derivat!";
    if (limba == 1) return "No tree to derive!";
    if (limba == 2) return "Pas d'arbre a deriver!";
    if (limba == 3) return "Kein Baum zum Ableiten!";
    return "Error";
}

int main() {
    int latime = 1000;
    int inaltime = 700;
    initwindow(latime, inaltime, "Derivare Formala");

    InitializareButoane(latime, inaltime);
    DeseneazaInterfata(); // Va incarca bg1.jpg initial

    Nod* radacinaCurenta = nullptr;
    string expresieCurenta = "";

    bool ruleaza = true;
    while (ruleaza) {

        // 1. VERIFICARE MOUSE
        if (ismouseclick(WM_LBUTTONDOWN)) {
            int x, y;
            getmouseclick(WM_LBUTTONDOWN, x, y);

            int butonApasat = VerificaClick(x, y);

            // Buton 0: Insereaza
            if (butonApasat == 0) {
                DeseneazaInterfata();
                string inputNou = CitesteExpresieGrafic();
                DeseneazaInterfata();

                if (!inputNou.empty()) {
                    expresieCurenta = inputNou;
                    vector<string> tokeni = Vectorizeaza(expresieCurenta);
                    if (ValideazaExpresie(tokeni)) {
                        cameraX = 0; cameraY = 0;
                        radacinaCurenta = Arborizeaza(tokeni);
                        radacinaCurenta = Simplifica(radacinaCurenta);
                        radacinaCurenta = Simplifica(radacinaCurenta);
                        AfiseazaArboreGrafic(radacinaCurenta, expresieCurenta);
                    } else {
                        setcolor(TEXT_COLOR);
                        settextstyle(BOLD_FONT, HORIZ_DIR, 1);
                        setbkcolor(WHITE);
                        outtextxy(50, 200, (char*)GetErrorInvalid(limbaCurenta).c_str());
                    }
                } else {
                    if(radacinaCurenta) AfiseazaArboreGrafic(radacinaCurenta, expresieCurenta);
                }
            }
            // Buton 1: Deriveaza
            else if (butonApasat == 1) {
                if (radacinaCurenta) {
                    radacinaCurenta = Deriveaza(radacinaCurenta);
                    radacinaCurenta = Simplifica(radacinaCurenta);
                    radacinaCurenta = Simplifica(radacinaCurenta);
                    cameraX = 0; cameraY = 0;
                    expresieCurenta = ReconstruiesteStringDinArbore(radacinaCurenta);
                    AfiseazaArboreGrafic(radacinaCurenta, expresieCurenta);
                } else {
                    setcolor(TEXT_COLOR);
                    setbkcolor(WHITE);
                    outtextxy(50, 200, (char*)GetErrorNoTree(limbaCurenta).c_str());
                }
            }
            // Buton 2: Schimba Limba
            else if (butonApasat == 2) {
                limbaCurenta++;
                if (limbaCurenta > 3) limbaCurenta = 0;
                DeseneazaInterfata();
                if (radacinaCurenta) AfiseazaArboreGrafic(radacinaCurenta, expresieCurenta);
            }
            // --- BUTON NOU 3: Schimba Background ---
            else if (butonApasat == 3) {
                indexBackground++;
                if (indexBackground > 3) indexBackground = 0; // Ciclic: 0 -> 1 -> 2 -> 0

                // Redesenam interfata (aceasta va incarca noua poza)
                DeseneazaInterfata();

                // Daca avem un arbore, il redesenam peste noua poza
                if (radacinaCurenta) AfiseazaArboreGrafic(radacinaCurenta, expresieCurenta);
            }
            // Buton 4: Iesire (Mutat la index 4)
            else if (butonApasat == 4) {
                ruleaza = false;
            }
        }

        // 2. VERIFICARE TASTATURA (Scroll)
        if (kbhit()) {
            char key = getch();
            bool redraw = false;
            int step = 20;

            if (key == 0 || key == -32) {
                char code = getch();
                switch (code) {
                    case 72: cameraY += step; redraw = true; break; // SUS
                    case 80: cameraY -= step; redraw = true; break; // JOS
                    case 75: cameraX += step; redraw = true; break; // STANGA
                    case 77: cameraX -= step; redraw = true; break; // DREAPTA
                }
            }

            if (redraw && radacinaCurenta) {
                AfiseazaArboreGrafic(radacinaCurenta, expresieCurenta);
            }
        }
    }
    closegraph();
    return 0;
}
