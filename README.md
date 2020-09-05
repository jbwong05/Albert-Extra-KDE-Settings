# Albert Extra KDE Settings

## Description
QPlugin for the [Albert Launcher](https://albertlauncher.github.io/) that exposes all kcm settings modules to [Albert](https://albertlauncher.github.io/). The specific modules can be listed by running `kcmshell5 --list`. The previous version of this plugin only exposed certain modules to [Albert](https://albertlauncher.github.io/) and relied on hardcoding for its functionality and customization. If you wish to use the old version, it can still be found on a separate branch [here](https://github.com/jbwong05/Albert-Extra-KDE-Settings/tree/hardcoded_1.0). This new version exposes all kcm modules to [Albert](https://albertlauncher.github.io/) without any hardcoding. All customizations can now be done using the extension's config page found under the Extensions tab of the Albert settings.

## Dependencies
* KF5Service

This plugin also shares the same dependencies that are needed to build [Albert](https://albertlauncher.github.io/) from sources. Information about building [Albert](https://albertlauncher.github.io/) from sources and its dependencies can be found [here](https://albertlauncher.github.io/docs/installing/).

## Installation
```
git clone --recursive https://github.com/albertlauncher/albert.git
cd albert/plugins
git submodule add https://github.com/jbwong05/Albert-Extra-KDE-Settings.git
./Albert-Extra-KDE-Settings/updateCMakeLists.sh
cd ..
mkdir albert-build
cd albert-build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

## Uninstallation
```
sudo rm -f /usr/lib/albert/plugins/libextrakdesettings.so
```