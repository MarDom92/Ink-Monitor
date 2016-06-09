#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QProcess>                             //utworzenie procesu do uruchomienia zewnętrznego programu (komendy w terminalu)
#include <QMessageBox>
#include <QDialog>                              //nowe okno
#include <QSystemTrayIcon>                      //obszar powiadamiania
#include <QMenu>                                //menu kontekstowe
#include <QTextStream>                          //obsluga strumienia tekstowego
#include <QSettings>                            //ustawienia aplikacji

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

//metody
    void stworzAkcjeMenuTray();

    void sprawdzPoziomTuszu();
    void wyswietlPoziomTuszu();

    void zapiszUstawienia();
    void wczytajUstawienia();

private slots:
    void stworzTrayIcon();
    void wyswietlWiadomoscTray(QString tytul, QString informacja);

    void on_poziomTuszuButton_clicked();

private:
    Ui::MainWindow *ui;

    //zmienne określające poziom tuszu dla poszczególnych kolorów
    int wartoscCyan;
    int wartoscMagenta;
    int wartoscYellow;
    int wartoscBlack;

    //utworzenie ikony w obszarze powiadomień
    QSystemTrayIcon *trayIcon;
    //utworzenie menu obszaru powiadomień
    QMenu *trayIconMenu;

    //utworzenie akcji dla menu kontekstowego obszaru powiadamiania
    QAction *sprawdzAction;
    QAction *wyjscieAction;

    //utworzenie procesu dla polecenia systemowego
    QProcess *proces;

    //utworzenie listy stringów jako parametry do procesu uruchomienia powłoki sh
    QStringList options;
    QString wynikProcesu;

    QString poziomCyan;
    QString poziomMagenta;
    QString poziomYellow;
    QString poziomBlack;
};

#endif // MAINWINDOW_H
