#ifndef VERSION_H
#define VERSION_H

#define MAJOR_VERSION   2
#define MINOR_VERSION   7
#define RELEASE_NUMBER  1
#define BUILD_NUMBER    1

#define XSTR(S)                 STR(S)
#define STR(S)                  #S
#define VERSIONBUILD(A,B,C,D)   A##.B##.C##.D       
#define VERSION_STR()           XSTR(VERSIONBUILD(MAJOR_VERSION,MINOR_VERSION,RELEASE_NUMBER,BUILD_NUMBER))


#endif