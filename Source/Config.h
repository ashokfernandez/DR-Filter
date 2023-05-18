/*
  ==============================================================================

    Config.h
    Created: 18 May 2023 4:47:53am
    Author:  Ashok Fernandez

  ==============================================================================
*/

#pragma once

// Max and min hearing frequencies
#define MAX_FREQ 20000.0f
#define MIN_FREQ 20.0f

// How many values on either side of zero the filter will stay disabled for
#define FILTER_DEAD_ZONE 10.0f

// Min and max frequencies for filter range
#define LOWPASS_CUTOFF_MIN 20.0f
#define LOWPASS_CUTOFF_MAX 9500.0f
#define HIGHPASS_CUTOFF_MIN 20.0f
#define HIGHPASS_CUTOFF_MAX 5000.0f

// The cutoff control needs to map to a cutoff frequency in Hz, but this isn't a linear relationship
// The skew midpoint is the frequency the cutoff should should point to when the cutoff knob is at halfway
// The midpoint will determine how skewed the mapping is
#define HIGHPASS_CUTOFF_SKEW_MIDPOINT 500.0f
#define LOWPASS_CUTOFF_SKEW_MIDPOINT 1000.0f

// Step size inside the normalised cutoff value range
#define NORMALISED_INTERVAL 0.1f

// Minimum and maximum Q values for the filter, starting at 0.707 (butterworth) up to spikey
#define FILTER_Q_MIN 0.707f
#define FILTER_Q_MAX 8.0f