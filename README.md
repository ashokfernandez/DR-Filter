
    ██████╗     ██████╗          ███████╗   ██╗    ██╗      ████████╗   ███████╗    ██████╗ 
    ██╔══██╗    ██╔══██╗         ██╔════╝   ██║    ██║      ╚══██╔══╝   ██╔════╝    ██╔══██╗
    ██║  ██║    ██████╔╝         █████╗     ██║    ██║         ██║      █████╗      ██████╔╝
    ██║  ██║    ██╔══██╗         ██╔══╝     ██║    ██          ██║      ██╔══╝      ██╔══██╗
    ██████╔╝    ██║  ██║         ██║        ██║    ███████╗    ██║      ███████╗    ██║  ██║
    ╚═════╝     ╚═╝  ╚═╝         ╚═╝        ╚═╝    ╚══════╝    ╚═╝      ╚══════╝    ╚═╝  ╚═╝
                                                                                side effects 



## Features
- Single-knob filter, just like a DJ mixer
- Cutoff control goes from low pass on the left to high pass on the right
- Filter is completely removed from the signal when cutoff is in the middle
- Resonance control to adjust the level of the peak of the filter
- Drive control to add a warm analog sound to your audio
- Available in AU and VST formats and is compatible with macOS and Windows\*

### [Download Latest Versions of VST3 / AU](https://github.com/ashokfernandez/DR-Filter/releases)

**\*Note: Currently, the plugin has only been tested with [Xcode](https://developer.apple.com/xcode/) on macOS. The builds for Windows are automated and haven't been hand tested**

## Installation and Usage
To use DR Filter, you can install it by copying the files to the correct location on your computer

| OS        | Type   | Location*                                         |
|-----------|--------|--------------------------------------------------|
| **MacOS**     | VST 3  | User: `~/Library/Audio/Plug-Ins/VST3`            |
|           |        | System: `Macintosh HD/Library/Audio/Plug-Ins/VST3`|
|           | AU     | User: `~/Library/Audio/Plug-Ins/Components`      |
|           |        | System: `Macintosh HD/Library/Audio/Plug-Ins/Components`|
| **Windows**   | VST3   | `C:\Program Files\Common Files\VST3`                |

**\*Note: System will install for all users of the computer, User will install for an individual user**

### [Ableton](https://www.ableton.com/)
To use DR Filter in Ableton, follow these steps:
1. Open Ableton and create a new audio track.
2. Click on the "Audio Effects" category on the left-hand side of the screen and select "DR Filter (AU)" or "DR Filter (VST)" depending on the version you installed.
3. Drag DR Filter onto the track.
4. Adjust the filter parameters by clicking and dragging the knob controls.

### [Logic Pro X](https://www.apple.com/logic-pro/)
To use DR Filter in Logic Pro X, follow these steps:
1. Open Logic Pro X and create a new audio track.
2. Click on the "Mixer" button in the top right corner of the screen to show the mixer.
3. Click on the "Inserts" button on the audio track and select "DR Filter (AU)" or "DR Filter (VST)" depending on the version

## Building Code
DR Filter is an audio plugin written in C++ using JUCE v7.0.5.

To use DR Filter, you need to download and install JUCE. You can set up Xcode on macOS or [Visual Studio](https://visualstudio.microsoft.com/) on Windows to compile JUCE by following the instructions in the JUCE documentation. Once you have JUCE installed, build the plugin using your preferred build system or IDE, and add it to your preferred Digital Audio Workstation (DAW) or standalone audio application.

**For macOS:**

- To install an AU, copy the built plugin (.component file) to `/Library/Audio/Plug-Ins/Components` or `~/Library/Audio/Plug-Ins/Components` (for a user-level installation).
- To install a VST, copy the built plugin (.vst file) to `/Library/Audio/Plug-Ins/VST` or `~/Library/Audio/Plug-Ins/VST` (for a user-level installation).

**For Windows:**

- To install a VST, copy the built plugin (.dll file) to the appropriate VST plugins folder for your DAW (usually `C:\Program Files\Vstplugins` or `C:\Program Files (x86)\Vstplugins`).

After installation, you're ready to use DR Filter in your DAW!

## Testing / Development Flow
1. Find AudioPluginHost in the JUCE SDK.
2. Open AudioPluginHost's Projucer file and compile it.
3. Add DR Filter as an effect in AudioPluginHost.
4. Adjust the filter parameters by clicking and dragging the knob controls.
5. Change the post compile settings in DR Filter.projucer to point to your new AudioPluginHost executable. 
6. Compile, AudioPluginHost opens, play around with your build
6. Quit AudioPluginHost, tweak code and recompile

## Releasing New Versions

To build a new release of the pluigin, execute the `release.sh` script. This script simplifies the version incrementation process and triggers the automated build process via GitHub Actions.

```bash
./release.sh
```

The script will prompt you to choose which version segment to increment (major, minor, or patch), and will then create and push a new git tag accordingly.

This action will trigger the GitHub Actions to start the automated build process. Binaries will be built for the following platforms and formats:

- Windows: VST3
- MacOS: VST3, AU

Upon successful completion of the build process, the produced binaries will be available as artifacts on the [GitHub Actions build page](https://github.com/ashokfernandez/DR-Filter/actions). From there, a new release can be created such that it appears on the [releases page](https://github.com/ashokfernandez/DR-Filter/releases). 

**Note**: This operation requires push access to the repository.

## License

This project is licensed under the terms of the GNU General Public License version 3 (GPLv3).

JUCE, the underlying framework used in this project, is also available under the GPLv3 license. This means that you are free to use, modify, and distribute both the project and JUCE, provided that you adhere to the terms and conditions of the GPLv3 license. If you make any modifications or additions to JUCE, those modifications must also be made available under the GPLv3 license.

If you intend to use JUCE for commercial purposes or create proprietary software with JUCE, you should review the commercial licenses available on the [JUCE website](https://juce.com/get-juce/).

