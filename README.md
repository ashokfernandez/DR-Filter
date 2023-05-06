
    ██████╗     ██████╗             ███████╗    ██╗    ██╗         ████████╗    ███████╗    ██████╗ 
    ██╔══██╗    ██╔══██╗            ██╔════╝    ██║    ██║         ╚══██╔══╝    ██╔════╝    ██╔══██╗
    ██║  ██║    ██████╔╝            █████╗      ██║    ██║            ██║       █████╗      ██████╔╝
    ██║  ██║    ██╔══██╗            ██╔══╝      ██║    ██║            ██║       ██╔══╝      ██╔══██╗
    ██████╔╝    ██║  ██║            ██║         ██║    ███████╗       ██║       ███████╗    ██║  ██║
    ╚═════╝     ╚═╝  ╚═╝            ╚═╝         ╚═╝    ╚══════╝       ╚═╝       ╚══════╝    ╚═╝  ╚═╝
                                                                                        side effects    






## Features
- Single-knob filter, just like a DJ mixer
- Cutoff control goes from low pass on the left to high pass on the right
- Filter is completely removed from the signal when cutoff is in the middle
- Resonance control to adjust the level of the peak of the filter
- Drive control to add a warm analog sound to your audio
- Available in Audio Unit (AU) and Virtual Studio Technology (VST) formats and is compatible with macOS and probably Windows

**Note: Currently, the plugin has only been compiled in Xcode on macOS. However, it should work with JUCE on other operating systems.**

## Installation
DR Filter is an audio plugin written in C++ using JUCE v7.0.5.

To use DR Filter, you need to download and install JUCE. You can set up Xcode on macOS or Visual Studio on Windows to compile JUCE by following the instructions in the JUCE documentation. Once you have JUCE installed, build the plugin using your preferred build system or IDE, and add it to your preferred Digital Audio Workstation (DAW) or standalone audio application.

**For macOS:**

- To install an Audio Unit, copy the built plugin (.component file) to `/Library/Audio/Plug-Ins/Components` or `~/Library/Audio/Plug-Ins/Components` (for a user-level installation).
- To install a VST, copy the built plugin (.vst file) to `/Library/Audio/Plug-Ins/VST` or `~/Library/Audio/Plug-Ins/VST` (for a user-level installation).

**For Windows:**

- To install a VST, copy the built plugin (.dll file) to the appropriate VST plugins folder for your DAW (usually `C:\Program Files\Vstplugins` or `C:\Program Files (x86)\Vstplugins`).

After installation, you're ready to use DR Filter in your DAW!

## Usage
To use DR Filter, add it as an effect in your preferred Digital Audio Workstation (DAW) or standalone audio application. 

### Ableton
To use DR Filter in Ableton, follow these steps:
1. Open Ableton and create a new audio track.
2. Click on the "Audio Effects" category on the left-hand side of the screen and select "AU DR Filter" or "VST DR Filter" depending on the version you installed.
3. Drag DR Filter onto the track.
4. Adjust the filter parameters by clicking and dragging the knob controls.

### Logic Pro X
To use DR Filter in Logic Pro X, follow these steps:
1. Open Logic Pro X and create a new audio track.
2. Click on the "Mixer" button in the top right corner of the screen to show the mixer.
3. Click on the "Inserts" button on the audio track and select "AU DR Filter" or "VST DR Filter" depending on the version you installed.
4. Adjust the filter parameters by clicking and dragging the knob controls.

**Note:** These instructions are for demonstration purposes only and may vary depending on your version of Ableton or Logic Pro X.

## Testing
To test DR Filter, we recommend compiling AudioPluginHost from JUCE on your own computer. Instructions on how to do this can be found in the JUCE SDK. Once you have AudioPluginHost set up, you can test DR Filter by adding it as an effect and adjusting the filter parameters.
