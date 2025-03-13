#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include <random>

bool is_number(const std::string& s)
{
    auto it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

std::vector<std::vector<int>> parseTspFile(const std::string& filename) {
    std::vector<std::vector<int>> result;

    // read the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "File " << filename << " could not be opened." << std::endl;
        return result;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string token;
        std::stringstream ss(line);
        std::vector<int> vec;
        while (std::getline(ss, token, ' ')) {
            if (token == "EOF") {
                return result;
            } else if (is_number(token)) {
                vec.push_back(std::stoi(token));
            }
        }

        if (vec.size() == 3) result.push_back(vec);
    }
    return result;
}

std::vector<std::vector<int>> calculateDistanceMatrix(const std::vector<std::vector<int>>& points) {
    std::vector<std::vector<int>> result(points.size(), std::vector<int>(points.size(), 0));

    for (int i = 0; i < points.size(); i++) {
        for (int j = i + 1; j < points.size(); j++) {
            result[i][j] = sqrt(pow(points[i][1] - points[j][1], 2) + pow(points[i][2] - points[j][2], 2)); // NOLINT(*-narrowing-conversions)
            result[j][i] = result[i][j];
        }
    }
    return result;
}

bool writeMatrixToCSV(const std::vector<std::vector<int>>& matrix, const std::string& filename) {
    std::ofstream outputFile(filename);

    if (!outputFile.is_open()) {
        return false;
    }

    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            outputFile << matrix[i][j];

            if (j < matrix[i].size() - 1) {
                outputFile << ",";
            }
        }
        outputFile << "\n";
    }

    outputFile.close();
    return true;
}

int closestPointIdx(const std::vector<std::vector<int>>& matrix, const int point_id) {
    return std::distance(matrix[point_id].begin(), std::ranges::min_element(matrix[point_id]));
}

int furthestPointIdx(const std::vector<std::vector<int>>& matrix, const int point_id) {
    return std::distance(matrix[point_id].begin(), std::ranges::max_element(matrix[point_id]));
}

// void removeByValue(std::vector<int> &vector, const int value) {
//     vector.erase(std::ranges::remove(vector, value).begin(),
//                            vector.end());
// }

bool vectorContains(const std::vector<int> &vector, const int value) {
    return std::ranges::find(vector, value) != vector.end();
}

void greedyTpsNN(const std::vector<std::vector<int>> &inMatrix, std::vector<std::vector<int>> &outCycles, int &score) {
    auto first_cycle = std::vector<int>();
    auto second_cycle = std::vector<int>();
    auto matrix = inMatrix;
    for (int i = 0; i < matrix.size(); ++i) {
        matrix[i][i] = INT_MAX;
    }

    const auto n = matrix.size();
    int remaining_points = n - 2;

    // losowy punkt
    std::random_device seed;
    std::mt19937 gen(seed());
    std::uniform_int_distribution<> dist(0, n - 1);
    const int randomNumber = dist(gen);

    // dodajemy do obu cykli po pierwszym wierzchołku
    // std::ranges::fill(matrix[randomNumber], INT_MAX);
    first_cycle.push_back(randomNumber);
    for (int i = 0; i < n; ++i) {
        matrix[i][randomNumber] = INT_MAX;
    }
    second_cycle.push_back(furthestPointIdx(inMatrix, randomNumber));
    // std::ranges::fill(matrix[second_cycle.back()], INT_MAX);
    for (int i = 0; i < n; ++i) {
        matrix[i][second_cycle.back()] = INT_MAX;
    }
    do {
        auto last1 = first_cycle.back();
        auto closest1 = closestPointIdx(matrix, last1);
        first_cycle.push_back(closest1);
        score += inMatrix[last1][closest1];
        // std::ranges::fill(matrix[closest1], INT_MAX);
        for (int i = 0; i < n; ++i) {
            matrix[i][closest1] = INT_MAX;
        }
        remaining_points--;

        if (remaining_points == 0) {
            continue;
        }

        auto last2 = second_cycle.back();
        auto closest2 = closestPointIdx(matrix, last2);
        second_cycle.push_back(closest2);
        score += inMatrix[closest2][last2];
        // std::ranges::fill(matrix[closest2], INT_MAX);
        for (int i = 0; i < n; ++i) {
            matrix[i][closest2] = INT_MAX;
        }
        remaining_points--;
    } while (remaining_points > 0);

    score += inMatrix[first_cycle[0]][first_cycle[first_cycle.size() - 2]];
    first_cycle.push_back(first_cycle[0]);
    second_cycle.push_back(second_cycle[0]);

    score += inMatrix[second_cycle[0]][second_cycle[second_cycle.size() - 2]];
    outCycles.push_back(first_cycle);
    outCycles.push_back(second_cycle);
}

int main(int argc, char *argv[]) {

    // parsujemy plik .tsp
    const auto list = parseTspFile(argv[1]);
    writeMatrixToCSV(list, std::string(argv[1]) + "_list.csv");

    // obliczamy macierz odległości
    const auto distanceMatrix = calculateDistanceMatrix(list);
    writeMatrixToCSV(distanceMatrix, std::string(argv[1]) + "_distanceMatrix.csv");

    // algorytm 1: greedy nearest neighbour
    std::cout << "Greedy nearest neighbour" << std::endl;
    int greedyNNScore = 0;
    std::vector<std::vector<int>> greedyNNCycles;
    greedyTpsNN(distanceMatrix, greedyNNCycles, greedyNNScore);
    writeMatrixToCSV(greedyNNCycles, std::string(argv[1]) + "_greedyNN.csv");
    std::cout << "c1 len\tc2 len\tscore" << std::endl;
    std::cout << greedyNNCycles[0].size() << "\t" <<  greedyNNCycles[1].size() << "\t" << greedyNNScore << std::endl;
}
