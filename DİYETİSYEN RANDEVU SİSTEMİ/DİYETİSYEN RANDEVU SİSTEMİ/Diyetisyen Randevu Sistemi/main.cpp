#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <vector>
#include <ctime>
#include <sstream>
#include <algorithm>

using namespace std;

const string RANDEVULAR_DOSYASI = "randevular.txt";
const string ARSIV_DOSYASI = "silinmisler.txt";
const vector<string> STANDART_SAATLER = {"09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00"};

struct Randevu {
    string isim;
    string tarih; // GG/AA/YYYY formatýnda
    string saat;  // SS:dd formatýnda

    void ekranaYazdir() const {
        cout << left << setw(20) << isim << setw(15) << tarih << setw(10) << saat << endl;
    }
};

// Yardýmcý fonksiyonlar
bool dosyaVarMi(const string& dosyaAdi);
vector<Randevu> randevulariOku();
void randevulariYaz(const vector<Randevu>& randevular);
bool tarihGecerliMi(const string& tarih);
bool saatGecerliMi(const string& saat);
bool randevuVarMi(const vector<Randevu>& randevular, const string& tarih, const string& saat);
bool kisiAyniGunZatenVarMi(const vector<Randevu>& randevular, const string& isim, const string& tarih);
void arsiveEkle(const Randevu& randevu);
bool tarihKarsilastir(const string& tarih1, const string& tarih2);
string bugununTarihi();

// Ana fonksiyonlar
void randevuEkle();
void randevuSil();
void randevuGuncelle();
void randevuListele();
void uygunSaatOner();
void randevuAra();
void bugununRandevulari();

int main() {
    if (!dosyaVarMi(RANDEVULAR_DOSYASI)) {
        ofstream olustur(RANDEVULAR_DOSYASI);
        olustur.close();
    }

    int secim;
    do {
        cout << "\n=== DIYETISYEN RANDEVU PLANLAYICI ===\n";
        cout << "1. Randevu Ekle\n";
        cout << "2. Randevu Sil\n";
        cout << "3. Randevu Guncelle\n";
        cout << "4. Tum Randevulari Listele\n";
        cout << "5. Bugunun Randevularini Listele\n";
        cout << "6. Uygun Saat Onerisi\n";
        cout << "7. Randevu Ara (Isme gore)\n";
        cout << "8. Cikis\n";
        cout << "Seciminiz: ";
        cin >> secim;
        cin.ignore();

        switch (secim) {
            case 1: randevuEkle(); break;
            case 2: randevuSil(); break;
            case 3: randevuGuncelle(); break;
            case 4: randevuListele(); break;
            case 5: bugununRandevulari(); break;
            case 6: uygunSaatOner(); break;
            case 7: randevuAra(); break;
            case 8: cout << "Programdan cikiliyor...\n"; break;
            default: cout << "Gecersiz secim. Lutfen 1-8 arasinda bir sayi girin.\n";
        }
    } while (secim != 8);

    return 0;
}

bool dosyaVarMi(const string& dosyaAdi) {
    ifstream dosya(dosyaAdi);
    return dosya.good();
}

vector<Randevu> randevulariOku() {
    vector<Randevu> randevular;
    ifstream dosya(RANDEVULAR_DOSYASI);
    Randevu r;

    while (dosya >> r.isim >> r.tarih >> r.saat) {
        randevular.push_back(r);
    }

    return randevular;
}

void randevulariYaz(const vector<Randevu>& randevular) {
    ofstream dosya(RANDEVULAR_DOSYASI);
    for (const auto& r : randevular) {
        dosya << r.isim << " " << r.tarih << " " << r.saat << endl;
    }
}

bool tarihGecerliMi(const string& tarih) {
    if (tarih.length() != 10 || tarih[2] != '/' || tarih[5] != '/') {
        return false;
    }

    int gun = stoi(tarih.substr(0, 2));
    int ay = stoi(tarih.substr(3, 2));
    int yil = stoi(tarih.substr(6, 4));

    // Basit tarih kontrolü (daha geliþmiþ bir versiyon için harici kütüphane kullanýlabilir)
    if (gun < 1 || gun > 31 || ay < 1 || ay > 12 || yil < 1900) {
        return false;
    }

    return true;
}

