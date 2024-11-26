#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <set>
#include "iomanip"

using namespace std;

static vector<string> getMinterms(const vector<int>& results, const int& n) {
    vector<string> minterms;
    for (size_t i = 0; i < results.size(); i++) {
        if (results[i] == 1) {
            bitset<16> binary_i(i);
            string binary_str = binary_i.to_string();
            minterms.push_back(binary_str.substr(binary_str.size()-n));
        }
    }
    return minterms;
}

static bool isCombine(const string& minterm1, const string& minterm2)
{
    int diffCount = 0;
    for (int i = 0; i < minterm1.size(); i++) {
        if (minterm1[i] != minterm2[i]) {
            diffCount++;
        }
    }
    return diffCount == 1;
}

static string combine(const string& minterm1, const string& minterm2)
{
    string result = minterm1;
    for (size_t i = 0; i < minterm1.size(); i++) {
        if (minterm1[i] != minterm2[i]) {
            result[i] = '-';
        }
    }
    return result;
}

template <typename T>
static void delFromVect(vector<T>& vect, T& elem)
{
    auto it = find(vect.begin(), vect.end(), elem);
    if (it != vect.end()) {
        vect.erase(it);
    }
}

static vector<string> combineMinterms(vector<string> minterms) {
    bool wasCombined = true;
    vector<string>result;

    while (wasCombined) {
        wasCombined = false;
        vector<string> combined;
        vector<bool> combinedFlags(minterms.size(), false);

        for (size_t i = 0; i < minterms.size(); i++) {
            for (size_t j = i + 1; j < minterms.size(); j++) {
                if (isCombine(minterms[i], minterms[j])) {
                    string combinedMinterm = combine(minterms[i], minterms[j]);
                    delFromVect(result, minterms[i]);
                    delFromVect(result, minterms[j]);
                    delFromVect(combined, minterms[i]);
                    delFromVect(combined, minterms[j]);
                    if (find(combined.begin(), combined.end(), combinedMinterm) == combined.end())
                    {
                        combined.push_back(combinedMinterm);
                        result.push_back(combinedMinterm);
                    }
                    combinedFlags[i] = true;
                    combinedFlags[j] = true;
                    wasCombined = true;
                }
            }
        }
        for (size_t i = 0; i < combinedFlags.size(); i++) {
            if (!combinedFlags[i]) {
                if (find(result.begin(), result.end(), minterms[i]) == result.end())
                {
                    result.push_back(minterms[i]);
                }
            }
        }
        minterms = combined;
    }

    return result;
}

static bool isCover(const string& term1, const string& term2)
{
    int diffCount = 0;
    for (int i = 0; i < term1.size(); i++) {
        if (term1[i] != term2[i] && term1[i] != '-') {
            diffCount++;
        }
    }
    return diffCount == 0;
}

static vector<vector<bool>> getImplicantMatrix(const vector<string>& minterms, const vector<string>& combinedMinterms)
{
    vector<vector<bool>> matrix(combinedMinterms.size(), vector<bool>(minterms.size(), false));

    for (size_t i = 0; i < minterms.size(); i++) {
        for (size_t j = 0; j < combinedMinterms.size(); j++) {
            if (isCover(combinedMinterms[j], minterms[i])) {
                matrix[j][i] = true;
            }
        }
    }

    return matrix;
}

static string convertMinterm(const string& minterm, const char ch[]) {
    string result;
    for (size_t i = 0; i < minterm.size(); ++i) {
        if (minterm[i] == '1') {
            result += ch[i];
        }
        else if (minterm[i] == '0') {
            result += '!';
            result += ch[i];
        }
    }
    return result;
}

static void printImplicantMatrix(const vector<string>& minterms,
                                 const vector<string>& combinedMinterms,
                                 const vector<vector<bool>>& implicantMatrix,
                                 const int& n)
{
    const int cellWidth1 = n + 2;
    cout << "\n\tИмпликантная матрица\n";
    cout << setw(cellWidth1) << "";
    
    for (size_t i = 0; i < minterms.size(); i++) cout << setw(cellWidth1) << left << minterms[i];
    cout << endl;

    for (size_t i = 0; i < combinedMinterms.size(); i++) {
        cout <<  setw(cellWidth1) << left << combinedMinterms[i];
        for (size_t j = 0; j < minterms.size(); j++) {
            cout << setw(cellWidth1);
            implicantMatrix[i][j] == 1 ? cout << '+' : cout << ' ';
        }
        cout << endl;
    }

    char ch[] = {'x', 'y', 'z', 'w', 'v', 'u', 't', 'n', 'm', 'p'};

    const int cellWidth2 = 2*n+2;

    cout << "\n\tИмпликантная матрица\n";
    cout << setw(cellWidth2) << "";

    for (size_t i = 0; i < minterms.size(); i++) {
        cout << setw(cellWidth2) << left << convertMinterm(minterms[i], ch);
    }
    cout << endl;

    for (size_t i = 0; i < combinedMinterms.size(); i++) {
        cout << setw(cellWidth2) << left << convertMinterm(combinedMinterms[i], ch);

        for (size_t j = 0; j < minterms.size(); j++) {
            cout << setw(cellWidth2);
            implicantMatrix[i][j] ? cout << '+' : cout << ' ';
        }
        cout << endl;
    }
}

