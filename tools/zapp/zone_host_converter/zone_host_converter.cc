#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <map>

// Host position
#define host_p 0
// Zone position
#define zone_p 1
// Verbose flag bit position
#define verbose_bp 1

#define BIT(a) (1 << a)

// Expecting:
//
//...emtf_host_init...  << Line containing this text
//  ...                 Multiple setup lines that are ignored
//  ...
//  lut[,,] = * ...     Starts reading after lut[ lines encountered
//  lut[...
//  lut[...             Repeatedly until last line:
//  lut[,,] = * ...     << Last lut[ line interpreted as part of host_init
//  xxxx
//                      Leading whitespace ignored
//
//...emtf_zones_init...  << Line containing this text
//  ...                 Multiple setup lines that are ignored
//  ...
//  lut[,,] = * ...     Starts reading after lut[ lines encountered
//  lut[...
//  lut[...             Repeatedly until last line:
//  lut[,,] = * ...     << Last lut[ line interpreted as part of host_init
//  xxxx
//                      Leading whitespace ignored
//
//
//  The host and zone init can be in separate files, or in the same file as long
//  as these criteria are met (specifically the line containing emtf_init text and
//  the following line after all the lut[ that does not contain lut[)

int zone_host_converter(int argc, char **argv);
int loc_in_file(std::ifstream &file, std::string str, std::string &line, int flags=BIT(verbose_bp));

int main(int argc, char **argv) {
    int tmp;

    if (argc < 2) {
        std::cout << "Usage: <prog> <host/zone filename> [<host/zone filename>]" << std::endl;
        return -1;
    }

    std::cout << std::endl << "Please make sure lut[] = # lines are not interrupted by empty lines, '#', or anything else" << std::endl << std::endl;

    tmp = zone_host_converter(argc, argv);
    if (tmp) {
        std::cout << std::endl << "Unsuccessful for files: " << std::endl;
        for (int i = 1; i < argc; i++)
            std::cout << "    '" << argv[i] << "'" << std::endl;
        std::cout << "  Error: " << tmp << " ";
        if      (tmp == host_p + 1)
            std::cout << "[host definition file not found]" << std::endl;
        else if (tmp == zone_p + 1)
            std::cout << "[zone definition file not found]" << std::endl;
        else if (tmp == -2)
            std::cout << "[Cannot open input file]" << std::endl;
        else
            std::cout << "[No error information available]" << std::endl;
    }
    std::cout << std::endl;

    return tmp;
}