bool saatGecerliMi(const string& saat) {
    if (saat.length() != 5 || saat[2] != ':') {
        return false;
    }

    int saatDegeri = stoi(saat.substr(0, 2));
    int dakika = stoi(saat.substr(3, 2));

    return saatDegeri >= 0 && saatDegeri < 24 && dakika >= 0 && dakika < 60;
}

bool randevuVarMi(const vector<Randevu>& randevular, const string& tarih, const string& saat) {
    return any_of(randevular.begin(), randevular.end(), [&](const Randevu& r) {
        return r.tarih == tarih && r.saat == saat;
    });
}

bool kisiAyniGunZatenVarMi(const vector<Randevu>& randevular, const string& isim, const string& tarih) {
    return any_of(randevular.begin(), randevular.end(), [&](const Randevu& r) {
        return r.isim == isim && r.tarih == tarih;
    });
}

void arsiveEkle(const Randevu& randevu) {
    ofstream dosya(ARSIV_DOSYASI, ios::app);
    dosya << randevu.isim << " " << randevu.tarih << " " << randevu.saat << endl;
}

string bugununTarihi() {
    time_t simdi = time(0);
    tm* zaman = localtime(&simdi);

    stringstream ss;
    ss << setw(2) << setfill('0') << zaman->tm_mday << "/"
       << setw(2) << setfill('0') << (zaman->tm_mon + 1) << "/"
       << (zaman->tm_year + 1900);

    return ss.str();
}

void randevuEkle() {
    Randevu yeniRandevu;
    cout << "Isim: ";
    getline(cin, yeniRandevu.isim);
    cout << "Tarih (GG/AA/YYYY): ";
    getline(cin, yeniRandevu.tarih);
    cout << "Saat (SS:dd): ";
    getline(cin, yeniRandevu.saat);

    if (!tarihGecerliMi(yeniRandevu.tarih)) {
        cout << "Hata: Gecersiz tarih formati!.\n";
        return;
    }

    if (!saatGecerliMi(yeniRandevu.saat)) {
        cout << "Hata: Gecersiz saat formati!.\n";
        return;
    }

    vector<Randevu> randevular = randevulariOku();

    if (randevuVarMi(randevular, yeniRandevu.tarih, yeniRandevu.saat)) {
        cout << "Hata: Bu saat zaten dolu. Lutfen baska bir saat deneyin.\n";
        return;
    }

    if (kisiAyniGunZatenVarMi(randevular, yeniRandevu.isim, yeniRandevu.tarih)) {
        cout << "Hata: Ayni kisi ayni gun sadece bir randevu alabilir.\n";
        return;
    }

    randevular.push_back(yeniRandevu);
    randevulariYaz(randevular);
    cout << "Randevu basariyla eklendi.\n";
}

void randevuListele() {
    vector<Randevu> randevular = randevulariOku();

    if (randevular.empty()) {
        cout << "Kayitli randevu bulunamadi.\n";
        return;
    }

    cout << "\n--- Tum Randevular ---\n";
    cout << left << setw(20) << "Isim" << setw(15) << "Tarih" << setw(10) << "Saat" << endl;
    cout << "------------------------------------------\n";

    for (const auto& r : randevular) {
        r.ekranaYazdir();
    }
}

void bugununRandevulari() {
    string bugun = bugununTarihi();
    vector<Randevu> randevular = randevulariOku();
    vector<Randevu> bugununRandevulari;

    copy_if(randevular.begin(), randevular.end(), back_inserter(bugununRandevulari),
        [&bugun](const Randevu& r) { return r.tarih == bugun; });

    if (bugununRandevulari.empty()) {
        cout << "Bugune ait randevu bulunamadi.\n";
        return;
    }

    cout << "\n--- Bugunun Randevulari (" << bugun << ") ---\n";
    cout << left << setw(20) << "Isim" << setw(10) << "Saat" << endl;
    cout << "------------------------------\n";

    for (const auto& r : bugununRandevulari) {
        cout << left << setw(20) << r.isim << setw(10) << r.saat << endl;
    }
}

