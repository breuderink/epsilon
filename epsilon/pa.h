
// Passive-aggressive parameters, see [1].
typedef struct {
	float C;   // Perform aggressive updates with high C.
	float eps; // Size of insensitive band for regression.
} PA_t;

/*
# References


[1] Crammer, K. et al. “Online Passive-Aggressive Algorithms.” J. Mach. Learn.
    Res. (2003).
*/