int zone_host_converter(int argc, char **argv) {
    std::ifstream ifile;
    std::ofstream ofile;
    std::string line;
    std::map<std::tuple<int,int,int>, int> sen_lay; // Which sensors are in which layer
    char *str;
    std::tuple<int,int,int> key(0,0,0);
    int brk, obrk, cbrk, fnd, ind[4];
    int host = 0, zone = 0, lbnd = 0, ubnd = 0;
    int files[2] = {0,0}; // [0] -- host filename position in argv[]
                          // [1] -- zone filename position in argv[]
    int lines[2] = {0,0}; // [0] -- host definition line in argv[files[]]
                          // [1] -- zone definition line in argv[files[]]

    ofile.open("zone_host_converter_output.txt");
    if (!ofile.is_open()) {
        std::cout << "Was not able to open output file: 'zone_host_converter_output.txt'" << std::endl;
        return -2;
    }

    // STEP 1: Locate host and zone input files. Possibilities:
    //             host/zone in one input file
    //             host/zone in separate input files
    //             host/zone information missing
    for (int i = argc-1; i > 0; i--) {
        ifile.open(argv[i]);
        if (!ifile.is_open()) {
            std::cout << "Was not able to open " << argv[i] << std::endl;
            return -2;
        }

        // Look for host definition
        fnd = loc_in_file(ifile, "emtf_host_init", line);
        if (fnd) {
            files[host_p] = i;
            lines[host_p] = fnd;
        }

        // Return to top of file
        ifile.clear();
        ifile.seekg(0);

        // Look for zone definition
        fnd = loc_in_file(ifile, "emtf_zones_init", line);
        if (fnd) {
            files[zone_p] = i;
            lines[zone_p] = fnd;
        }

        ifile.close();
    }

    std::cout << std::endl;
    std::cout << "Host filename: " << argv[files[host_p]] << std::endl;
    std::cout << "Zone filename: " << argv[files[zone_p]] << std::endl;

    // STEP 1.1: Error checking
    if (files[host_p] == 0)
        return host_p + 1; // Host definition file not found

    if (files[zone_p] == 0)
        return zone_p + 1; // Zone definition file not found

    // STEP 2: EMTFLayer information
    ofile << "# Decide EMTF hit layer number" << std::endl
          << "class EMTFLayer(object):" << std::endl
          << "  def __init__(self):" << std::endl
          << "  lut = np.zeros((5,5,5), dtype=np.int32) - 99  # (type, station, ring) -> layer" << std::endl;

    ifile.open(argv[files[host_p]]);
    // STEP 2.1: Go to def find...initializer() section
    for (int i = 0; i < lines[host_p]; i++)
        getline(ifile, line);
    // STEP 2.2: Go to LUT[,,] = # section
    while (!ifile.eof()) {
        getline(ifile, line);
        if (line.find("lut[") != std::string::npos)
            break;
    }
    // STEP 2.3: Pass and parse all lines until lut[] lines run out
    do {
        // Exit early if line isn't lut[] = #
        if (line.find("lut[") == std::string::npos)
            break;

        const char *linec = line.c_str();
        obrk = 0;

        // Remove opening whitespace
        while (linec[obrk] == ' ' || linec[obrk] == '\t') obrk++;

        // Check for data
        brk = sscanf(linec + obrk, "lut[%d,%d,%d] = %d", ind, ind+1, ind+2, ind+3);
        if (brk != 4) {
            //if (flags & BIT(verbose_bp))
            std::cout << "Found: " << brk << " In: " << line << std::endl;
            continue;
        }
        //std::cout << line << std::endl;
        //std::cout << ind[0] << ", " << ind[1] << ", " << ind[2] << ", " << std::endl;

        // Save new data
        std::get<0>(key) = ind[0];
        std::get<1>(key) = ind[1];
        std::get<2>(key) = ind[2];
        sen_lay.emplace(key, ind[3]);

        //if (flags & BIT(verbose_bp))
        //std::cout << "\t" << ind[0] << ", " << ind[1] << ", " << ind[2] << ", "
                  //<< sen_lay[key] << std::endl;
        ofile << line << std::endl;
    } while (getline(ifile, line) && !ifile.eof());
    ofile << "  self.lut = lut" << std::endl << std::endl;
    ifile.close();

    std::cout << std::endl << "EMTFLayer information transferred" << std::endl;

    // STEP 3: EMTFZone information
    ofile << "# Decide EMTF hit zones" << std::endl
          << "class EMTFZone(object):" << std::endl
          << "  def __init__(self):" << std::endl
          << "  lut = np.zeros((5,5,5,7,2), dtype=np.int32) - 99  # (type, station, ring) -> [zone] x [min_theta,max_theta]" << std::endl;

    ifile.open(argv[files[zone_p]]);
    // STEP 3.1: Go to def find...initializer() section
    for (int i = 0; i < lines[zone_p]; i++)
        getline(ifile, line);
    // STEP 3.2: Go to LUT[,,] = # section
    while (!ifile.eof()) {
        getline(ifile, line);
        if (line.find("lut[") != std::string::npos)
            break;
    }
    // STEP 3.3: Replace host with type,station,ring for ZAPP compatibility
    do {
        // Exit early if line isn't lut[] = #
        if (line.find("lut[") == std::string::npos)
            break;

        const char *linec = line.c_str();
        host = zone = lbnd = ubnd = obrk = 0;

        // Remove opening whitespace
        while (linec[obrk] == ' ' || linec[obrk] == '\t') obrk++;

        // Check for data
        brk = sscanf(linec + obrk, "lut[%d,%d] = %d,%d", &host, &zone, &lbnd, &ubnd);
        if (brk != 4) {
            //if (flags & BIT(verbose_bp))
            std::cout << "Found: " << brk << " In: " << line << std::endl;
            continue;
        }

        // Replace data
        for (auto const& s : sen_lay) {
            if (s.second == host) {
                ofile << "  lut[" << std::get<0>(s.first) << "," << std::get<1>(s.first) << "," << std::get<2>(s.first) << "]"
                              << "[" << zone << "] = " << lbnd << "," << ubnd << std::endl;
            }
        }
    } while (getline(ifile, line) && !ifile.eof());
    ofile << "  self.lut = lut" << std::endl << std::endl;

    std::cout << "EMTFZone  information generated" << std::endl;

    return 0;
}

// Advances file until str is found
// returns loc'th line, ie line 0 would return 1, not found would return 0
int loc_in_file(std::ifstream &file, std::string str, std::string &line, int flags) {
    int fnd = 0;
    int loc = 0;
    while (!file.eof()) {
        getline(file, line);
        loc++;
        if ((fnd = (line.find(str) != std::string::npos))) {
            if (flags & BIT(verbose_bp))
                std::cout << "Found " << str << ", Line " << loc << ": '" << line << "'" << std::endl;
            getline(file, line);
            //if (flags & BIT(verbose_bp))
            //    std::cout << line << std::endl;
            break;
        }
    }
    //if (!fnd)
    //    std::cout << std::endl << "Unable to find " << str << std::endl;
    if (fnd)
        return loc;
    else
        return 0;
}
