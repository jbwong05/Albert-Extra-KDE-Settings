#include <QDebug>
#include <QPointer>
#include <QRegularExpression>
#include <array>
#include <stdexcept>
#include <string>
#include "albert/util/standarditem.h"
#include "xdg/iconlookup.h"
#include "albert/util/shutil.h"
#include "albert/util/standardactions.h"
#include "albert/util/standarditem.h"
#include "configwidget.h"
#include "extension.h"
using namespace Core;
using namespace std;

namespace {

    const uint NUMEXTRASETTINGS = 33;

    array<const QString, 33> itemTitles{
        {"Bluetooth",
        "Default Applications",
        "Startup Programs",
        "Time",
        "Colors",
        "Cursors",
        "Virtual Desktops",
        "Fonts",
        "Icons",
        "Destkop Theme",
        "Drivers",
        "Accounts",
        "Keyboard",
        "External display",
        "Themes",
        "Network Manager",
        "Printer Manager",
        "Splash Screen",
        "Touchpad",
        "Background Services",
        "Login/Logout Settings",
        "GTK Style",
        "Shortcuts",
        "Window Actions",
        "Window Behavior",
        "Window Switching",
        "Gestures",
        "Mouse",
        "Advanced Power Management",
        "Power Management",
        "Lock Screen Settings",
        "Widget Style",
        "Users"}
    };

    array<vector<QString>, 33> aliases{
        {{"bluetooth"},
        {"default applications", "default programs"},
        {"startup programs", "autostart", "startup applications"},
        {"clock", "time", "date"},
        {"colors"},
        {"cursors"},
        {"virtual desktops", "desktops"},
        {"fonts"},
        {"icons"},
        {"desktop theme", "shell theme"},
        {"drivers", "driver manager", "device manager"},
        {"accounts"},
        {"keyboard"},
        {"external display", "display"},
        {"themes", "look and feel"},
        {"network manager", "wifi"},
        {"printer manager", "printers"},
        {"splash screen", "loading screen"},
        {"touchpad", "mousepad"},
        {"services", "background services"},
        {"login", "logout", "desktop session"},
        {"gtk style", "gtk theme"},
        {"shortcuts", "keyboard shortcuts"},
        {"window actions"},
        {"window behavior"},
        {"window switcher"},
        {"gestures", "touch screen"},
        {"mouse"},
        {"advanced power management"},
        {"power management", "energy saving"},
        {"lock screen settings"},
        {"widget style"},
        {"users"}}
    };

    array<const QString, 33> itemDescriptions{
        {"Open bluetooth devices",
        "Open default applications",
        "Open startup programs",
        "Change date and time settings",
        "Change colors",
        "Open cursors",
        "Open virutal desktop settings",
        "Open fonts",
        "Open icons",
        "Change desktop theme",
        "Open driver manager",
        "Open accounts",
        "Change keyboard",
        "Open external display settings",
        "Change theme",
        "Open network manager",
        "Open printer manager",
        "Change splash screen",
        "Open touchpad settings",
        "Open background services",
        "Open Login/Logout settings",
        "Change gtk theme",
        "Open keyboard shortcuts",
        "Open window actions and behavior",
        "Open individual window behavior",
        "Open window switcher settings",
        "Open gesture settings",
        "Open mouse settings",
        "Open advanced power management settings",
        "Open energy saving settings",
        "Open lock screen settings",
        "Change widget style",
        "Open users"}
    };

    array<const QString, 33> iconNames{
        {"bluetooth",
        "cs-default-applications",
        "system-settings",
        "alarm-clock",
        "preferences-color",
        "system-settings",
        "preferences-system-windows",
        "fonts",
        "system-settings",
        "system-settings",
        "cs-drivers",
        "preferences-desktop-online-accounts",
        "keyboard",
        "cs-display",
        "cs-theme",
        "cs-network",
        "printer",
        "system-settings",
        "system-settings",
        "system-settings",
        "system-log-out",
        "system-settings",
        "keyboard",
        "preferences-system-windows",
        "preferences-system-windows",
        "preferences-system-windows",
        "system-settings",
        "mouse",
        "cs-power",
        "cs-power",
        "system-settings",
        "cs-theme",
        "cs-user-accounts"}
    };