void randevuSil() {
    string hedefTarih, hedefSaat;
    cout << "Silinecek randevunun tarihi: ";
    getline(cin, hedefTarih);
    cout << "Silinecek randevunun saati: ";
    getline(cin, hedefSaat);

    vector<Randevu> randevular = randevulariOku();
    vector<Randevu> yeniRandevular;
    bool silindi = false;

    for (const auto& r : randevular) {
        if (r.tarih == hedefTarih && r.saat == hedefSaat) {
            arsiveEkle(r);
            silindi = true;
        } else {
            yeniRandevular.push_back(r);
        }
    }

    if (silindi) {
        randevulariYaz(yeniRandevular);
        cout << "Randevu basariyla silindi.\n";
    } else {
        cout << "Hata: Belirtilen randevu bulunamadi.\n";
    }
}

void randevuGuncelle() {
    string eskiTarih, eskiSaat;
    cout << "Guncellenecek randevunun tarihi: ";
    getline(cin, eskiTarih);
    cout << "Guncellenecek randevunun saati: ";
    getline(cin, eskiSaat);

    vector<Randevu> randevular = randevulariOku();
    bool bulundu = false;

    for (auto& r : randevular) {
        if (r.tarih == eskiTarih && r.saat == eskiSaat) {
            bulundu = true;
            cout << "Yeni isim (" << r.isim << "): ";
            getline(cin, r.isim);

            string yeniTarih;
            cout << "Yeni tarih (" << r.tarih << "): ";
            getline(cin, yeniTarih);
            if (!yeniTarih.empty() && tarihGecerliMi(yeniTarih)) {
                r.tarih = yeniTarih;
            }

            string yeniSaat;
            cout << "Yeni saat (" << r.saat << "): ";
            getline(cin, yeniSaat);
            if (!yeniSaat.empty() && saatGecerliMi(yeniSaat)) {
                r.saat = yeniSaat;
            }

            break;
        }
    }

    if (bulundu) {
        randevulariYaz(randevular);
        cout << "Randevu basariyla guncellendi.\n";
    } else {
        cout << "Hata: Belirtilen randevu bulunamadi.\n";
    }
}

void uygunSaatOner() {
    string tarih;
    cout << "Tarih (GG/AA/YYYY): ";
    getline(cin, tarih);

    if (!tarihGecerliMi(tarih)) {
        cout << "Hata: Gecersiz tarih formatý!\n";
        return;
    }

    vector<Randevu> randevular = randevulariOku();
    vector<string> uygunSaatler;

    for (const auto& saat : STANDART_SAATLER) {
        if (!randevuVarMi(randevular, tarih, saat)) {
            uygunSaatler.push_back(saat);
        }
    }

    if (uygunSaatler.empty()) {
        cout << "Bu tarihte uygun saat bulunmamaktadir.\n";
    } else {
        cout << "\n--- " << tarih << " icin Uygun Saatler ---\n";
        for (const auto& saat : uygunSaatler) {
            cout << "- " << saat << endl;
        }
    }
}

void randevuAra() {
    string isim;
    cout << "Aranacak isim: ";
    getline(cin, isim);

    vector<Randevu> randevular = randevulariOku();
    vector<Randevu> bulunanRandevular;

    copy_if(randevular.begin(), randevular.end(), back_inserter(bulunanRandevular),
        [&isim](const Randevu& r) { return r.isim.find(isim) != string::npos; });

    if (bulunanRandevular.empty()) {
        cout << "Hata: '" << isim << "' ismiyle kayitli randevu bulunamadi.\n";
    } else {
        cout << "\n--- '" << isim << "' icin Bulunan Randevular ---\n";
        cout << left << setw(20) << "Isim" << setw(15) << "Tarih" << setw(10) << "Saat" << endl;
        cout << "------------------------------------------\n";

        for (const auto& r : bulunanRandevular) {
            r.ekranaYazdir();
        }
    }
}
