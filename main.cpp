#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <list>
#include <string>
#include <set>

using namespace std;

unsigned X;
unsigned Y;

struct koordinata {
    int x;
    int y;
    bool in() {return x>=0 && y>=0 && x<X && y<Y;}
    koordinata(int _x, int _y)
    {
        x=_x;
        y=_y;
    }
    koordinata(){};

};

koordinata operator+ (koordinata a, const koordinata &b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

bool operator== (const koordinata &a, const koordinata &b) {
    return a.x == b.x && a.y == b.y;
}

vector<koordinata> szomszedok(const koordinata &k)
{
    vector<koordinata> szomszed_vec;

    int nums[3] = {-1, 0, 1};
    for (int i : nums)
    {
        for (int j : nums)
        {
            koordinata sz(k.x+i, k.y+j);
            if (sz.in() and not (sz == k))
            {
                szomszed_vec.push_back(sz);
            }
        }
    }

    return szomszed_vec;
}


bool operator<(const koordinata &k, const koordinata &l){
    if(k.x != l.x){
        return k.x < l.x;
    }
    return k.y < l.y;
}


class Elem{
private:

public:
    string kategoria;
    int szint;
    void nullazo(){
        kategoria="0";
        szint =0;
    }
    bool ures_e ()
    {
        return kategoria == "0" && szint == 0;
    }

    Elem(){nullazo();};
    Elem (string _kategoria, int _szint)
    {
        kategoria = _kategoria;
        szint = _szint;
    }
};

bool operator == (const Elem & e1, const Elem &e2){
    return e1.kategoria == e2.kategoria && e1.szint == e2.szint;
}


bool operator < (const Elem &e1 , const Elem & e2){
    return e1.kategoria == e2.kategoria && e1.szint < e2.szint;
}
ostream& operator<< (ostream& o, Elem e)
{
    o<<e.kategoria<<e.szint;
    return o;
}
void kiir(vector<vector<Elem>> allapot)
{
    for ( unsigned i = 0 ; i < Y ; i++)
    {
        for ( unsigned j = 0 ; j < X ; j++)
        {
            cout << allapot[i][j]<<" ";
        }
        cout<<endl;
    }
}


class Terkep
{
private:


    vector<vector<Elem>> aktualis_allapot ;
    vector<vector<Elem>> kezdeti_allapot ;
    vector<Elem> lerakando_szekvencia;

public:

    void fajlbeolvasas(string fajlnev)
    {
        // meret, elemek beolvasasa
        // kezdeti allapot beallitasa
        ifstream file(fajlnev);


        if (file.fail())
        {
            cout <<"file.fail"<<endl;
        }

        string ideiglenes;
        string s_x;
        string s_y;
        string s_kezdeti_mezo;
        Elem mezo;
        string mezo_kategoria;
        int mezo_szint;
        vector<Elem> sor;

        getline(file, ideiglenes);
        stringstream tarolo;
        tarolo << ideiglenes;

        getline(tarolo, s_x, ',');
        stringstream ss_x;
        ss_x << s_x;
        ss_x >> X;


        getline(tarolo, s_y);
        stringstream ss_y;
        ss_y << s_y;
        ss_y >> Y;

        while(file.good())
            {
                for (unsigned i = 0 ;i < Y ; i ++)
                {
                    getline(file, ideiglenes);
                    stringstream tarolo_ketto;
                    tarolo_ketto << ideiglenes;   //itt lehet egy hiba, hogy a << nem törli az elõzetes tartalmat (szerintem törli)
                    sor.clear();

                    while (getline(tarolo_ketto, s_kezdeti_mezo, ','))
                    {
                        mezo_kategoria = s_kezdeti_mezo[0];
                        stringstream ss_mezo_szint;
                        ss_mezo_szint << s_kezdeti_mezo[1];
                        ss_mezo_szint >> mezo_szint;

                        mezo.kategoria = mezo_kategoria;
                        mezo.szint = mezo_szint;
                        sor.push_back(mezo);
                    }
                    kezdeti_allapot.push_back(sor);
                }
                getline (file, ideiglenes);
                stringstream tarolo2;
                tarolo2<<ideiglenes;
                string s_lerakando_elem;
                Elem lerakando;

                while ( getline( tarolo2, s_lerakando_elem ,','))
                {
                    lerakando.kategoria=s_lerakando_elem[0];
                    stringstream ss_szam;
                    ss_szam<<s_lerakando_elem[1];
                    int szint;
                    ss_szam>>szint;
                    lerakando.szint=szint;
                    lerakando_szekvencia.push_back(lerakando);
                }

            }
        file.close();
        aktualis_allapot = kezdeti_allapot;


    }


    vector<Elem> getSequence()
    {
        return lerakando_szekvencia;
    }
    vector<vector<Elem>> getInitialState ()
    {
        return kezdeti_allapot;
    }

};


class State {
   State *parent;
   list<koordinata> ures_mezok;
   vector<vector<Elem>> aktualis_allapot;
   vector<vector<Elem>> kezdeti_allapot;

public:
   vector<vector<Elem>>  getActualState()
   {
       return aktualis_allapot;
   }
   State (Terkep t){ //kezdeti állapot beállítása
       vector<vector<Elem>> kezdeti = t.getInitialState();
       kezdeti_allapot = kezdeti;
       aktualis_allapot = kezdeti;
       parent = nullptr;
       UresMezok();
   }
   State (State *_parent, Elem amit_lerakunk, koordinata ahova_lerakjuk){ // lépésnél állapot frissítése, mergelés
       parent = _parent;
       aktualis_allapot = parent->aktualis_allapot;
       aktualis_allapot[ahova_lerakjuk.x][ahova_lerakjuk.y] = amit_lerakunk;

       Elem frissitett = amit_lerakunk;
       Elem uj_elem = amit_lerakunk;

       do {
            set<koordinata> vizsgalt_mezok;
            set<koordinata> osszefuggo_mezok;
            frissitett = uj_elem;
            osszefuggo_keres(vizsgalt_mezok, osszefuggo_mezok, frissitett, ahova_lerakjuk);

            if( osszefuggo_mezok.size() >= 3)
            {
               int szint_emelkedes = (osszefuggo_mezok.size()-1)/2 ;
               Elem uj (frissitett.kategoria, frissitett.szint+szint_emelkedes);
                uj_elem = uj; //uj elemnél történik e szintemelkedés --> igen-> akkor nem lesz ugyanaz az ujelem, mint a frissitett
                for ( set<koordinata>:: iterator it = osszefuggo_mezok.begin(); it!= osszefuggo_mezok.end(); ++it){
                    aktualis_allapot[it->x][it->y].nullazo();
                }
                aktualis_allapot[ahova_lerakjuk.x][ahova_lerakjuk.y] = uj_elem;
            }

       } while(not(frissitett == uj_elem));
        UresMezok();
   }
   void UresMezok () // üres mezők keresése
   {
       ures_mezok.clear();

       for (int i = 0 ; i < Y ; i++)
       {
           for ( int j = 0 ; j < X ; j++)
           {
               if ( aktualis_allapot[i][j].ures_e()) {
                   koordinata koord (i, j);
                   ures_mezok.push_back(koord);
               }
           }
       }
   }

    bool jatek_vege()
    {
        return not ures_mezok.empty();
    }

    void osszefuggo_keres(set<koordinata>& vizsgalt_mezok, set<koordinata>& osszefuggo_mezok, const Elem& amit_lerakunk, const koordinata& aktualis_mezo)
    {/*1. megnézzük, hogy az aktuális mező már benne van-e a vizsgált mezőkben
        (ha igen visszatérünk, ha nem, akkor beeltesszük és folytatjuk)--> egy mező csak egyszer legyen vizsgálva
        2. az aktuális mező értéke egyenlő-e az elemmel --> ha egyenlő, akkor betesszük az összefüggő mezőkbe és folytatjuk tovább az algoritmust
        --> ha nem akkor visszatérünk
        vége: meghívjuk a felette lévő, alatta, jobbra, balra lévő szomszédra is a függvényt
    */


        for(set<koordinata>:: iterator it= vizsgalt_mezok.begin(); it != vizsgalt_mezok.end(); ++it)
        {
            if( it-> x == aktualis_mezo.x && it-> y == aktualis_mezo.y) // igy kell iteratoros elemet?
                return;

        }
        vizsgalt_mezok.insert(aktualis_mezo);

        if(not(aktualis_allapot[aktualis_mezo.x][aktualis_mezo.y]== amit_lerakunk))
        {
            return;
        }
        if(osszefuggo_mezok.size()<3) osszefuggo_mezok.insert(aktualis_mezo);

        koordinata up(aktualis_mezo.x-1, aktualis_mezo.y);
        koordinata left(aktualis_mezo.x, aktualis_mezo.y-1);
        koordinata down(aktualis_mezo.x+1, aktualis_mezo.y);
        koordinata right(aktualis_mezo.x, aktualis_mezo.y+1);

        if(up.in()){
            osszefuggo_keres(vizsgalt_mezok, osszefuggo_mezok, amit_lerakunk, up);
        }
        if(left.in()){
            osszefuggo_keres(vizsgalt_mezok, osszefuggo_mezok, amit_lerakunk, left);
        }
        if(down.in()){
            osszefuggo_keres(vizsgalt_mezok, osszefuggo_mezok, amit_lerakunk, down);
        }
        if(right.in()){
            osszefuggo_keres(vizsgalt_mezok, osszefuggo_mezok, amit_lerakunk, right);
        }
    }

    bool koordinata_szabad(koordinata k)
    {
        UresMezok();
        for (list<koordinata>::iterator it = ures_mezok.begin(); it != ures_mezok.end(); it++)
        {
            if (it->x == k.x and it->y == k.y) return true;
        }
        return false;
    }

    vector<koordinata> get_szabad_koordinatak()
    {
        UresMezok();
        vector<koordinata> koords;
        for (list<koordinata>::iterator it = ures_mezok.begin(); it != ures_mezok.end(); it++)
        {
            koords.push_back((*it));
        }
        return koords;
    }

};



bool oldalszomszedok(koordinata k1, koordinata k2){
       return((k1.x == k2.x && (abs(k1.y - k2.y)<2)) ||
             (k1.y == k2.y && (abs(k1.x - k2.x)<2)));
   }
bool atloszomszedok(koordinata k1, koordinata k2){
    return ((k1.y == k2.y+1 && k1.x == k2.x+1) ||
            (k1.y == k2.y-1 && k1.x == k2.x+1) ||
            (k1.y == k2.y+1 && k1.x ==k2.x-1) ||
            (k1.y == k2.y-1 && k1.x == k2.x-1));
}

koordinata algoritmus (State* actualstate , Elem lerakando)
{
    vector<vector<int>> pontok(Y, vector<int>(X, 0));
    vector<vector<Elem>> state = actualstate->getActualState();
    vector<koordinata> szabadok = actualstate->get_szabad_koordinatak();
    koordinata koord_with_max_score;
    int max_score = -100000;

   if (szabadok.size()==0)
    {
        koordinata k (-1,-1);
        koord_with_max_score = k;

    }
    else{
    for (koordinata szabad_koordinata : szabadok)
    {
        vector<koordinata> szomszed_vec = szomszedok(szabad_koordinata);
        int score = 0;

       // cout << "Vizsgalt: (" << szabad_koordinata.x << ", " << szabad_koordinata.y << ")" << endl;

        for (koordinata szomszed : szomszed_vec)
        {

            if(state[szomszed.y][szomszed.x].kategoria == "0")
            {
                continue;
            }

            if(state[szomszed.y][szomszed.x] == lerakando)
            {
                if (oldalszomszedok(szabad_koordinata, szomszed))
                {
                    score += 16;
                }
                else
                {
                    score += 8;
                }

            }

            if(state[szomszed.y][szomszed.x].kategoria == lerakando.kategoria and abs(state[szomszed.x][szomszed.y].szint-lerakando.szint) == 1)
            {
                if (oldalszomszedok(szabad_koordinata, szomszed))
                {
                    score -= 2;
                }
                else
                {
                    score += 2;
                }
            }

            if(state[szomszed.y][szomszed.x].kategoria == lerakando.kategoria and abs(state[szomszed.x][szomszed.y].szint-lerakando.szint) == 2)
            {
                if (oldalszomszedok(szabad_koordinata, szomszed))
                {
                    score -= 3;
                }
            }

            if(state[szomszed.y][szomszed.x].kategoria != lerakando.kategoria)
            {
                if (oldalszomszedok(szabad_koordinata, szomszed))
                {
                    score -= 5;
                }
                else
                {
                    score -= 4;
                }
            }
        }
       // cout << "Score: " << score << endl;

        if (score > max_score)
        {
            max_score = score;
            koord_with_max_score = szabad_koordinata;
        }
    }}
    return koord_with_max_score;
}



int main()
{
    Terkep t;
    string fajlnev;
    cout << "Add meg a beolvasando fajl nevet!" << endl;
    getline(cin,fajlnev);
    t.fajlbeolvasas(fajlnev);
    State * actualstate = new State (t);
    bool game = 1;
    bool automata = true;
    int aktualis_elem = 0;
    int szamlalo = 1;


//    cout << "Kezdeti allapot:" << endl;
    //kiir(actualstate->getActualState());

    while(game)
    {
        koordinata koord;
        if (not automata)
        {

            kiir(actualstate->getActualState());
            cout<< " A lerakando elem : " << t.getSequence()[aktualis_elem]<<endl;
            int x;
            int y;
            cout<< " A koordinata ahova az elemet helyezzem : ";
            // fvhívás -> koordinátát ad vissza
            cin>>x>>y;
            koord = koordinata(x,y);
            while (!actualstate->koordinata_szabad(koord))
            {
                cout<< " A koordinata mar foglalt, adjon meg masikat: ";
                cin>>x>>y;
                koord = koordinata(x,y);
            }

        }else
        {
            kiir(actualstate->getActualState());
            cout << szamlalo <<". lepes, amit lerakunk : "  << t.getSequence()[aktualis_elem] << endl;
            koord = algoritmus(actualstate, t.getSequence()[aktualis_elem]);
            /*if(!koord.in()) {
                cout << "A tabla tele van, vegallapot :"<<endl;
                break;
            }*/
        }

        State* ujState = new State(actualstate, t.getSequence()[aktualis_elem], koord );
        actualstate = ujState; // van-e meg nullas elem
        aktualis_elem = (aktualis_elem+1)%t.getSequence().size();
        game = actualstate->jatek_vege();
        szamlalo++;

    }

    kiir(actualstate->getActualState());

    return 0;
}