    array<const QString, 33> clicommands{
        {"kcmshell5 bluedevildevices",
        "kcmshell5 filetypes",
        "kcmshell5 autostart",
        "kcmshell5 clock",
        "kcmshell5 colors",
        "kcmshell5 cursortheme",
        "kcmshell5 desktop",
        "kcmshell5 fonts",
        "kcmshell5 icons",
        "kcmshell5 kcm_desktoptheme",
        "kcmshell5 kcm_driver_manager",
        "kcmshell5 kcm_kaccounts",
        "kcmshell5 kcm_keyboard",
        "kcmshell5 kcm_kscreen",
        "kcmshell5 kcm_lookandfeel",
        "kcmshell5 kcm_networkmanagement",
        "kcmshell5 kcm_printer_manager",
        "kcmshell5 kcm_splashscreen",
        "kcmshell5 kcm_touchpad",
        "kcmshell5 kcmkded",
        "kcmshell5 kcmsmserver",
        "kcmshell5 kde-gtk-config",
        "kcmshell5 keys",
        "kcmshell5 kwinoptions",
        "kcmshell5 kwinrules",
        "kcmshell5 kwintabbox",
        "kcmshell5 kwintouchscreen",
        "kcmshell5 mouse",
        "kcmshell5 powerdevilglobalconfig",
        "kcmshell5 powerdevilprofilesconfig",
        "kcmshell5 screenlocker",
        "kcmshell5 style",
        "kcmshell5 user_manager"}
    };

}

class ExtraKdeSettings::Private {
    public:
        QPointer<ConfigWidget> widget;
        vector<QString> iconPaths;
        vector<QString> commands;
};


/** ***************************************************************************/
ExtraKdeSettings::Extension::Extension()
    : Core::Extension("org.albert.extension.extrakdesettings"),
      Core::QueryHandler(Core::Plugin::id()),
      d(new Private) {

    registerQueryHandler(this);

    // Load settings
    QString currentIconPath;
    for (size_t i = 0; i < NUMEXTRASETTINGS; ++i) {
        currentIconPath = XDG::IconLookup::iconPath(iconNames[i]);
        currentIconPath = currentIconPath == "" ? "system-settings" : currentIconPath;
        d->iconPaths.push_back(currentIconPath);
        
        d->commands.push_back(clicommands[i]);
    }
}



/** ***************************************************************************/
ExtraKdeSettings::Extension::~Extension() {

}


// Will handle user configuration of extension in albert settings menu
/** ***************************************************************************/
QWidget *ExtraKdeSettings::Extension::widget(QWidget *parent) {
    if (d->widget.isNull()) {
        d->widget = new ConfigWidget(parent);
    }
    return d->widget;
}



/** ***************************************************************************/
void ExtraKdeSettings::Extension::handleQuery(Core::Query * query) const {

    if ( query->string().isEmpty())
        return;

    // Checks for matches
    QRegularExpression re(QString("(%1)").arg(query->string()), QRegularExpression::CaseInsensitiveOption);
    for (size_t i = 0; i < NUMEXTRASETTINGS; ++i) {
        for (auto &alias : aliases[i]) {
            if ( alias.startsWith(query->string(), Qt::CaseInsensitive) ) {
                auto item = make_shared<StandardItem>(itemTitles[i]);
                item->setText(QString(itemTitles[i]).replace(re, "<u>\\1</u>"));
                item->setSubtext(itemDescriptions[i]);
                item->setIconPath(d->iconPaths[i]);
                item->addAction(make_shared<ProcAction>(itemTitles[i], QStringList(Core::ShUtil::split(d->commands[i]))));
                query->addMatch(std::move(item), static_cast<uint>(static_cast<float>(query->string().size())/itemTitles[i].size()*UINT_MAX));
                break;
            }
        }
    }
}