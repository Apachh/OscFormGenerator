/*
    sudo apt-get install libcanberra-gtk-module
*/

#include <cstring>
#include <iostream>
#include <cmath>
#include <limits>
#include <fstream>
#include <filesystem>

#include "my_arr.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

enum FORM_TYPES {
    NONE = 0,
    MEANDER,
    SIN,
    EXPONENT,
    DUAL_LEVEL_MEANDER,
    DUAL_LEVEL_MEANDER_WITH_EXPONENT
};

struct Steps_t {
    float pStep = 0.0f;
    float nStep = 0.0f;
};

const char* FORMS[] = {
    "MEANDER",
    "SIN",
    "EXPONENT",
    "DUAL_LEVEL_MEANDER",
    "DUAL_LEVEL_MEANDER_WITH_EXPONENT"
};

static const unsigned MAX_NUM_POINTS = 100;
static const unsigned X_AXIS = 0;
static const unsigned Y_AXIS = 1;
static const unsigned Z_AXIS = 2;
static const unsigned AXIS_CNT = 2;
static const unsigned FULL_X_PART = 10;
static const double PI = M_PI;
static const float ZERO = 0.001f;
// static const float ZERO = 0.5f;
static const char* OUT_FILENAME = "output.dat";
static const char* SYS_FILENAME = "gtk_var.txt";

FILE* gp;
std::fstream sys;
std::fstream outFile;

int formType = NONE;

arr::Array_t<MAX_NUM_POINTS, float, long int> sequence;

long int max = 0;
long int min = 0;

void introMessages();

Steps_t calcIEsteps(unsigned pMean);

unsigned genExponent(unsigned pNum, long int z0, long int z1);
unsigned genMeander(long int z0, long int z1);
unsigned genSin(unsigned pNum, long int z0, long int z1);
unsigned getDualLevelMeander(long int z0, long int z1);
unsigned getDualLevelMeanderWithExp(unsigned pNum, long int z0, long int z1);

template<typename T, long long GRADE>
static inline T rounding(T x);

void test();

int main() {

    // test();

    std::memset(&sequence, 0, sizeof(sequence));
    
    if(std::filesystem::exists(OUT_FILENAME)) {
        std::filesystem::remove(OUT_FILENAME);
    }

    // sys.open(SYS_FILENAME, std::ios::in);
    
    // if(!sys.is_open()) {
    //     std::cerr << "Error open to sysfile:" << SYS_FILENAME << endl;
    //     return 1;
    // }

    // string command = "echo $GTK_PATH > ";
    // command.append(SYS_FILENAME);
    // system(command.c_str());

    // if(!std::filesystem::is_empty(SYS_FILENAME))
        system("unset GTK_PATH");
    
    // sys.close();
    // std::filesystem::remove(SYS_FILENAME);

    gp = popen("gnuplot -", "w");

    outFile.open(OUT_FILENAME, std::ios::out);
    outFile << 0 << " " << 0 << endl;

    introMessages();

    unsigned nPoints = 0;
    if(outFile.is_open()) {
        switch(formType) {
            case MEANDER:                          nPoints = genMeander(max, min); break;
            case SIN:                              nPoints = genSin(24, max, min); break;
            case EXPONENT:                         nPoints = genExponent(24, max, min); break;
            case DUAL_LEVEL_MEANDER:               nPoints = getDualLevelMeander(max, min); break;
            case DUAL_LEVEL_MEANDER_WITH_EXPONENT: 
            default: break;
        }
    } else {
        std::cerr << "Error, closed" << endl;
        std::cerr << "Impossible to open file " << OUT_FILENAME << endl;
        return 1;
    }
    if(!nPoints) {
        std::cerr << "Error, closed" << endl;
        std::cerr << "nPoints = 0, pattern is Empty " << endl;
        return 1;
    }

    cout << "Pattern points: { ";
    unsigned i = 0;
    while(i < (nPoints - 1)) {
        cout << sequence.get<Y_AXIS>(i) << ", ";
        i++;
    }
    cout << sequence.get<Y_AXIS>(i) << " }" << endl;

    bool isCont = !(formType == SIN);
    i = 0;
    float x = 0.0f;
    while(i < (nPoints + 1)) {    
        outFile << sequence.get<X_AXIS>(i) << " " << sequence.get<Y_AXIS>(i) << endl;
        cout << i << ": " << sequence.get<X_AXIS>(i) << " " << sequence.get<Y_AXIS>(i) << endl;
        i++;
    }
    
    outFile.close();

    fprintf(gp, "set xrange [-2: %d]\n", FULL_X_PART + 2);
    fprintf(gp, "set yrange [%ld: %ld]\n", long(-std::abs(min) * 1.5f), long(std::abs(max) * 1.5f));
    fprintf(gp, "set arrow 1 from -50 to 50 nohead lc rgb \"black\" lw 1\n");
    fprintf(gp, "plot './");
    fprintf(gp, OUT_FILENAME);
    fprintf(gp, "' with lines \n");
    fflush(gp);
    while (true);
    
    return 0;
}

