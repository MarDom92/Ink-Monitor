#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //tworzy akcje menu kontekstowego i ikonę w obszarze powiadomień
    stworzAkcjeMenuTray();

    //tworzy i wyświetla ikonę w obszarze powiadamiania
    stworzTrayIcon();
    trayIcon->show();

    //tworzy proces odpowiedzialny za uruchomienie zewnętrznego polecenia
    proces = new QProcess(this);

    wczytajUstawienia();
}

MainWindow::~MainWindow()
{
    zapiszUstawienia();

    delete ui;
}

//tworzy akcje dla menu kontekstowego obszaru powiadamiania
void MainWindow::stworzAkcjeMenuTray()
{
    sprawdzAction = new QAction("Sprawdź", this);
    connect(sprawdzAction, SIGNAL(triggered()), this, SLOT(on_poziomTuszuButton_clicked()));

    wyjscieAction = new QAction("Wyjście", this);
    connect(wyjscieAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

//tworzy ikonę w obszarze powiadamiania
void MainWindow::stworzTrayIcon()
{
    //sprawdza czy obszar powiadamiania jest dostępny
    if (QSystemTrayIcon::isSystemTrayAvailable() == false)
    {
        QMessageBox::critical(this,"Uwaga","Obszar powiadamiania jest niedostępny.");
        QApplication::exit();
    }

    //tworzy menu obszaru powiadamiania i dodaje odpowiednie akcje
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(sprawdzAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(wyjscieAction);

    //tworzy ikonę obszaru powiadamiania
    trayIcon = new QSystemTrayIcon(this);
    QIcon ikona(":/icon/trayicon.png");

    //przypisuje ikonę i menu do obszaru powiadamiania
    trayIcon->setIcon(ikona);
    trayIcon->setContextMenu(trayIconMenu);
}

//pokazuje wiadomość w dymku w obszarze powiadamiania
void MainWindow::wyswietlWiadomoscTray(QString tytul, QString informacja)
{
    trayIcon->showMessage(tytul, informacja);
}

//sprawdza poziom tuszu wykorzystując zewnętrzny program konsolowy 'ink'
void MainWindow::sprawdzPoziomTuszu()
{
    //polecenie uruchomienia zewnętrznego programu konsolowego w powłoce - do sprawdzenia poziomu tuszu
    options << "-c" << "ink -p usb";
    //uruchomieenie powłoki
    proces->start("/bin/sh", options);
    //czekanie do zakończenia procesu
    proces->waitForFinished(-1);
    //string, który zawiera w sobie dane z wykonanego programu w powłoce
    wynikProcesu = proces->readAllStandardOutput();
}

void MainWindow::wyswietlPoziomTuszu()
{
    //znalezienie pozycji pierwszego znaku '%' w stringu wynikProcesu
    int procent = wynikProcesu.indexOf("%");
    //wyodrębnienie wartości poziomu tuszu ze stringu i wyświetlenie tej wartości
    poziomCyan = wynikProcesu.mid(procent-3, 3);
    ui->cyanlineEdit->setText(poziomCyan);
    //ucięcie początku stringu ze znalezionym znakiem '%' za tym znakiem, aby ponownie później go przeszukać
    wynikProcesu = wynikProcesu.right(wynikProcesu.size()-procent-1);
    //konwersja wartości stringu na wartość liczbową i wyświetlenie jej na pasku postępu
    wartoscCyan = poziomCyan.toInt();
    ui->cyanBar->setValue(wartoscCyan);

    //analogicznie jak wyżej
    procent = wynikProcesu.indexOf("%");
    poziomMagenta = wynikProcesu.mid(procent-3, 3);
    ui->magentalineEdit->setText(poziomMagenta);
    wynikProcesu = wynikProcesu.right(wynikProcesu.size()-procent-1);
    wartoscMagenta = poziomMagenta.toInt();
    ui->magentaBar->setValue(wartoscMagenta);

    //analogicznie jak wyżej
    procent = wynikProcesu.indexOf("%");
    poziomYellow = wynikProcesu.mid(procent-3, 3);
    ui->yellowlineEdit->setText(poziomYellow);
    wynikProcesu = wynikProcesu.right(wynikProcesu.size()-procent-1);
    wartoscYellow = poziomYellow.toInt();
    ui->yellowBar->setValue(wartoscYellow);

    //analogicznie jak wyżej
    procent = wynikProcesu.indexOf("%");
    poziomBlack = wynikProcesu.mid(procent-3, 3);
    ui->blacklineEdit->setText(poziomBlack);
    wartoscBlack = poziomBlack.toInt();
    ui->blackBar->setValue(wartoscBlack);
}

//zapisuje ustawienia do pliku
void MainWindow::zapiszUstawienia()
{
    //utworzenie odwołania do ustawień systemowych (nazwa firmy, nazwa programu)
    QSettings ustawSystemowe("MD", "Poziom tuszu");

    //grupa ustawień mainwindow
    ustawSystemowe.beginGroup("mainwindow");

    //poszczególne opcje do zapisania w ustawieniach systemowych
    ustawSystemowe.setValue("geometria", saveGeometry());
    ustawSystemowe.setValue("stan", saveState());
    ustawSystemowe.setValue("maksymalizacja", isMaximized());

    //w przypadku maksymalizacji okna
    if (!isMaximized())
    {
        ustawSystemowe.setValue("pozycja", pos());
        ustawSystemowe.setValue("rozmiar", size());
    }

    //koniec grupy mainwindow
    ustawSystemowe.endGroup();
}

//wczytuje ustawienia z pliku
void MainWindow::wczytajUstawienia()
{
    //utworzenie odwołania do ustawień systemowych (nazwa firmy, nazwa programu)
    QSettings ustawSystemowe("MD", "Poziom tuszu");

    //grupa ustawień mainwindow
    ustawSystemowe.beginGroup("mainwindow");

    //poszczególne opcje do wczytania z ustawień systemowych
    restoreGeometry(ustawSystemowe.value("geometria", saveGeometry()).toByteArray());
    restoreState(ustawSystemowe.value("stan", saveState()).toByteArray());
    move(ustawSystemowe.value("pozycja", pos()).toPoint());
    resize(ustawSystemowe.value("rozmiar", size()).toSize());

    //w przypadku maksymalizacji okna
    if (ustawSystemowe.value("maksymalizacja", isMaximized()).toBool())
        showMaximized();

    //koniec grupy mainwindow
    ustawSystemowe.endGroup();
}

void MainWindow::on_poziomTuszuButton_clicked()
{
    ui->poziomTuszuButton->setEnabled(false);
    sprawdzPoziomTuszu();

    if (wynikProcesu == "Could not get device id.\nCould not get ink level.\n")
    {
        QMessageBox::critical(this, "Brak drukarki", "Nie znaleziono drukarki.\nSprawdź czy jest włączona.");
    }
    else
    {
        do
        {
            sprawdzPoziomTuszu();
            wyswietlPoziomTuszu();
        } while ((wartoscCyan == 0) && (wartoscMagenta == 0) && (wartoscYellow == 0) && (wartoscBlack == 0));

        if(wartoscCyan < 10 || wartoscMagenta < 10 || wartoscYellow < 10 || wartoscBlack < 10)
            wyswietlWiadomoscTray("Kończy się tusz", "Wartość przynajmniej jednego z tuszy spadła poniżej 10 %.");

        if(wartoscCyan == 0 || wartoscMagenta == 0 || wartoscYellow == 0 || wartoscBlack == 0)
            wyswietlWiadomoscTray("Koniec tuszu", "Wartość przynajmniej jednego z tuszy wynosi 0%.");
    }

    ui->poziomTuszuButton->setEnabled(true);
}
