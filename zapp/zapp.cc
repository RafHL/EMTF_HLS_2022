/*******************************************************************************
 * Author:  Rafael Hernandez
 * Purpose: Zone Array Parser and Partitioner (ZAPP) is used for partially
 *              autonomous optimization of the zone boundary array using
 *              background knowledge about Zones that no compiler would have. I
 *              call it an ASPO, an Application-Specific Program Optimizer
 *              because of the use of background knowledge specific to a certain
 *              application
 * Assumptions: If boundaries' minimum is 1, then can safely subtract 1 from
 *                  lower and upper bound. If not, then keeps values as is --
 *                  may increase size if on boundary of 2 (15 vs 16).
 *              Python has EMTFZone or EMTFLayer, one def: line, and then the
 *                  initialization.
 *              No multiline code support for Python ('code; more_code') in this
 *                  version of ZAPP
 ******************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <cmath>

#define THIS_ZAPP_VERSION 2.0
#define prompt_bp 0
#define verbose_bp 1
#define BIT(a) (1 << a)

template <typename T>
struct triad {
    T first;
    T second;
    T third;
    triad(T first = 0, T second = 0, T third = 0) :
        first(first), second(second), third(third) { }
};

template <typename T>
struct tripair {
    std::pair<T,T> p;
    triad<T>       t;
};

int find(std::vector<triad<int>> &inp, triad<int> val); // Finds val in inp

int zapp(std::string zba_name, int flags = 1, int num = 0);
int loc_in_file(std::ifstream &file, std::string str, std::string &line, int flags=0);
void print_vect(std::vector<tripair<int>> &vect);
void print_vect_loc(std::vector<tripair<int>> &vect);

int main(int argc, char **argv) {
    int tmp = 0;
    int flags = 0;
    int valid = 0;
    int version = THIS_ZAPP_VERSION;

    for (int i = 1; i < argc; i++) { // Limited by main input to int, instead of size_t
        if (argv[i][0] == '-') {
            switch(argv[i][1]) {
                case 'h':
                    std::cout << "Welcome to Zone Array Partitioner and Parser (ZAPP)!" << std::endl << std::endl
                              << "I provide CMS L1-Trigger HLS with optimized zones boundaries based on experimental EMTFZone/EMTFLayer arrays & initializations in Python." << std::endl
                              << "Instead of executing scripts in Python directly, I parse for what I need from these scripts." << std::endl << std::endl
                              << "Usage: ./program_name filename" << std::endl
                              << "Input file should have EMTFZone and EMTFLayer initializations." << std::endl
                              << "Flags can be supplied between, before, or after filenames." << std::endl
                              << "Warning: on some machines, output files with the same name will be overwritten to nothing regardless of your answer to the prompt (-p 1)." << std::endl
                              << std::endl << "Supported flags:" << std::endl
                              << "-h        Help, shows current output." << std::endl
                              << "-p [1,0]  Change whether program prompts for writing output file for all following input filenames." << std::endl
                              << "-v [1,0]  Choose verbosity. Prints out debugging information if 1." << std::endl
                              << "-V        Display version information" << std::endl
                              << std::endl << "Please send any questions to Rafael Hernandez at rhernandezlopez1@ufl.edu." << std::endl;
                    break;
                case 'p':
                    i++;
                    if (i < argc) {
                        tmp = std::atoi(argv[i]);
                        if (tmp) {
                            std::cout << "Will now prompt for overwriting output file." << std::endl << std::endl;
                            flags |= BIT(prompt_bp);
                        } else {
                            std::cout << "Will now be skipping prompt for overwriting output file." << std::endl << std::endl;
                            flags &= ~BIT(prompt_bp);
                        }
                    }
                    break;
                case 'v':
                    i++;
                    if (i < argc) {
                        tmp = std::atoi(argv[i]);
                        if (tmp) {
                            std::cout << "Will print debugging information." << std::endl << std::endl;
                            flags |= BIT(verbose_bp);
                        } else {
                            std::cout << "Will not print debugging information." << std::endl << std::endl;
                            flags &= ~BIT(verbose_bp);
                        }
                    }
                    break;
                case 'V':
                    std::cout << "Zone Array Parser and Partitioner (ZAPP) V-" << version << std::endl << std::endl;
                default:
                    break;
            }
            continue;
        }
        valid++;
        tmp = zapp(std::string(argv[i]), flags, valid-1);
        if (tmp)
            std::cout << std::endl << "Unsuccessful for file: '" << argv[i] << "' "
                << "Input number: " << valid << std::endl << "Error: " << tmp << std::endl;
        std::cout << std::endl;
    }

    return tmp;
}

int zapp(std::string zba_name, int flags, int num) {
    // L1TMuonSimulationsMar2017/Analyzers/test7/rootpy_trackbuilding11.py
    //std::string zba_name = "rootpy_trackbuilding11.py";
#ifdef MYIMAGESIPYNB
    // L1TMuonSimulationsMar2017/Analyzers/test8/myimages5.ipynb
    std::string cms_name = "myimages5.ipynb";
#endif // MYIMAGESIPYNB
    std::string of_name = "zone_bnds";
    std::string line;

    if (num)
        of_name += std::to_string(num);

#ifdef MYIMAGESIPYNB
    // [layer] -> array of valid layers
    std::vector<std::vector<int>> zns_val_lay; // Which layers are valid in each zone
    int pos, vind;
    char val;
#endif // MYIMAGESIPYNB
    // [type][station][ring] -> layer
    std::vector<std::vector<std::vector<int>>> sen_lay; // Which sensors are in which layer
    // Valid [type][station][ring] -> layer
    std::vector<std::vector<std::vector<std::vector<int>>>> val_lay; // Which layers are valid
    // [zone][type][station][ring] -> (min_bound, max_bound)
    std::vector<std::vector<std::vector<std::vector<std::pair<int,int>>>>> sen_bnd; // Sensor bounds in each zone
    // [enum_value] first=type, second=station, third=ring
    std::vector<triad<int>> enum_tsr; // Keeps track of necessary converters and enumerates them

    int brk, obrk, cbrk, tmp, fnd, ind[5];

    // STEP 0: Open Python files
    std::ifstream zba_file(zba_name);
#ifdef MYIMAGESIPYNB
    std::ifstream cms_file(cms_name);
#endif // MYIMAGESIPYNB
    std::ofstream of(of_name + ".cc");
    std::ofstream hf(of_name + ".h");

    if (!zba_file.is_open()) {
        std::cout << "Was not able to open " << zba_name << std::endl;
        return -1;
    }

    if (!of.is_open()) {
        std::cout << "Was not able to create " << of_name << ".cc" << std::endl;
        return -1;
    }

    if (!hf.is_open()) {
        std::cout << "Was not able to create " << of_name << ".h" << std::endl;
        return -1;
    }

#ifdef MYIMAGESIPYNB
    if (!cms_file.is_open()) {
        std::cout << "Was not able to open " << cms_name << std::endl;
        return -1;
    }

    // STEP 1: Identify valid_layers location
    fnd = loc_in_file(cms_file, "valid_layers", line);
    if (!fnd)
        return -1;

    // STEP 2: Parse and reconstruct valid_layers
    //         Expects "valid_layers[] = [..]"
    //             handles arbitrarily large number of items within '= [..]'
    brk = obrk = cbrk = tmp = 0;
    while (!cms_file.eof()) {
        if (line.find("np.") != std::string::npos && line.find("valid_layers") != std::string::npos) { // Skip initialize to 0
            std::cout << "Found valid_layers initialization: " << line << std::endl;
            // TODO account for mutliple statements in one line
            if (line.find(";") != std::string::npos)
                std::cout << "Multiple statements in one line detected, but TODO " << line << std::endl;
            std::cout << line << std::endl;
            continue;
        // Parse initialization
        } else if (line.find("valid_layers") != std::string::npos || brk > 0) {
            for (int i = 0; (val = line[i]) != '\0'; i++) {
                brk += (obrk = (val == '[')); // Increment if opening bracket
                brk -= (cbrk = (val == ']')); // Decrement if closing bracket
                // TODO test for multiple spaces between elements
                if (brk && val >= '0' && val <= '9') { // If numeric, absorb number if inside brackets
                    tmp = strtol(line.c_str()+i, NULL, 10);
                    //std::cout << "Got the value: " << tmp << std::endl;
                    if (tmp != 0)
                        i += log10(tmp); // Move forward however many base10 digits are in tmp
                    // Check for more values before next bracket. If only one
                    //     value, then is index. If the comma comes from ipynb
                    //     formatting (","), ignore the comma
                    if (
                        (pos = line.find(',', i)) == std::string::npos ||
                        (pos > line.find(']', i) && (pos-1 != line.find("\",")))
                    ) {
                        vind = tmp;
                        if ((unsigned) vind >= zns_val_lay.size()) // If index larger, grow vector
                            zns_val_lay.resize(vind+1);
                        std::cout << std::endl << "Found index: " << tmp << std::endl;
                    } else { // If more values, then index was already found and we collected data
                        zns_val_lay[vind].push_back(tmp);
                        std::cout << "Found data[" << vind << "][" << zns_val_lay[vind].size()-1 << "]: " << tmp << std::endl;
                    }
                }
            }
        }
        getline(cms_file, line);
    }
    cms_file.close(); // No longer need cms file
    std::cout << std::endl;
#endif // MYIMAGESIPYNB

    // STEP 3: Locate EMTFLayer which identifies sensor <--> layer relationships
    fnd = loc_in_file(zba_file, "class EMTFLayer", line);
    if (!fnd)
        return -1;
    getline(zba_file, line);
    //    if (line.find("lut") == std::string::npos) {
    //        std::cout << "Unable to find EMTFLayer's lut in file '" << zba_name << "'" << std::endl;
    //        return -1;
    //    }

    // STEP 4: Parse and reconstruct sensors' layers
    //         Expects "lut[,,] = "
    brk = obrk = cbrk = tmp = 0;
    while (!zba_file.eof()) {
        if (line.find("np.") != std::string::npos && line.find("lut") != std::string::npos && cbrk < 1) { // Skip initialize to 0
            if (flags & BIT(verbose_bp))
                std::cout << "Found lut initialization: " << line << std::endl;
            // TODO account for mutliple statements in one line
            if (line.find(";") != std::string::npos)
                std::cout << "Multiple statements in one line detected, but TODO " << line << std::endl;
            if (flags & BIT(verbose_bp))
                std::cout << line << std::endl;
            getline(zba_file, line);
            cbrk++;
            continue;
        // Parse initialization
        } else if (line.find("self.lut = lut") != std::string::npos) {
            break;
        } else if (line.find("lut") != std::string::npos) {
            const char *linec = line.c_str();
            obrk = 0;
            while (linec[obrk] == ' ' || linec[obrk] == '\t') obrk++;
            brk = sscanf(linec + obrk, "lut[%d,%d,%d] = %d", ind, ind+1, ind+2, &tmp);
            if (brk != 4) {
                if (flags & BIT(verbose_bp))
                    std::cout << "Found: " << brk << " In: " << line << std::endl;
                getline(zba_file, line);
                continue;
            }
            //std::cout << line << std::endl;
            //std::cout << ind[0] << ", " << ind[1] << ", " << ind[2] << ", " << std::endl;

            if ((unsigned) ind[0] >= sen_lay.size()) {
                sen_lay.resize(ind[0] + 1);
                if (flags & BIT(verbose_bp))
                    std::cout << "ind0: " << ind[0] << ", " << sen_lay.size() << std::endl;
            }

            if ((unsigned) ind[1] >= sen_lay[ind[0]].size()) {
                sen_lay[ind[0]].resize(ind[1] + 1);
                if (flags & BIT(verbose_bp))
                    std::cout << "ind1: " << ind[1] << ", " << sen_lay[ind[0]].size() << std::endl;
            }

            if ((unsigned) ind[2] >= sen_lay[ind[0]][ind[1]].size()) {
                sen_lay[ind[0]][ind[1]].resize(ind[2] + 1);
                if (flags & BIT(verbose_bp))
                    std::cout << "ind2: " << ind[2] << ", " << sen_lay[ind[0]][ind[1]].size() << std::endl;
            }

            // std::cout << "\tSize1: " << std::hex << sen_lay.size() << ", "
                      // << "Size2: "   << std::hex << sen_lay[ind[0]].size() << ", "
                      // << "Size3: "   << std::hex << sen_lay[ind[0]][ind[1]].size() << ", " << std::endl;
            sen_lay[ind[0]][ind[1]][ind[2]] = tmp;

            if (flags & BIT(verbose_bp))
                std::cout << "\t" << ind[0] << ", " << ind[1] << ", " << ind[2] << ", "
                          << sen_lay[ind[0]][ind[1]][ind[2]] << std::endl;
        }
        getline(zba_file, line);
    }

    // STEP 6: Locate EMTFZone which identifies boundaries
    fnd = loc_in_file(zba_file, "class EMTFZone", line);
    if (!fnd)
        return -1;
    getline(zba_file, line);

    // STEP 7: Parse and reconstruct zone boundaries
    //         Expects "lut[,,][] = ,"
    brk = obrk = cbrk = tmp = 0;
    while (!zba_file.eof()) {
        if (line.find("np.") != std::string::npos && line.find("lut") != std::string::npos && cbrk < 1) { // Skip initialize to 0
            if (flags & BIT(verbose_bp))
                std::cout << "Found lut initialization: " << line << std::endl;
            // TODO account for mutliple statements in one line
            if (line.find(";") != std::string::npos)
                std::cout << "Multiple statements in one line detected, but TODO " << line << std::endl;
            if (flags & BIT(verbose_bp))
                std::cout << line << std::endl;
            getline(zba_file, line);
            cbrk++;
            continue;
        // Parse initialization
        } else if (line.find("self.lut = lut") != std::string::npos) {
            break;
        } else if (line.find("lut") != std::string::npos) {
            const char *linec = line.c_str();
            obrk = 0;
            while (linec[obrk] == ' ' || linec[obrk] == '\t') obrk++;
            brk = sscanf(linec + obrk, "lut[%d,%d,%d][%d] = %d,%d", ind+1, ind+2, ind+3, ind, ind+4, &tmp);
            if (brk != 6) {
                if (flags & BIT(verbose_bp))
                    std::cout << "Found: " << brk << " In: " << line << std::endl;
                getline(zba_file, line);
                continue;
            }
            if (flags & BIT(verbose_bp)) {
                std::cout << line << std::endl
                          << ind[0] << ", "
                          << ind[1] << ", "
                          << ind[2] << ", "
                          << ind[3] << ", " << std::endl;
            }

            if ((unsigned) ind[0] >= sen_bnd.size()) {
                sen_bnd.resize(ind[0] + 1);
                val_lay.resize(ind[0] + 1);
                if (flags & BIT(verbose_bp))
                    std::cout << "ind0: " << ind[0] << ", " << sen_bnd.size() << std::endl;
            }

            if ((unsigned) ind[1] >= sen_bnd[ind[0]].size()) {
                sen_bnd[ind[0]].resize(ind[1] + 1);
                val_lay[ind[0]].resize(ind[1] + 1);
                if (flags & BIT(verbose_bp))
                    std::cout << "ind1: " << ind[1] << ", " << sen_bnd[ind[0]].size() << std::endl;
            }

            if ((unsigned) ind[2] >= sen_bnd[ind[0]][ind[1]].size()) {
                sen_bnd[ind[0]][ind[1]].resize(ind[2] + 1);
                val_lay[ind[0]][ind[1]].resize(ind[2] + 1);
                if (flags & BIT(verbose_bp))
                    std::cout << "ind2: " << ind[2] << ", " << sen_bnd[ind[0]][ind[1]].size() << std::endl;
            }

            if ((unsigned) ind[3] >= sen_bnd[ind[0]][ind[1]][ind[2]].size()) {
                sen_bnd[ind[0]][ind[1]][ind[2]].resize(ind[3] + 1);
                val_lay[ind[0]][ind[1]][ind[2]].resize(ind[3] + 1, 0);
                if (flags & BIT(verbose_bp))
                    std::cout << "ind3: " << ind[3] << ", " << sen_bnd[ind[0]][ind[1]][ind[2]].size() << std::endl;
            }

            // std::cout << "\tSize1: " << std::hex << sen_bnd.size() << ", "
                      // << "Size2: "   << std::hex << sen_bnd[ind[0]].size() << ", "
                      // << "Size3: "   << std::hex << sen_bnd[ind[0]][ind[1]].size() << ", " << std::endl;
            sen_bnd[ind[0]][ind[1]][ind[2]][ind[3]] = std::pair<int, int>(ind[4], tmp);

            val_lay[ind[0]][ind[1]][ind[2]][ind[3]] = 1;
            if (flags & BIT(verbose_bp)) {
                std::cout << "\tVal1: " << ind[0] << ", "
                          << "Val2: " << ind[1] << ", "
                          << "Val3: " << ind[2] << ", "
                          << "Val4: " << ind[3] << ", "
                          << sen_bnd[ind[0]][ind[1]][ind[2]][ind[3]].first << ", "
                          << sen_bnd[ind[0]][ind[1]][ind[2]][ind[3]].second << std::endl;
            }
        }
        getline(zba_file, line);
    }
    zba_file.close();

    // STEP 8: Partition and reorganize zone boundaries based on valid layers
    //     and closeness to collision beam
    //         Analyze bounds
    // tritmp
    //      |
    //      +-- t
    //      |   |
    //      |   +-- type
    //      |   +-- station
    //      |   +-- ring
    //      |
    //      +-- p
    //          |
    //          +-- lower bound
    //          +-- upper bound
    // Enumerates sensors for printing
    std::vector<std::vector<int>> zns_ind_zapp;    // Used to store zone indexes
    std::vector<int> zn_ind;                       // Used to store a single line of indecies
    int max_ind = std::numeric_limits<int>::min(); // Contains maximum ind in zns_ind_zapp
    std::vector<tripair<int>> vect;                // Used to organize layers within a zone
    std::vector<std::vector<tripair<int>>> outp;   // Final output
    tripair<int> tritmp;
    size_t zone_cnt, max_type, max_station, max_ring, max_layer_zone;
    int min_val = std::numeric_limits<int>::max();
    int max_val = std::numeric_limits<int>::min();
    int max_layer = std::numeric_limits<int>::min();
    zone_cnt = sen_bnd.size();
    max_layer_zone = max_ring = max_station = max_type = 0;
    for (size_t zn = 0; zn < zone_cnt; zn++) {
        vect.clear();
        if (max_type < sen_bnd[zn].size())
            max_type = sen_bnd[zn].size();
        for (size_t ty = 0, f = sen_bnd[zn].size(); ty < f; ty++) {
            if (max_station < sen_bnd[zn][ty].size())
                max_station = sen_bnd[zn][ty].size();
            for (size_t st = 0, g = sen_bnd[zn][ty].size(); st < g; st++) {
                if (max_ring < sen_bnd[zn][ty][st].size())
                    max_ring = sen_bnd[zn][ty][st].size();
                for (size_t ri = 0, h = sen_bnd[zn][ty][st].size(); ri < h; ri++) {
                    //std::cout << "val_lay " << val_lay[zn][ty][st][ri]
                              //<< ", " << zn << ", " << ty << ", " << st
                              //<< ", " << ri << std::endl;
                    if (!val_lay[zn][ty][st][ri])
                        continue;
                    tritmp.t.first = ty;
                    tritmp.t.second= st;
                    tritmp.t.third = ri;
                    tritmp.p.first = sen_bnd[zn][ty][st][ri].first;
                    tritmp.p.second= sen_bnd[zn][ty][st][ri].second;
                    if (tritmp.p.first < min_val)
                        min_val = tritmp.p.first;
                    if (tritmp.p.first > max_val)
                        max_val = tritmp.p.first;
                    if (tritmp.p.second < min_val)
                        min_val = tritmp.p.second;
                    if (tritmp.p.second > max_val)
                        max_val = tritmp.p.second;
                    bool wr = 0;
                    for (auto i = vect.begin(), a = vect.end(); i != a; i++) {
                        //std::cout << i->t.first << ", " << i->t.second << ", " << i->t.third << std::endl;
                        if (sen_lay[i->t.first][i->t.second][i->t.third] >= sen_lay[ty][st][ri]) {
                            if (max_layer < sen_lay[ty][st][ri])
                                max_layer = sen_lay[ty][st][ri];
                            vect.insert(i, tritmp);
                            wr = 1;
                            break;
                        }
                    }
                    if (!wr)
                        vect.push_back(tritmp);
                }
            }
        }
        if (max_layer_zone < vect.size())
            max_layer_zone = vect.size();
        if (flags & BIT(prompt_bp)) {
            std::cout << "loc";
            print_vect_loc(vect);
            std::cout << "val";
            print_vect(vect);
            std::cout << std::endl
                      << std::endl
                      << std::endl;
        }
        outp.push_back(vect);
        zn_ind.clear();
        int indb = 0;
        int inde = 0;
        for (int i = 0, e = vect.size(); i < e; i++) {
            inde = indb + vect[i].p.second - vect[i].p.first;
            if (flags & BIT(verbose_bp))
                std::cout << indb << ", " << inde << ", ";
            zn_ind.push_back(indb);
            zn_ind.push_back(inde);
            indb = inde + 1;
        }
        if (flags & BIT(verbose_bp))
            std::cout << std::endl;
        if (zn_ind.back() > max_ind)
            max_ind = zn_ind.back();
        zns_ind_zapp.push_back(zn_ind);
    }
    for (size_t ty = 0; ty < max_type; ty++) { // Fill up enumerated sensors
        for (size_t st = 0; st < max_station; st++) {
            for (size_t ri = 0; ri < max_ring; ri++) {
                for (size_t zn = 0; zn < zone_cnt; zn++) {
                    try {
                        if (!val_lay.at(zn).at(ty).at(st).at(ri))
                            continue;
                    } catch(std::out_of_range &e) { // If out of range, then wasn't valid
                        continue;
                    }
                    enum_tsr.emplace_back(ty, st, ri);
                    break; // Store only once per type|station|ring
                }
            }
        }
    }

    // STEP 9: Prompt for feedback
    if (flags & BIT(prompt_bp)) {
        while (1) {
            std::cout << "Does this look alright? ";
            std::cin >> line;
            if (line.find("y") != std::string::npos || line.find("Y") != std::string::npos)
                break;
            else if (line.find("n") != std::string::npos || line.find("N") != std::string::npos)
                return -1;
            std::cout << std::endl << "None of the following detected: n,N,y,Y" << std::endl << std::endl;
        }
    }

    // STEP 10: Output file
    of << "/********************************************************************************" << std::endl
       << " * Automatically generated by ZAPP-V" << THIS_ZAPP_VERSION << ", please DO NOT edit." << std::endl
       << " * Contact Rafael Hernandez at rhernandezlopez1@ufl.edu with any" << std::endl
       << " *     questions or concerns." << std::endl
       << " * Requires " << of_name << ".h" << std::endl
       << " *******************************************************************************/" << std::endl
       << std::endl;
    hf << "#ifndef __" << of_name << "_h__" << std::endl << "#define __" << of_name << "_h__" << std::endl << std::endl
       << "/********************************************************************************" << std::endl
       << " * Automatically generated by ZAPP-V" << THIS_ZAPP_VERSION << ", please DO NOT edit." << std::endl
       << " * Contact Rafael Hernandez at rhernandezlopez1@ufl.edu with any" << std::endl
       << " *     questions or concerns." << std::endl
       << " *******************************************************************************/" << std::endl
       << std::endl;

    hf << "// Comment for ap_uint arrays, uncomment for constexpr arrays" << std::endl
       << "#define ZAPP_CONSTEXPR_ARRAYS" << std::endl;

    hf << std::endl;

    // STEP 11: Send bounds of input file

    int zapp_lay_sz = ceil(log2(max_layer));
    hf << "// Max layer: " << max_layer << std::endl
       << "// Largest layer value needs " << log2(max_layer)
       << " bits" << std::endl
       << "#define ZAPP_LAY_SZ " << zapp_lay_sz << std::endl;

    hf << std::endl;

    int ind_type = (min_val == 1); // Start indexing at one if minimum is 1
    int zapp_bnd_sz = ceil(log2(max_val-ind_type));
    hf << "// Detected index type: " << ((ind_type) ? "One" : "Zero") << std::endl
       << "// Max zone boundary: " << max_val-ind_type << std::endl
       << "// Largest value for zone boundaries needs " << log2(max_val-ind_type)
       << " bits" << std::endl
       << "#define ZAPP_BND_SZ " << zapp_bnd_sz << std::endl;

    hf << std::endl;

    int zapp_typ_sz = ceil(log2(max_type));
    hf << "// Max types: " << max_type << std::endl
       << "// Largest value for type needs " << log2(max_type)
       << " bits" << std::endl
       << "#define ZAPP_TYP_SZ " << zapp_typ_sz << std::endl;

    hf << std::endl;

    int zapp_stn_sz = ceil(log2(max_station));
    hf << "// Max stations:" << std::endl
       << "#define ZAPP_STN_CNT " << max_station << std::endl
       << "// Largest value for station needs " << log2(max_station)
       << " bits" << std::endl
       << "#define ZAPP_STN_SZ " << zapp_stn_sz << std::endl;

    hf << std::endl;

    int zapp_rin_sz = ceil(log2(max_ring));
    hf << "// Max rings:" << std::endl
       << "#define ZAPP_RIN_CNT "<< max_ring << std::endl
       << "// Largest value for rings needs " << log2(max_ring)
       << " bits" << std::endl
       << "#define ZAPP_RIN_SZ " << zapp_rin_sz << std::endl;

    hf << std::endl;

    int zapp_conv_cnt = enum_tsr.size();
    int zapp_conv_sz = ceil(log2(zapp_conv_cnt));
    int zapp_enum_tsr_sz = (zapp_typ_sz >= zapp_stn_sz && zapp_typ_sz >= zapp_rin_sz) ? zapp_typ_sz :
                           (zapp_stn_sz >= zapp_typ_sz && zapp_stn_sz >= zapp_rin_sz) ? zapp_stn_sz :
                                                                                        zapp_rin_sz;
    hf << "// Number of converters necessary, used for enumeration" << std::endl
       << "#define ZAPP_CONV_CNT " << zapp_conv_cnt << std::endl
       << "// " << log2(zapp_conv_cnt) << "bits needed" << std::endl
       << "#define ZAPP_CONV_SZ " << zapp_conv_sz << std::endl
       << "// Bits needed for enum_tsr values:" << std::endl
       << "#define ZAPP_ENUM_TSR_SZ " << zapp_enum_tsr_sz << std::endl;

    hf << std::endl;

    int zapp_zone_cnt = zone_cnt;
    hf << "// Number of zones recorded:" << std::endl
       << "#define ZAPP_ZONE_CNT " << zapp_zone_cnt << std::endl;

    hf << std::endl;

    int zapp_layer_zone_cnt = max_layer_zone;
    hf << "// Maximum number of layers in all zones:" << std::endl
       << "#define ZAPP_LAYER_ZONE_CNT " << zapp_layer_zone_cnt << std::endl;

    hf << std::endl;

    int zapp_ind_sz = ceil(log2(max_ind+1-ind_type));
    hf << "// Lagest index:" << std::endl
       << "#define ZAPP_IND_CNT " << max_ind+1-ind_type << std::endl
       << "// Bits needed:" << std::endl
       << "#define ZAPP_IND_SZ " << zapp_ind_sz << std::endl;

    hf << std::endl;

    hf << "#endif" << std::endl << std::endl;
    hf.close();

    of << "// Enumerated type, station, ring" << std::endl
       << "#ifdef ZAPP_CONSTEXPR_ARRAYS" << std::endl
       << "constexpr int             enum_tsr[ZAPP_CONV_CNT][3] = {" << std::endl
       << "#else" << std::endl
       << "ap_uint<ZAPP_ENUM_TSR_SZ> enum_tsr[ZAPP_CONV_CNT][3] = {" << std::endl
       << "#endif" << std::endl;
    // Print enumerated type and location array here
    int ty_align = log10(1 << zapp_typ_sz)+1;
    int st_align = log10(1 << zapp_stn_sz)+1;
    int ri_align = log10(1 << zapp_rin_sz)+1;
    for (int i = 0; i < zapp_conv_cnt; i++) {
        of << "    {" << std::setw(ty_align) << enum_tsr[i].first  << ", "
           <<            std::setw(st_align) << enum_tsr[i].second << ", "
           <<            std::setw(ri_align) << enum_tsr[i].third  << "}"
           << ((i < zapp_conv_cnt-1) ? ", " : "  ") << "// " << i << std::endl;
    }
    of << "};" << std::endl;

    of << std::endl
       << "// Location array for reference, or possible future use." << std::endl
       << "// These numbers are the indecies of enum_tsr which has the detectors used."
       << std::endl
       << "#ifdef ZAPP_CONSTEXPR_ARRAYS" << std::endl
       << "constexpr int         loc_zapp[ZAPP_ZONE_CNT][ZAPP_LAYER_ZONE_CNT] = {" << std::endl
       << "#else" << std::endl
       << "// Size should be largest of ZAPP_LAY_SZ and ZAPP_RIN_SZ" << std::endl
       << "ap_uint<ZAPP_CONV_SZ> loc_zapp[ZAPP_ZONE_CNT][ZAPP_LAYER_ZONE_CNT] = {" << std::endl
       << "#endif" << std::endl;
    // Location array here
    int zn = 0;
    int zero = 0;
    int align = log10(1 << zapp_conv_sz)+1;
    while (zn < (signed) zapp_zone_cnt) {
        of << "    {";
        int ly = 0;
        int e = (signed) outp[zn].size();
        while (ly < e) {
            int ind = find(enum_tsr, outp[zn][ly].t);
            of << std::setw(align) << ind;
            if (++ly < (signed) max_layer_zone)
                of << ", ";
        }
        while (ly < (signed) max_layer_zone) {
            of << std::setw(align) << zero;
            if (++ly < (signed) max_layer_zone)
                of << ", ";
        }
        of << "}";
        if (++zn < (signed) zone_cnt)
            of << ",";
        of << std::endl;
    }
    of << "};" << std::endl;
       //<< "*/" << std::endl;

    of << std::endl
       << "// Size of each zone array for use with zone boundary array:" << std::endl
       << "#ifdef ZAPP_CONSTEXPR_ARRAYS" << std::endl
       << "constexpr int        siz_zapp[ZAPP_ZONE_CNT] = {" << std::endl
       << "#else" << std::endl
       << "ap_uint<ZAPP_LAY_SZ> siz_zapp[ZAPP_ZONE_CNT] = {" << std::endl
       << "#endif" << std::endl;
    // Size array here
    zn = 0;
    of << "    ";
    while (zn < (signed) zapp_zone_cnt) {
        of << outp[zn].size();
        if (++zn < (signed) zapp_zone_cnt)
            of << ", ";
    }
    of << std::endl << "};" << std::endl
       << std::endl;

    of << std::endl
       << "// Zone boundary array: 0-low 1-high" << std::endl
       << "#ifdef ZAPP_CONSTEXPR_ARRAYS" << std::endl
       << "constexpr int        zns_bnd_zapp[ZAPP_ZONE_CNT][ZAPP_LAYER_ZONE_CNT][2] = {" << std::endl
       << "#else" << std::endl
       << "ap_uint<ZAPP_BND_SZ> zns_bnd_zapp[ZAPP_ZONE_CNT][ZAPP_LAYER_ZONE_CNT][2] = {" << std::endl
       << "#endif" << std::endl;
    // Zone boundary array here
    // TODO 2
    zn = 0;
    align = log10(1 << zapp_bnd_sz)+1;
    while (zn < (signed) zapp_zone_cnt) {
        of << "    {";
        int ly = 0;
        int e = (signed) outp[zn].size();
        while (ly < e) {
            of << "{" << std::setw(align) << outp[zn][ly].p.first-ind_type << ", " << std::setw(align) << outp[zn][ly].p.second-ind_type << "}";
            if (++ly < (signed) max_layer_zone)
                of << ",";
        }
        while (ly < (signed) max_layer_zone) {
            of << "{" << std::setw(align) << zero << ", " << std::setw(align) << zero << "}";
            if (++ly < (signed) max_layer_zone)
                of << ",";
        }
        of << "}";
        if (++zn < (signed) zapp_zone_cnt)
            of << ",";
        of << std::endl;
    }
    of << "};" << std::endl
       << std::endl;

    of << std::endl
       << "// Zone index array:" << std::endl
       << "#ifdef ZAPP_CONSTEXPR_ARRAYS" << std::endl
       << "constexpr int        zns_ind_zapp[ZAPP_ZONE_CNT][2*ZAPP_LAYER_ZONE_CNT] = {" << std::endl
       << "#else" << std::endl
       << "ap_uint<ZAPP_IND_SZ> zns_ind_zapp[ZAPP_ZONE_CNT][2*ZAPP_LAYER_ZONE_CNT] = {" << std::endl
       << "#endif" << std::endl;
    // // TODO 2
    // Zone index array here
    zn = 0;
    align = log10(1 << ((int)ceil(log2(max_ind+1))))+1;
    while (zn < (signed) zapp_zone_cnt) {
        of << "    {";
        int ly = 0;
        int e = (signed) zns_ind_zapp[zn].size();
        while (ly < e) {
            of << std::setw(align) << zns_ind_zapp[zn][ly] << ", " << std::setw(align) << zns_ind_zapp[zn][ly+1];
            ly += 2;
            if (ly < (signed) 2*zapp_layer_zone_cnt)
                of << ", ";
        }
        while (ly < (signed) 2*zapp_layer_zone_cnt) {
            of << std::setw(align) << zero << ", " << std::setw(align) << zero;
            ly += 2;
            if (ly < (signed) 2*zapp_layer_zone_cnt)
                of << ", ";
        }
        of << "}";
        if (++zn < (signed) zapp_zone_cnt)
            of << ",";
        of << std::endl;
    }
    of << std::endl << "};" << std::endl
       << std::endl;

    of << std::endl;
    of.close();

    // STEP Final: Include file in project
    std::cout << std::endl << "Input filename: " << zba_name << std::endl
              << "Output filename: " << of_name << std::endl;

    int decoded = (2*zone_cnt*max_ring*max_station*max_type + max_ring*max_station*max_type);
    int encoded = 3*zapp_conv_cnt +
                  zapp_zone_cnt*zapp_layer_zone_cnt +
                  zapp_zone_cnt +
                  4*zapp_zone_cnt*zapp_layer_zone_cnt;
    std::cout << std::endl << "Decoded: " << decoded << " elements used." << std::endl
              << "Encoded indecies full: " << encoded << " elements used." << std::endl
              << "\t" << 100*(decoded - encoded)/decoded << "% element reduction." << std::endl;


    decoded *= 32;
    encoded = zapp_enum_tsr_sz*3*zapp_conv_cnt +
              zapp_conv_sz*zapp_zone_cnt*zapp_layer_zone_cnt +
              zapp_lay_sz*zapp_zone_cnt +
              zapp_bnd_sz*2*zapp_zone_cnt*zapp_layer_zone_cnt +
              zapp_ind_sz*2*zapp_zone_cnt*zapp_layer_zone_cnt;
    std::cout << std::endl << "Decoded: " << decoded << " bits used." << std::endl
              << "Encoded indecies w/o type and location: " << encoded << " bits used." << std::endl
              << "\t" << 100*(decoded - encoded)/decoded << "% bit reduction." << std::endl;

    encoded += zapp_typ_sz*zapp_zone_cnt*zapp_layer_zone_cnt + 2*((zapp_lay_sz > zapp_rin_sz) ? zapp_lay_sz : zapp_rin_sz)*zapp_zone_cnt*zapp_layer_zone_cnt;
    std::cout << "Encoded indecies w/  type and location: " << encoded << " bits used." << std::endl
              << "\t" << 100*(decoded - encoded)/decoded << "% bit reduction." << std::endl << std::endl;

    return 0;
}