void introMessages() {
    unsigned formCnt = sizeof(FORMS) / sizeof(FORMS[0]);
    unsigned i = 0;
    while(i < formCnt) {
        cout << (i + 1) <<". " << FORMS[i] << endl;
        i++;
    }
    cout << "Choose the form for pattern: ";
    cin >> formType;
    if((formType > formCnt) || (formType == 0))
        cout << "Choosed wrong number:" << formType << endl << "Excepted :" << "{1, " << formCnt << "}" << endl;
    cout << "Choosed: " << FORMS[formType - 1] << endl;

    cout << "Insert max value: ";
    cin >> max;
    cout << "Insert min value: ";
    cin >> min;
    cout << "Choosed range: {" << -std::abs(min) << ", " << std::abs(max) << "}" << endl;
}

Steps_t calcIEsteps(unsigned pMean) {
    char I_E[] = "1:1\n";
    Steps_t output = {0, 0};

    cout << "Insert I:E (Format like: 1:2): ";
    cin.get();
    cin.getline(I_E, sizeof(I_E));
    cout << "Choosed I:E: " << I_E << endl;
    
    unsigned Ipart = unsigned(I_E[0]) - 48;
    unsigned Epart = unsigned(I_E[2]) - 48;
    unsigned part = FULL_X_PART / (Ipart + Epart);
    Ipart *= part;
    Epart *= part;

    output.pStep = float(Ipart) / pMean;
    output.nStep = float(Epart) / pMean;

    return output;
}

unsigned genExponent(unsigned pNum, long int z0, long int z1) {
    float y = 0.0f;
    float x = 0.0f;
    float k = 1.0f;
    float xr = 0.0f;

    float x_max = rounding<float, 2>((std::log(1.0f / ZERO)) / k);
    float step = x_max / (float(pNum) / 2.0f);
    unsigned pMean = (pNum / 2);

    Steps_t sp = {0, 0};
    sp = calcIEsteps(pMean);

    unsigned i = 0;
    while(i < pNum) {
        x = x * !(i == pMean);
        y = 1.0f / std::exp(k * x);
        x += step;
        sequence.get<Y_AXIS>(i) = (i < pMean) ? (std::abs(z0) * y) : (-std::abs(z1) * y);
        sequence.get<X_AXIS>(i) = xr;
        xr += (i < pMean) ? !(i == (pMean - 1)) * sp.pStep : 
              !(i == (pNum - 1)) * sp.nStep;
        i++;
    }
    sequence.get<X_AXIS>(i) = xr;
    
    return pNum;
}

unsigned genMeander(long int z0, long int z1) {
    const int PATTERN_SIZE = 4;
    unsigned pMean = PATTERN_SIZE / 2;
    float xr = 0.0f;

    Steps_t sp = {0, 0};
    sp = calcIEsteps(pMean);

    unsigned i = 0;
    while (i < PATTERN_SIZE) {
        sequence.get<Y_AXIS>(i) = (i < pMean) ? std::abs(z0) : -std::abs(z1);
        sequence.get<X_AXIS>(i) = xr;
        xr += (i < pMean) ? !(i == (pMean - 1)) * sp.pStep : 
              !(i == (PATTERN_SIZE - 1)) * sp.nStep;
        i++;
    }
    sequence.get<X_AXIS>(i) = xr;

    return PATTERN_SIZE;
}

unsigned genSin(unsigned pNum, long int z0, long int z1) {
    static const unsigned PERIOD_PI_RAD = 2;
    float y = 0.0f;
    float x = 0.0f;
    float xr = 0.0f;
    unsigned pMean = (pNum / 2);
    float step = float(PERIOD_PI_RAD) / pNum;

    Steps_t sp = {0, 0};
    sp = calcIEsteps(pMean);

    unsigned i = 0;
    while(i < pNum) {
        y = sin(PI * x);
        sequence.get<Y_AXIS>(i) = y * ((i <= pMean) ? std::abs(z0) : std::abs(z1));
        sequence.get<X_AXIS>(i) = xr;
        xr += (i < pMean) ? sp.pStep : sp.nStep;
        x += step;
        i++;
    }
    x += step;
    sequence.get<X_AXIS>(i) = xr;

    return pNum;
}

