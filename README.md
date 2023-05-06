
    ██████╗     ██████╗             ███████╗    ██╗    ██╗         ████████╗    ███████╗    ██████╗ 
    ██╔══██╗    ██╔══██╗            ██╔════╝    ██║    ██║         ╚══██╔══╝    ██╔════╝    ██╔══██╗
    ██║  ██║    ██████╔╝            █████╗      ██║    ██║            ██║       █████╗      ██████╔╝
    ██║  ██║    ██╔══██╗            ██╔══╝      ██║    ██║            ██║       ██╔══╝      ██╔══██╗
    ██████╔╝    ██║  ██║            ██║         ██║    ███████╗       ██║       ███████╗    ██║  ██║
    ╚═════╝     ╚═╝  ╚═╝            ╚═╝         ╚═╝    ╚══════╝       ╚═╝       ╚══════╝    ╚═╝  ╚═╝
                                                                                        side effects    


DR Filter is an audio plugin written in C++ using JUCE v7.0.5.

**Note: Currently, the plugin has only been compiled in Xcode on macOS. However, it should work with JUCE on other operating systems.**

## Features

- Cutoff control which goes from low pass on the left to high pass on the right, with the filter completely removed from the signal in the middle
- Resonance control which controls the level of the peak of the filter
- Drive control which controls the amount of saturation applied. The saturation has a warm analog sound
- Low-latency processing for use in real-time audio applications
- Available in Audio Unit (AU) and Virtual Studio Technology (VST) formats
- Cross-platform compatibility (Windows, macOS, Linux)

## Installation

To install DR Filter, follow these steps:

1. Download and install [JUCE v7.0.5](https://juce.com/).
2. Build the plugin using your preferred build system or IDE.
3. Add the built plugin to your preferred Digital Audio Workstation (DAW) or standalone audio application.

**For macOS:**

- To install an Audio Unit, copy the built plugin (.component file) to `/Library/Audio/Plug-Ins/Components` or `~/Library/Audio/Plug-Ins/Components` (for a user-level installation).
- To install a VST, copy the built plugin (.vst file) to `/Library/Audio/Plug-Ins/VST` or `~/Library/Audio/Plug-Ins/VST` (for a user-level installation).

**For Windows:**

- To install an Audio Unit, copy the built plugin (.dll file) to the appropriate VST plugins folder for your DAW (usually `C:\Program Files\Vstplugins` or `C:\Program Files (x86)\Vstplugins`).
- To install a VST, copy the built plugin (.dll file) to the appropriate VST plugins folder for your DAW (usually `C:\Program Files\Vstplugins` or `C:\Program Files (x86)\Vstplugins`).

[Insert additional installation instructions here, if needed]

## Usage

To use DR Filter, add it as an effect in your preferred Digital Audio Workstation (DAW) or standalone audio application. 

**For macOS:**

- To add an Audio Unit, open the DAW's plugin manager, locate DR Filter in the list of installed plugins, and click "Enable."
- To add a VST, open the DAW's plugin manager, locate the VST plugins folder, and scan for new plugins. DR Filter should appear in the list of available plugins.

**For Windows:**

- To add an Audio Unit, most DAWs do not support Audio Units on Windows. Use the VST version instead.
- To add a VST, open the DAW's plugin manager, locate the VST plugins folder, and scan for new plugins. DR Filter should appear in the list of available plugins.

[Insert additional usage instructions or examples here, if needed]

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).

Is there anything else you would like to add to the README?
