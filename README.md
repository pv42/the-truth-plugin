# The Truth Plugin

## Installation
Make sure arcdps is installed. If arcdps is not installed, this plugin is simply not loaded and does nothing.  
Download the latest version from [github releases](https://github.com/pv42/the-truth-plugin/releases/latest).  
Then put the .dll file into the same folder as arcdps (normally `<Guild Wars 2>/bin64`).  
To disable, just remove the .dll or move it to a different folder.
If you use arcdps plugin manager install into `<Guild Wars 2>/addons/arcdps`.

## Usage
Set up your own name:
- open arcdps settings (shift+alt+t by default)
- click on the truth and then settings
- in the settings window set the "own name" field to the name that is used for you in the google sheet  

Visibility
- by default a compleate roles roster is displayed in the Lions Arch Aerodrome, this can be toggled with shift+alt+r
- your own roles are also displayed on raid day in the raid wing itself, this can be changed in the settings
 

## Contact
For any errors, feature requests and questions, simply open a new issue here or write pv42#6061 on discord.

## Developer notes
requires cpr, best installed using vcpkg:
```git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install cpr:x64-windows-static-md
```
Otherwise a google sheets api key is required place it into api_key.h like this:
```
#define GOOGLE_API_KEY "1234567890-abc-defghijklmnopqrst"
```
Build using Microsoft Visual Studio 2019

## LICENSE

This project is licensed with the MIT License.

### Dear ImGui
[Dear ImGui](https://github.com/ocornut/imgui) is also licensed with the MIT License and included as git submodule to this project.

### json
[json](https://github.com/nlohmann/json) is a json library created by nlohmann and licensed with the MIT License. It is included into this project with a single file [json.hpp](/json.hpp).

### cpr
[cpr](https://github.com/whoshuu/cpr) is a http library, that is licensed with the MIT License. It is included with vcpkg and itself has dependencies.