unsigned getDualLevelMeander(long int z0, long int z1) {
    const int PATTERN_SIZE = 8;
    float xr = 0.0f;
    long int hlvl = 0;
    long int llvl = 0;

    cout << "Insert second high level: ";
    cin >> hlvl;
    cout << "Insert second low level: ";
    cin >> llvl;

    unsigned pMean = (PATTERN_SIZE / 2);

    Steps_t sp = {0, 0};
    sp = calcIEsteps(pMean);

    const long int SEQ[PATTERN_SIZE / 2] = {std::abs(z0), std::abs(hlvl), 
                                            -std::abs(z1), -std::abs(llvl)};
    
    unsigned i = 0;
    while (i < PATTERN_SIZE) {
        sequence.get<Y_AXIS>(i) = SEQ[i / 2];
        sequence.get<X_AXIS>(i) = xr;
        xr += (i < pMean) ? !(i % 2) * sp.pStep : !(i % 2) * sp.nStep;
        i++;
    }
    sequence.get<X_AXIS>(i) = xr;

    return PATTERN_SIZE;
}


//см график y = a + (a / exp(x))
unsigned getDualLevelMeanderWithExp(unsigned pNum, long int z0, long int z1) {
    float y = 0.0f;
    float x = 0.0f;
    float k = 1.0f;
    float xr = 0.0f;

    long int hlvl = 0;
    long int llvl = 0;

    cout << "Insert second high level: ";
    cin >> hlvl;
    cout << "Insert second low level: ";
    cin >> llvl;

    unsigned pMean = pNum / 2;
    unsigned pQuart = pMean / 2;

    Steps_t sp0 = {0, 0};
    sp0 = calcIEsteps(pMean);
    
    cout << "Now insert I:E for second h and l level." << endl;
    Steps_t sp1 = {0, 0};
    sp1 = calcIEsteps(pQuart);

    unsigned i = 0;
    while (i < pMean) {
        y = 1.0f / std::exp(k * x);
    }


    return 0;
}

template<int X, int Y>
static constexpr int64_t constexpr_int_pow() {
    int64_t pow_val = X;
    for(int i = 0; i < (Y - 1); ++i)
        pow_val *= X;

    return pow_val;
}

template<typename T>
T restrictInt(int64_t in) {
    static_assert(!std::is_floating_point_v<T>, "This func should be use only for integer types!!!");
    // static constexpr bool       IS_TYPE_SIGNED = std::is_signed<T>::value;
    static constexpr bool       IS_TYPE_SIGNED = std::numeric_limits<T>::is_signed;
    static constexpr uint32_t   SIZE_T = sizeof(T);
    static constexpr int64_t    MAX_T = IS_TYPE_SIGNED ? (pow(2LL, SIZE_T * 8LL) / 2LL) - 1 : (pow(2LL, SIZE_T * 8LL) - 1);
    static constexpr int64_t    MIN_T = IS_TYPE_SIGNED ? -((pow(2LL, SIZE_T * 8LL) / 2LL)) : 0;
    return in > MAX_T ? MAX_T : (in < MIN_T ? MIN_T : in);
}

template<typename T, long long GRADE>
static inline T rounding(T x) {
    static constexpr long long DECADE = static_cast<int32_t>(constexpr_int_pow<10, GRADE>());
    cout << "DECADE = " << DECADE << endl;
    if constexpr (std::is_floating_point<T>::value) {
        static constexpr double PROBE_NUM = 1.0 / DECADE / 2.0;
        int64_t buf = (x + PROBE_NUM) * DECADE;
        return T(buf) / DECADE;
    } else {
        static constexpr int32_t PROBE_NUM = static_cast<int32_t>(DECADE / 2);
        int32_t buf = x + PROBE_NUM;
        buf -= buf % DECADE;
        return restrictInt<T>(buf);
    }
}

#include <tuple>
void test() {

    tpl::Tuple_t<int, float, char, double, char> tuple;
    std::tuple<int, float, char, double, char> ttupl;
    
    typedef typename tpl::Tuple_t<int, float, char, float> Tuple_type;
    arr::Array_t<10, int, char, float> arr;

    cout << tpl::get<0>(tuple) << endl;
    cout << tpl::get<1>(tuple) << endl;
    cout << tpl::get<2>(tuple) << endl;

    tpl::get<0>(tuple) = 10;
    cout << tpl::get<0>(tuple) << endl;

    cout << arr.getCount() << endl;

    cout << tpl::getCount<Tuple_type>() << endl;

    arr.get<0>(1);

    // cout << "Member Sizes: " << endl;
    // cout << arr.getMemberShifts(0) << endl;
    // cout << arr.getMemberShifts(1) << endl;
    // cout << arr.getMemberShifts(2) << endl;

    // cout << "NEW getter: " << endl;
    // cout << arr.get<int>(1, 0) << endl;
    // arr.get<int>(1, 0) = 123;
    // cout << arr.get<int>(1, 0) << endl;

    sizeof(tuple);
    sizeof(ttupl);
    sizeof(arr);

    while (true);
};
