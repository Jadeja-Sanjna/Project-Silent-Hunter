# Project-Silent-Hunter: Naval Submarine Sonar & Torpedo Fire-Control System
Project Silent Hunter is a tactical C-based naval fire-control system that processes raw hydrophone frequencies to identify vessel classes, compute real-time Doppler shift velocities and threat levels, and generate precise torpedo intercept trajectories utilizing advanced dynamic memory and binary files.

# Executive Summary
Project Silent Hunter is a tactical C-based naval fire-control system that processes raw hydrophone frequencies to identify vessel classes, compute real-time Doppler shift velocities and threat levels, and generate precise torpedo intercept trajectories. Operating deep beneath the ocean in complete darkness, modern attack submarines rely on passive sonar acoustics to detect and track contact signatures without compromising their covert position. This software emulates a submarine central combat intelligence computer, executing real-time signal classification, Doppler velocity estimation, dynamic heap tracking, and binary data persistence.

# 1. Main System Tactical Menu
The interactive menu allows operators to scan acoustic frequencies, inspect active rosters, calculate fire solutions, and save records.

# 2. Acoustic Frequency Scanning & Target Signature Identification
Entering raw hydrophone frequency, bearing angle, and distance range. The system matches the signal against known acoustic profiles and assesses threat levels.

# 3. Tactical Sonar Contact Roster Display
Formatted tabular output showing tracked contact IDs, matched vessel classifications, detected frequencies, Doppler shifts, nautical ranges, and threat status.

# 4. Torpedo Fire-Control Solution Generator
Target selection generates actionable intercept solutions including recommended torpedo lead bearings and estimated seconds to impact.

# 5. Binary Database Auto-Persistence Logging
Persistent loading and saving logs showing direct memory serialization to disk.

# Core System Features: 
1.Passive Sonar Signature Engine: Matches raw detected sound frequencies (Hz) against a stored profile database using pointer arithmetic and absolute variance math (fabs). Categorizes targets into military, commercial, research, or unknown categories based on tolerance thresholds.

2.Real-Time Doppler Threat Analysis: Calculates frequency delta. Classifies targets into CRITICAL (Closing Hostile), MEDIUM (Receding Hostile), or LOW (Neutral Vessel) threat levels.

3.Torpedo Intercept Fire-Control Computer: Computes intercept run times in seconds based on target nautical distance and torpedo knots velocity. Applies Doppler lead-angle adjustments and normalizes bearings within 0.0degrees - 360.0degrees using trigonometric modulo math (fmod).

4.Dynamic Heap Memory Management: Initializes tracking array dynamically on the heap (malloc). Automatically doubles array capacity using safe temporary allocation (realloc) as target capacity fills up.

5.Binary Record Serialization (fwrite / fread): Automatically saves active state structures directly to sonar_database.bin on exit or entry. 
Auto-restores existing records on system initialization.

# Syllabus Compliance & System Constraints
Permitted Feature Used - Syllabus Implementation Detail

Control Flow & Parsing - Bounded input loops using fgets(), atoi(), and atof() to prevent scanf crashes.

Pointers & Pointer Offsets - Pointer arithmetic (*(lib + i)), dereferencing, and double pointers (struct SonarContact **).

Structures - Nested typed data (struct AcousticSignature, struct SonarContact).

Dynamic Memory - Heap allocation via malloc(), realloc(), and pointer-nullifying free().

File I/O - Direct binary record streaming via fwrite() and fread().

Mathematics (math.h) - Precision floating-point operations using fabs() and fmod().

# Defensive Engineering & Edge Cases

Buffer Overflow Prevention: Replaced standard scanf("%s") with explicit length-bounded fgets() string processing.

Infinite Loop Safeguards: Inputs are ingested into text buffers prior to numeric parsing (atoi/atof), preventing infinite input loops caused by trailing characters.

Dangling Pointer Defense: Deallocation functions accept double pointers (**) to directly set freed pointers back to NULL in the calling function stack.

RAM Allocation Verification: Every allocation call (malloc/realloc) validates returned pointer addresses against NULL prior to dereferencing.

Missing Database Handling: File read functions handle missing or uncreated files cleanly without aborting system execution.

# Screenshots for reference:
<img width="1557" height="865" alt="Screenshot 2026-10-30 234245" src="https://github.com/user-attachments/assets/47c8ec00-dfcc-40c4-859a-e49edbf7cd64" />
<img width="1581" height="838" alt="Screenshot 2026-10-30 234314" src="https://github.com/user-attachments/assets/a5901487-54d4-41b9-8f68-8672a6d8f426" />
<img width="1576" height="878" alt="Screenshot 2026-10-30 234342" src="https://github.com/user-attachments/assets/2016f61b-0375-4e69-a220-b7b4a5772d31" />
<img width="912" height="663" alt="Screenshot 2026-10-30 234400" src="https://github.com/user-attachments/assets/3a86d989-0b36-4459-8459-c35ad2278180" />
<img width="1651" height="963" alt="Screenshot 2026-10-30 234455" src="https://github.com/user-attachments/assets/e9ed8404-0102-4170-98e9-5d84940b803b" />

THANK YOU