static set<vector<int>> findCoverings(const vector<vector<bool>>& implicantMatrix, int& minSize)
{
    set<vector<int>> coverings;
    int numMinterms = implicantMatrix[0].size();
    int numCombinedMinterms = implicantMatrix.size();

    for (int i = 0; i < (1 << numCombinedMinterms); i++) {
        vector<int> covering;
        vector<bool> covered(numMinterms, false);

        for (int j = 0; j < numCombinedMinterms; j++) {
            if (i & (1 << j)) {
                covering.push_back(j);
                for (int k = 0; k < numMinterms; k++) {
                    if (implicantMatrix[j][k]) {
                        covered[k] = true;
                    }
                }
            }
        }

        if (count(covered.begin(), covered.end(), false) == 0) {
            coverings.insert(covering);
            if (covering.size() < minSize) minSize = covering.size();
        }
    }

    return coverings;
}

static vector<vector<int>> findMinCovering(const set<vector<int>>& allCoverings, const int& minSize)
{
    vector<vector<int>> result;
    for (const auto& covering : allCoverings) {
        if (covering.size() == minSize) {
            result.push_back(covering);
        }
    }
    return result;
}

static void printMDNF(const vector<string>& combinedMinterms, const vector<vector<int>>minCoverings)
{
    cout << "\nМДНФ:\n";
    for (size_t i = 0; i < minCoverings.size(); i++) {
        cout << i+1 << ") ";
        for (size_t j = 0; j < minCoverings[i].size(); j++) {
            j != minCoverings[i].size() - 1 ? cout << combinedMinterms[minCoverings[i][j]] << " v "
                : cout << combinedMinterms[minCoverings[i][j]] << endl;
        }
    }

    cout << "\nМДНФ:\n";
    char ch[] = { 'x', 'y', 'z', 'w', 'v', 'u', 't', 'n', 'm', 'p' };
    for (size_t i = 0; i < minCoverings.size(); i++) {
        cout << i + 1 << ") ";
        for (size_t j = 0; j < minCoverings[i].size(); j++) {
            string strMinterm = convertMinterm(combinedMinterms[minCoverings[i][j]], ch);
            j != minCoverings[i].size() - 1 ? cout << strMinterm << " v " : cout << strMinterm << endl;
        }
    }
}

static void minimizationQuine(const vector<int>& binaryVector)
{
    vector<string> minterms = getMinterms(binaryVector, log2(binaryVector.size()));
    vector<string> combinedMinterms = combineMinterms(minterms);

    vector<vector<bool>> implicantMatrix = getImplicantMatrix(minterms, combinedMinterms);

    printImplicantMatrix(minterms, combinedMinterms, implicantMatrix, log2(binaryVector.size()));

    int minSize = 10000;
    set<vector<int>> coverings = findCoverings(implicantMatrix, minSize);

    printMDNF(combinedMinterms, findMinCovering(coverings, minSize));
}

static vector<int> getBinaryString() 
{
    string input;
    vector<int> binaryVector;

    while (true) {
        cout << "Введите двоичную строку: ";
        getline(cin, input);

        input.erase(remove(input.begin(), input.end(), ' '), input.end());

        bool validInput = true;
        for (char ch : input) {
            if (ch != '0' && ch != '1') {
                validInput = false;
                break;
            }
        }

        if (validInput) {
            for (char ch : input) {
                binaryVector.push_back(ch - '0');
            }

            int size = binaryVector.size();
            if ((size & (size - 1)) == 0) { // Проверка на степень двойки
                break;
            }
            else {
                binaryVector.clear();
                cout << "Ошибка: количество элементов должно быть 2^n. Попробуйте снова." << endl;
            }
        }
        else {
            cout << "Ошибка: строка должна содержать только 0 и 1. Попробуйте снова." << endl;
        }
    }
    return binaryVector;
}

int main() {
    setlocale(LC_ALL, "Rus");
    vector<int> binaryVector = getBinaryString(); // 11110001 1110001000000011 11011011 1000100000111111
    minimizationQuine(binaryVector);
    return 0;
}