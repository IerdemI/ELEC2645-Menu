// funcs.c - Definitions of menu functions
    #include <stdlib.h>
    #include <stdio.h>
    #include "funcs.h"
    #include <string.h>
    #include <math.h>
    #define max_samples 2000

    static double signal_data[max_samples]; // generated samples
    static int signal_length = 0;       //how many samples are valid
    static double signal_fs = 0.0;      //sample rate (Hz)


    double SIprefix(const char *text)
    {
        double number = atof(text);
        int len = strlen(text);

        if (len == 0) return 0.0;

        // Check last character for suffix
        char last = text[len - 1];
        
        switch (last) {
            case 'k':
            case 'K':
                return number * 1000.0;
            case 'M':
                return number * 1000000.0;
            case 'm': 
                return number * 0.001;
            case 'u':  
            case 'U':
                return number * 0.000001;
            case 'n': 
            case 'N':
                return number * 0.000000001;
            default:
                return number;
        }
    }


    void menu_item_1(void)
    {
        char input[16];
        int choice = 0;
        double V, I, R;

        printf("\n[Menu 1: Ohm's Law Calculator]\n");
        printf("Ohm's Law: V = I * R\n\n");
        printf("What do you want to calculate?\n");
        printf("  1) Voltage (V)\n");
        printf("  2) Current (I)\n");
        printf("  3) Resistance (R)\n");
        printf("  b) Back to main menu\n");
        printf("Enter choice: ");

        scanf("%s", input);

        // If user pressed 'b' or 'B', go back immediately
        if (input[0] == 'b' || input[0] == 'B') {
            return;
        }

        choice = atoi(input);

        switch (choice) {

            case 1:  //calculate voltage
                printf("\nCalculate Voltage  V = I * R\n");

                printf("Enter current I (A) or 'b' to go back: ");
                scanf("%s", input);
                if (input[0] == 'b' || input[0] == 'B') return;
                I = SIprefix(input);

                printf("Enter resistance R (ohms) or 'b' to go back: ");
                scanf("%s", input);
                if (input[0] == 'b' || input[0] == 'B') return;
                R = SIprefix(input);

                V = I * R;
                printf("\nResult: V = %.3f V\n", V);
                break;

            case 2:  //calculate current
                printf("\nCalculate Current  I = V / R\n");

                printf("Enter voltage V (V) or 'b' to go back: ");
                scanf("%s", input);
                if (input[0] == 'b' || input[0] == 'B') return;
                V = SIprefix(input);

                printf("Enter resistance R (ohms) or 'b' to go back: ");
                scanf("%s", input);
                if (input[0] == 'b' || input[0] == 'B') return;
                R = SIprefix(input);

                if (R == 0) {
                    printf("Error: resistance cannot be zero.\n");
                    return;
                }

                I = V / R;
                printf("\nResult: I = %.3f A\n", I);
                break;

            case 3:  //calculate resistance
                printf("\nCalculate Resistance  R = V / I\n");

                printf("Enter voltage V (V) or 'b' to go back: ");
                scanf("%s", input);
                if (input[0] == 'b' || input[0] == 'B') return;
                V = SIprefix(input);

                printf("Enter current I (A) or 'b' to go back: ");
                scanf("%s", input);
                if (input[0] == 'b' || input[0] == 'B') return;
                I = SIprefix(input);

                if (I == 0) {
                    printf("Error: current cannot be zero.\n");
                    return;
                }

                R = V / I;
                printf("\nResult: R = %.3f ohms\n", R);
                break;

            default:
                printf("Invalid selection.\n");
                return;
        }
    }



    void menu_item_2(void)
    {
        char input[32];
        double Vin, R1, R2, Vout;

        printf("\n[Menu 2: Voltage Divider Calculator]\n");
        printf("Formula: Vout = Vin * (R2 / (R1 + R2))\n\n");

        //get Vin
        printf("Enter input voltage Vin (V) or 'b' to go back: ");
        scanf("%s", input);
        if (input[0] == 'b' || input[0] == 'B') {
            return; // back to main menu
        }
        Vin = SIprefix(input);

        // R1
        printf("Enter resistor R1 (ohms) or 'b' to go back: ");
        scanf("%s", input);
        if (input[0] == 'b' || input[0] == 'B') {
            return;
        }
        R1 = SIprefix(input);

        // R2
        printf("Enter resistor R2 (ohms) or 'b' to go back: ");
        scanf("%s", input);
        if (input[0] == 'b' || input[0] == 'B') {
            return;
        }
        R2 = SIprefix(input);

        if (R1 <= 0 || R2 <= 0) {
            printf("Error: R1 and R2 must be positive values.\n");
            return;
        }

        Vout = Vin * (R2 / (R1 + R2));

        printf("\nResult: Vout = %.3f V\n", Vout);
    }

    void menu_item_3(void)
    {
        char input[32];
        int wave_choice = 0;
        double amplitude, offset, freq, fs, duration;
        int N;
        const double PI = 3.14159265358979323846;

        printf("\n[Menu 3: Signal Generator]\n");
        printf("This will generate a waveform and store it for analysis.\n\n");

        // waveform type 
        printf("Select waveform type:\n");
        printf("  1) Sine\n");
        printf("  2) Square\n");
        printf("  3) Triangle\n");
        printf("  b) Back to main menu\n");
        printf("Enter choice: ");
        scanf("%s", input);

        if (input[0] == 'b' || input[0] == 'B') {
            return; // back to main menu
        }

        wave_choice = atoi(input);
        if (wave_choice < 1 || wave_choice > 3) {
            printf("Invalid waveform choice.\n");
            return;
        }

        // amplatude 
        printf("Enter amplitude (e.g. 2, 1.5k) or 'b' to go back: ");
        scanf("%s", input);
        if (input[0] == 'b' || input[0] == 'B') {
            return;
        }
        amplitude = SIprefix(input);

        //DC offset
        printf("Enter DC offset (e.g. 0, 1.65, -1, 500m) or 'b' to go back: ");
        scanf("%s", input);
        if (input[0] == 'b' || input[0] == 'B') {
            return;
        }
        offset = SIprefix(input);

        //frequency
        printf("Enter signal frequency (Hz, e.g. 50, 1k) or 'b' to go back: ");
        scanf("%s", input);
        if (input[0] == 'b' || input[0] == 'B') {
            return;
        }
        freq = SIprefix(input);

        if (freq <= 0) {
            printf("Frequency must be positive.\n");
            return;
        }

        //sample rate
        printf("Enter sampling frequency fs or 'b' to go back: ");
        scanf("%s", input);
        if (input[0] == 'b' || input[0] == 'B') {
            return;
        }
        fs = SIprefix(input);

        if (fs <= 0) {
            printf("Sampling frequency must be positive.\n");
            return;
        }

        if (fs < 2.0 * freq) {
            printf("Warning: fs < 2 * f (Nyquist). Signal will be aliased.\n");
        }

        //duration
        printf("Enter duration (seconds, e.g. 0.1, 10m) or 'b' to go back: ");
        scanf("%s", input);
        if (input[0] == 'b' || input[0] == 'B') {
            return;
        }
        duration = SIprefix(input);

        if (duration <= 0) {
            printf("Duration must be positive.\n");
            return;
        }

        //compute number of samples
        N = (int)(fs * duration);
        if (N <= 0) {
            printf("Error: computed sample count is zero.\n");
            signal_length = 0;
            return;
        }
        if (N > max_samples) {
            printf("Error: %d samples exceeds max_samples (%d).\n", N, max_samples);
            printf("Try a shorter duration or lower sample rate.\n");
            signal_length = 0;
            return;
        }

        //generate samples
        double dt = 1.0 / fs;

        for (int n = 0; n < N; ++n) {
            double t = n * dt;
            double sample = 0.0;

            switch (wave_choice) {
                case 1: { // sine
                    double phase = 2.0 * PI * freq * t;
                    sample = offset + amplitude * sin(phase);
                    break;
                }
                case 2: { //square
                    double phase = 2.0 * PI * freq * t;
                    double s = sin(phase);
                    sample = offset + (s >= 0 ? amplitude : -amplitude);
                    break;
                }
                case 3: { // triangle
                    // normalised phase 0..1
                    double cycles = freq * t;
                    double frac = cycles - floor(cycles); // fractional part
                    // triangle in range -1..+1
                    double tri = 4.0 * fabs(frac - 0.5) - 1.0;
                    sample = offset + amplitude * tri;
                    break;
                }
                default:
                    sample = offset;
                    break;
            }

            signal_data[n] = sample;
        }

        signal_length = N;
        signal_fs = fs;

        printf("\nGenerated %d samples at %.3f Hz.\n", signal_length, signal_fs);
        printf("You can now use Menu 4 to analyse this signal.\n");
    }

    void menu_item_4(void)
    {
        printf("\n[Menu 4: Signal Analyser]\n");

        if (signal_length <= 0) {
            printf("No signal has been generated yet.\n");
            printf("Use Menu 3 to create a signal first.\n");
            return;
        }

        int N = signal_length;
        double max = signal_data[0];
        double min = signal_data[0];
        double sum = 0.0;
        double sumSq = 0.0;

        for (int i = 0; i < N; ++i) {
            double x = signal_data[i];

            if (x > max) max = x;
            if (x < min) min = x;

            sum   += x;
            sumSq += x * x;
        }

        double mean = sum / N;
        double vrms = sqrt(sumSq / N);
        double vpp  = max - min;

        printf("\nSignal information:\n");
        printf("  Samples      : %d\n", N);
        printf("  Sample rate  : %.3f Hz\n", signal_fs);
        printf("  Vmax         : %.6f V\n", max);
        printf("  Vmin         : %.6f V\n", min);
        printf("  Vpp          : %.6f V\n", vpp);
        printf("  Vmean (DC)   : %.6f V\n", mean);
        printf("  Vrms         : %.6f V\n", vrms);
        
        
        
        //optional, export to CSV
        char answer[8];
        printf("\nDo you want to save this signal to a CSV file? (y/n): ");
        scanf("%7s", answer);

        if (answer[0] == 'y' || answer[0] == 'Y') {
            char filename[64];

            printf("Enter filename (e.g. signal.csv): ");
            scanf("%63s", filename);

            FILE *fp = fopen(filename, "w");
            if (!fp) {
                printf("Could not open file '%s' for writing.\n", filename);
                return;
            }

            //write header
            fprintf(fp, "time_s,voltage_V\n");

            //each sample: time = n / fs, value = signal_data n
            for (int i = 0; i < N; ++i) {
                double t = (signal_fs > 0.0) ? (i / signal_fs) : 0.0;
                fprintf(fp, "%.9f,%.9f\n", t, signal_data[i]);
            }

            fclose(fp);
            printf("Signal saved to '%s'. You can open this file in Excel.\n", filename);
        }
    }