// Advances file until str is found
int loc_in_file(std::ifstream &file, std::string str, std::string &line, int flags) {
    int fnd = 0;
    while (!file.eof()) {
        getline(file, line);
        if ((fnd = (line.find(str) != std::string::npos))) {
            if (flags & BIT(verbose_bp))
                std::cout << "Found " << str << ": " << line << std::endl;
            getline(file, line);
            if (flags & BIT(verbose_bp))
                std::cout << line << std::endl;
            break;
        }
    }
    if (!fnd)
        std::cout << std::endl << "Unable to find " << str << std::endl;
    return fnd;
}

void print_vect(std::vector<tripair<int>> &vect) {
    std::cout << std::endl << "{" << std::endl;
    for (auto &&tpair : vect)
        std::cout << "\t{" << tpair.p.first << ", " << tpair.p.second << "}" << std::endl;
    std::cout << "}" << std::endl;
}
void print_vect_loc(std::vector<tripair<int>> &vect) {
    std::cout << std::endl << "{" << std::endl;
    for (auto &&tpair : vect)
        std::cout << "\t{" << tpair.t.first << ", "
                  << tpair.t.second << ", " << tpair.t.third << "}" << std::endl;
    std::cout << "}" << std::endl;
}

int find(std::vector<triad<int>> &inp, triad<int> val) {
    if (inp.size() == 0)
        return -1;
    for (int i = 0, e = inp.size(); i < e; i++)
        if (inp[i].first == val.first)
            if (inp[i].second == val.second)
                if (inp[i].third == val.third)
                    return i;
    return -1;
}

