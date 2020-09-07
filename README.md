# Albert Extra KDE Settings

## Description
QPlugin for the [Albert Launcher](https://albertlauncher.github.io/) that exposes certain kcm settings modules to [Albert](https://albertlauncher.github.io/). The specific modules can be found by examining `src/extension.cpp`.

## Adding KCM Modules
Only the KCM settings modules that I use most frequently have been exposed to [Albert](https://albertlauncher.github.io/) through this extension. The specific modules can be found by examining `src/extension.cpp`. However, exposing other KCM settings modules is trivial. A full list of KCM settings modules can be obtained with the following:
```
kcmshell5 --list
```
Additional modules can be added by modifying the contents of the namespace declared at the beginning of `src/extension.cpp`.

## Dependencies
This plugin shares the same dependencies that are needed to build [Albert](https://albertlauncher.github.io/) from sources. Information about building [Albert](https://albertlauncher.github.io/) from sources and its dependencies can be found [here](https://albertlauncher.github.io/docs/installing/).

## Installation

### Option 1: Compile alongside Albert
```
git clone --recursive https://github.com/albertlauncher/albert.git
cd albert/plugins
git clone --single-branch --branch hardcoded_1.0 https://github.com/jbwong05/Albert-Extra-KDE-Settings.git
./Albert-Extra-KDE-Settings/updateCMakeLists.sh
cd ..
mkdir albert-build
cd albert-build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

### Option 2: Compile separately from Albert
```
git clone --single-branch --branch hardcoded_1.0 https://github.com/jbwong05/Albert-Extra-KDE-Settings.git
cd Albert-Extra-KDE-Settings
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_SEPARATELY=ON ..
make
sudo make install
```

## Uninstallation
```
sudo rm -f /usr/lib/albert/plugins/libextrakdesettings.so
```