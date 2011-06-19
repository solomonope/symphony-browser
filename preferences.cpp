#include "preferences.h"
#include "ui_preferences.h"
#include "browser.h"
#include <QSettings>
#include <QMessageBox>

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    doLoad();
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::doLoad() //Gets data from settings and sets initial data in the QDialog to what is currently set by the settings
{
    QSettings settings("data/settings.dat", QSettings::IniFormat);
    int openMode = 0;
    int adBlockEnable = 0;
    int adBlock2Enable = 0;
    int noScriptEnable = 0;
    int noScriptSecureEnable = 0;
    if (settings.contains("OpenMode"))
        openMode = settings.value("OpenMode").toInt();
    if (openMode > 1) //Invalid
        openMode = 0;
    QString homePage = "http://google.com";
    if (settings.contains("HomePage"))
        homePage = settings.value("HomePage").toString();
    if (settings.contains("AdBlockEnabled"))
        adBlockEnable = settings.value("AdBlockEnabled").toInt();
    if (settings.contains("AdBlock2Enabled"))
        adBlock2Enable = settings.value("AdBlock2Enabled").toInt();
    if (settings.contains("NoScriptEnabled"))
        noScriptEnable = settings.value("NoScriptEnabled").toInt();
    if (settings.contains("NoScriptSecureEnabled"))
        noScriptSecureEnable = settings.value("NoScriptSecureEnabled").toInt();
    if (openMode == 1) {
        ui->restoreTabsBtn->setChecked(true);
        ui->homePageBtn->setChecked(false);
    }
    else {
        ui->restoreTabsBtn->setChecked(false);
        ui->homePageBtn->setChecked(true);
    }
    if (adBlockEnable == 1)
        ui->adBlockEnabled->setChecked(true);
    else
        ui->adBlockEnabled->setChecked(false);
    if (adBlock2Enable == 1)
        ui->adBlock2Enabled->setChecked(true);
    else
        ui->adBlock2Enabled->setChecked(false);
    if (noScriptEnable == 1)
        ui->noScriptEnabled->setChecked(true);
    else
        ui->noScriptEnabled->setChecked(false);
    if (noScriptSecureEnable == 1)
        ui->noScriptSecureEnabled->setChecked(true);
    else
        ui->noScriptSecureEnabled->setChecked(false);

    ui->homePageBox->setText(homePage);
}

void Preferences::on_homePageBtn_clicked() //Handles radio button action
{
    ui->restoreTabsBtn->setChecked(false);
}

void Preferences::on_restoreTabsBtn_clicked() //Handles radio button action
{
    ui->homePageBtn->setChecked(false);
}

void Preferences::on_buttonBox_accepted() //Save and close
{
    //Saving and closing...
    QSettings settings("data/settings.dat", QSettings::IniFormat);
    if (ui->restoreTabsBtn->isChecked())
        settings.setValue("OpenMode", 1);
    else
        settings.setValue("OpenMode", 0);

    if (ui->adBlockEnabled->isChecked()) {
        settings.setValue("AdBlockEnabled", 1);
        browser *tmpWind = qobject_cast<browser*>(parent());
        bool tmpB = true;
        tmpWind->setAdBlockEnabled(&tmpB);
    }
    else {
        settings.setValue("AdBlockEnabled", 0);
        browser *tmpWind = qobject_cast<browser*>(parent());
        bool tmpB = false;
        tmpWind->setAdBlockEnabled(&tmpB);
    }

    if (ui->adBlock2Enabled->isChecked()) {
        settings.setValue("AdBlock2Enabled", 1);
        browser *tmpWind = qobject_cast<browser*>(parent());
        bool tmpB = true;
        tmpWind->setAdBlock2Enabled(&tmpB);
    }
    else {
        settings.setValue("AdBlock2Enabled", 0);
        browser *tmpWind = qobject_cast<browser*>(parent());
        bool tmpB = false;
        tmpWind->setAdBlock2Enabled(&tmpB);
    }

    if (ui->noScriptEnabled->isChecked()) {
        settings.setValue("NoScriptEnabled", 1);
        browser *tmpWind = qobject_cast<browser*>(parent());
        bool tmpB = true;
        tmpWind->setNoScriptEnabled(&tmpB);
    }
    else {
        settings.setValue("NoScriptEnabled", 0);
        browser *tmpWind = qobject_cast<browser*>(parent());
        bool tmpB = false;
        tmpWind->setNoScriptEnabled(&tmpB);
    }

    if (ui->noScriptSecureEnabled->isChecked()) {
        settings.setValue("NoScriptSecureEnabled", 1);
        browser *tmpWind = qobject_cast<browser*>(parent());
        bool tmpB = true;
        tmpWind->noScriptSecureEnabled = &tmpB;
        tmpWind->updateNoScriptSecure();
    }
    else {
        settings.setValue("NoScriptSecureEnabled", 0);
        browser *tmpWind = qobject_cast<browser*>(parent());
        bool tmpB = false;
        tmpWind->noScriptSecureEnabled = &tmpB;
        tmpWind->updateNoScriptSecure();
    }


    settings.setValue("HomePage", ui->homePageBox->text());
}
