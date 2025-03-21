#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
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
            result[i][j] = round(sqrt(pow(points[i][1] - points[j][1], 2) + pow(points[i][2] - points[j][2], 2))); // NOLINT(*-narrowing-conversions)
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

bool vectorContains(const std::vector<int> &vector, const int value) {
    return std::ranges::find(vector, value) != vector.end();
}

long calculateScore(const std::vector<std::vector<int>>& matrix, const std::vector<int>& cycle) {
    long score = 0;
    for (size_t i = 0; i < cycle.size() - 1; ++i) {
        score += matrix[cycle[i]][cycle[i + 1]];
    }
    return score;
}

void greedyNN(const std::vector<std::vector<int>> &inMatrix, std::vector<std::vector<int>> &outCycles, long &score) {
    auto first_cycle = std::vector<int>();
    auto second_cycle = std::vector<int>();
    auto matrix = inMatrix;
    for (int i = 0; i < matrix.size(); ++i) {
        matrix[i][i] = std::numeric_limits<int>::max();;
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
        matrix[i][randomNumber] = std::numeric_limits<int>::max();;
    }
    second_cycle.push_back(furthestPointIdx(inMatrix, randomNumber));
    // std::ranges::fill(matrix[second_cycle.back()], INT_MAX);
    for (int i = 0; i < n; ++i) {
        matrix[i][second_cycle.back()] = std::numeric_limits<int>::max();;
    }
    do {
        auto last1 = first_cycle.back();
        auto closest1 = closestPointIdx(matrix, last1);
        first_cycle.push_back(closest1);
        score += inMatrix[last1][closest1];
        // std::ranges::fill(matrix[closest1], INT_MAX);
        for (int i = 0; i < n; ++i) {
            matrix[i][closest1] = std::numeric_limits<int>::max();;
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
            matrix[i][closest2] = std::numeric_limits<int>::max();;
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

void greedyCycle(const std::vector<std::vector<int>> &inMatrix, std::vector<std::vector<int>> &outCycles, long &score) {
    auto first_cycle = std::vector<int>();
    auto second_cycle = std::vector<int>();
    auto matrix = inMatrix;
    for (int i = 0; i < matrix.size(); ++i) {
        matrix[i][i] = std::numeric_limits<int>::max();;
    }

    const auto n = matrix.size();
    int remaining_points = n - 2;
    std::vector<bool> visited(n, false);

    // losowy punkt
    std::random_device seed;
    std::mt19937 gen(seed());
    std::uniform_int_distribution<> dist(0, n - 1);
    const int randomNumber = dist(gen);

    // dodajemy do obu cykli po pierwszym wierzchołku
    // std::ranges::fill(matrix[randomNumber], std::numeric_limits<int>::max(););
    first_cycle.push_back(randomNumber);
    visited[randomNumber] = true;
    second_cycle.push_back(furthestPointIdx(inMatrix, randomNumber));
    // std::ranges::fill(matrix[second_cycle.back()], std::numeric_limits<int>::max(););
    visited[second_cycle.back()] = true;

    //dodajemy do obu cykli po drugim najbliższym wierzchołku
    const auto first_closest1 = closestPointIdx(matrix, first_cycle.back());
    first_cycle.push_back(first_closest1);
    visited[first_cycle.back()] = true;

    const auto first_closest2 = closestPointIdx(matrix, second_cycle.back());
    second_cycle.push_back(first_closest2);
    visited[second_cycle.back()] = true;
    remaining_points -= 2;

    do {
        int best_node = -1;
        int best_position = -1;
        long min_increase = std::numeric_limits<long>::max();

        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                for (int j = 0; j < first_cycle.size() - 1; ++j) {
                    const long current_increase =   matrix[first_cycle[j]][i] +
                                                    matrix[i][first_cycle[j + 1]] -
                                                    matrix[first_cycle[j]][first_cycle[j + 1]];
                    if (current_increase < min_increase) {
                        min_increase = current_increase;
                        best_node = i;
                        best_position = j + 1;
                    }
                }
            }
        }
        // wstaw najlepszy wierzchołek w najlepsze miejsce
        first_cycle.insert(first_cycle.begin() + best_position, best_node);
        visited[best_node] = true;
        remaining_points--;

        if (remaining_points <= 0) {
            continue;
        }

        best_node = -1;
        best_position = -1;
        min_increase = std::numeric_limits<long>::max();

        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                for (int j = 0; j < second_cycle.size() - 1; ++j) {
                    const long current_increase = matrix[second_cycle[j]][i] + matrix[i][second_cycle[j + 1]] - matrix[second_cycle[j]][second_cycle[j + 1]];
                    if (current_increase < min_increase) {
                        min_increase = current_increase;
                        best_node = i;
                        best_position = j + 1;
                    }
                }
            }
        }
        // wstaw najlepszy wierzchołek w najlepsze miejsce
        second_cycle.insert(second_cycle.begin() + best_position, best_node);
        visited[best_node] = true;
        remaining_points--;

    } while (remaining_points > 0);

    first_cycle.push_back(first_cycle[0]);
    second_cycle.push_back(second_cycle[0]);

    score = calculateScore(matrix, first_cycle) + calculateScore(matrix, second_cycle);
    outCycles.push_back(first_cycle);
    outCycles.push_back(second_cycle);
}

void greedyCycle2Regret(const std::vector<std::vector<int>> &inMatrix, std::vector<std::vector<int>> &outCycles, long &score) {
    auto first_cycle = std::vector<int>();
    auto second_cycle = std::vector<int>();
    auto matrix = inMatrix;
    for (int i = 0; i < matrix.size(); ++i) {
        matrix[i][i] = std::numeric_limits<int>::max();;
    }

    const auto n = matrix.size();
    int remaining_points = n - 2;
    std::vector<bool> visited(n, false);

    // losowy punkt
    std::random_device seed;
    std::mt19937 gen(seed());
    std::uniform_int_distribution<> dist(0, n - 1);
    const int randomNumber = dist(gen);

    // dodajemy do obu cykli po pierwszym wierzchołku
    // std::ranges::fill(matrix[randomNumber], std::numeric_limits<int>::max(););
    first_cycle.push_back(randomNumber);
    visited[randomNumber] = true;
    second_cycle.push_back(furthestPointIdx(inMatrix, randomNumber));
    // std::ranges::fill(matrix[second_cycle.back()], std::numeric_limits<int>::max(););
    visited[second_cycle.back()] = true;

    //dodajemy do obu cykli po drugim najbliższym wierzchołku
    const auto first_closest1 = closestPointIdx(matrix, first_cycle.back());
    first_cycle.push_back(first_closest1);
    visited[first_cycle.back()] = true;

    const auto first_closest2 = closestPointIdx(matrix, second_cycle.back());
    second_cycle.push_back(first_closest2);
    visited[second_cycle.back()] = true;
    remaining_points -= 2;

    do {
        int best_node = -1;
        int best_position = -1;
        long minRegret = std::numeric_limits<long>::lowest();

        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                long best_increase = std::numeric_limits<long>::max();
                long second_best_increase = std::numeric_limits<long>::max();
                int best_pos = -1;

                for (int j = 0; j < first_cycle.size() - 1; ++j) {
                    const long current_increase =   matrix[first_cycle[j]][i] +
                                                    matrix[i][first_cycle[j + 1]] -
                                                    matrix[first_cycle[j]][first_cycle[j + 1]];
                    if (current_increase < best_increase) {
                        second_best_increase = best_increase;
                        best_increase = current_increase;
                        best_pos = j + 1;
                    } else if (current_increase < second_best_increase) {
                        second_best_increase = current_increase;
                    }
                }

                long regret = second_best_increase - best_increase;
                if (regret > minRegret) {
                    minRegret = regret;
                    best_node = i;
                    best_position = best_pos;
                }
            }
        }
        // wstaw najlepszy wierzchołek w najlepsze miejsce
        first_cycle.insert(first_cycle.begin() + best_position, best_node);
        visited[best_node] = true;
        remaining_points--;

        if (remaining_points <= 0) {
            continue;
        }

        best_node = -1;
        best_position = -1;
        minRegret = std::numeric_limits<long>::lowest();

        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                long best_increase = std::numeric_limits<long>::max();
                long second_best_increase = std::numeric_limits<long>::max();
                int best_pos = -1;

                for (int j = 0; j < second_cycle.size() - 1; ++j) {
                    const long current_increase =   matrix[second_cycle[j]][i] +
                                                    matrix[i][second_cycle[j + 1]] -
                                                    matrix[second_cycle[j]][second_cycle[j + 1]];
                    if (current_increase < best_increase) {
                        second_best_increase = best_increase;
                        best_increase = current_increase;
                        best_pos = j + 1;
                    } else if (current_increase < second_best_increase) {
                        second_best_increase = current_increase;
                    }
                }

                const long regret = second_best_increase - best_increase;
                if (regret > minRegret) {
                    minRegret = regret;
                    best_node = i;
                    best_position = best_pos;
                }
            }
        }
        // wstaw najlepszy wierzchołek w najlepsze miejsce
        second_cycle.insert(second_cycle.begin() + best_position, best_node);
        visited[best_node] = true;
        remaining_points--;

    } while (remaining_points > 0);

    first_cycle.push_back(first_cycle[0]);
    second_cycle.push_back(second_cycle[0]);

    score = calculateScore(matrix, first_cycle) + calculateScore(matrix, second_cycle);
    outCycles.push_back(first_cycle);
    outCycles.push_back(second_cycle);
}

void greedyCycle2Regret(const std::vector<std::vector<int>> &inMatrix, std::vector<std::vector<int>> &outCycles, long &score, const double regretWeight) {
    auto first_cycle = std::vector<int>();
    auto second_cycle = std::vector<int>();
    auto matrix = inMatrix;
    for (int i = 0; i < matrix.size(); ++i) {
        matrix[i][i] = std::numeric_limits<int>::max();;
    }

    const auto n = matrix.size();
    int remaining_points = n - 2;
    std::vector<bool> visited(n, false);

    // losowy punkt
    std::random_device seed;
    std::mt19937 gen(seed());
    std::uniform_int_distribution<> dist(0, n - 1);
    const int randomNumber = dist(gen);

    // dodajemy do obu cykli po pierwszym wierzchołku
    // std::ranges::fill(matrix[randomNumber], std::numeric_limits<int>::max(););
    first_cycle.push_back(randomNumber);
    visited[randomNumber] = true;
    second_cycle.push_back(furthestPointIdx(inMatrix, randomNumber));
    // std::ranges::fill(matrix[second_cycle.back()], std::numeric_limits<int>::max(););
    visited[second_cycle.back()] = true;

    //dodajemy do obu cykli po drugim najbliższym wierzchołku
    const auto first_closest1 = closestPointIdx(matrix, first_cycle.back());
    first_cycle.push_back(first_closest1);
    visited[first_cycle.back()] = true;

    const auto first_closest2 = closestPointIdx(matrix, second_cycle.back());
    second_cycle.push_back(first_closest2);
    visited[second_cycle.back()] = true;
    remaining_points -= 2;

    do {
        int best_node = -1;
        int best_position = -1;
        long minRegret = std::numeric_limits<long>::lowest();

        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                long best_increase = std::numeric_limits<long>::max();
                long second_best_increase = std::numeric_limits<long>::max();
                int best_pos = -1;

                for (int j = 0; j < first_cycle.size() - 1; ++j) {
                    const long current_increase =   matrix[first_cycle[j]][i] +
                                                    matrix[i][first_cycle[j + 1]] -
                                                    matrix[first_cycle[j]][first_cycle[j + 1]];
                    if (current_increase < best_increase) {
                        second_best_increase = best_increase;
                        best_increase = current_increase;
                        best_pos = j + 1;
                    } else if (current_increase < second_best_increase) {
                        second_best_increase = current_increase;
                    }
                }

                const long regret = second_best_increase - best_increase;
                const double weighted_regret = regretWeight * regret - best_increase;
                if (weighted_regret > minRegret) {
                    minRegret = weighted_regret;
                    best_node = i;
                    best_position = best_pos;
                }
            }
        }
        // wstaw najlepszy wierzchołek w najlepsze miejsce
        first_cycle.insert(first_cycle.begin() + best_position, best_node);
        visited[best_node] = true;
        remaining_points--;

        if (remaining_points <= 0) {
            continue;
        }

        best_node = -1;
        best_position = -1;
        minRegret = std::numeric_limits<long>::lowest();

        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                long best_increase = std::numeric_limits<long>::max();
                long second_best_increase = std::numeric_limits<long>::max();
                int best_pos = -1;

                for (int j = 0; j < second_cycle.size() - 1; ++j) {
                    const long current_increase =   matrix[second_cycle[j]][i] +
                                                    matrix[i][second_cycle[j + 1]] -
                                                    matrix[second_cycle[j]][second_cycle[j + 1]];
                    if (current_increase < best_increase) {
                        second_best_increase = best_increase;
                        best_increase = current_increase;
                        best_pos = j + 1;
                    } else if (current_increase < second_best_increase) {
                        second_best_increase = current_increase;
                    }
                }

                const long regret = second_best_increase - best_increase;
                const double weighted_regret = regretWeight * regret - best_increase;
                if (weighted_regret > minRegret) {
                    minRegret = weighted_regret;
                    best_node = i;
                    best_position = best_pos;
                }
            }
        }
        // wstaw najlepszy wierzchołek w najlepsze miejsce
        second_cycle.insert(second_cycle.begin() + best_position, best_node);
        visited[best_node] = true;
        remaining_points--;

    } while (remaining_points > 0);

    first_cycle.push_back(first_cycle[0]);
    second_cycle.push_back(second_cycle[0]);

    score = calculateScore(matrix, first_cycle) + calculateScore(matrix, second_cycle);
    outCycles.push_back(first_cycle);
    outCycles.push_back(second_cycle);
}


template <typename Func>
void experimentRunner(const int n, const std::vector<std::vector<int>> &distanceMatrix, const std::string& pointsFilename, Func tsp, const std::string& datasetName, const std::string& algName ) {
    long minScore = std::numeric_limits<long>::max();
    long maxScore = std::numeric_limits<long>::min();
    long cumulativeScore = 0;
    std::vector<std::vector<int>> bestCycles;

    std::cout << "Running experiment:" << std::endl;
    std::cout << "Dataset: " << datasetName << std::endl;
    std::cout << "Algorithm: " << algName << std::endl;

    for (int i = 0; i < n; ++i) {
        long score = 0;
        std::vector<std::vector<int>> cycles;

        tsp(distanceMatrix, cycles, score);

        if (score > maxScore) {
            maxScore = score;
        } else if (score < minScore) {
            minScore = score;
            bestCycles = cycles;
        }
        cumulativeScore += score;
    }

    std::cout << "avg_score (min_score - max_score)" << std::endl;
    std::cout << cumulativeScore / n << " (" << minScore << " - " << maxScore << ")" << std::endl;
    const auto cyclesFilename = datasetName + "_cycles_" + algName + ".csv";
    writeMatrixToCSV(bestCycles, cyclesFilename);
    system(("python ./wizualizacja/main.py " + cyclesFilename + ' ' + pointsFilename).c_str());
}

template <typename Func>
void experimentRunner(const int n, const std::vector<std::vector<int>> &distanceMatrix, const std::string& pointsFilename, Func tsp, const std::string& datasetName, const std::string& algName, double regretWeight ) {
    static_assert(std::is_invocable_r_v<void, Func,
              const std::vector<std::vector<int>>&,
              std::vector<std::vector<int>>&,
              long&,
              const double>,
              "Func must match the signature of greedyCycle2Regret");
    long minScore = std::numeric_limits<long>::max();
    long maxScore = std::numeric_limits<long>::min();
    long cumulativeScore = 0;
    std::vector<std::vector<int>> bestCycles;

    std::cout << "Running experiment:" << std::endl;
    std::cout << "Dataset: " << datasetName << std::endl;
    std::cout << "Algorithm: " << algName << std::endl;

    for (int i = 0; i < n; ++i) {
        long score = 0;
        std::vector<std::vector<int>> cycles;

        tsp(distanceMatrix, cycles, score, regretWeight);

        if (score > maxScore) {
            maxScore = score;
        } else if (score < minScore) {
            minScore = score;
            bestCycles = cycles;
        }
        cumulativeScore += score;
    }

    std::cout << "avg_score (min_score - max_score)" << std::endl;
    std::cout << cumulativeScore / n << " (" << minScore << " - " << maxScore << ")" << std::endl;
    const auto cyclesFilename = datasetName + "_cycles_" + algName + ".csv";
    writeMatrixToCSV(bestCycles, cyclesFilename);
    system(("python ./wizualizacja/main.py " + cyclesFilename + ' ' + pointsFilename).c_str());
}

int main(int argc, char *argv[]) {
    const auto datasetName = std::string(argv[1]).substr(0, 7);

    // parsujemy plik .tsp
    const auto list = parseTspFile(argv[1]);
    std::string pointsFilename = datasetName + "_Pointlist.csv";
    writeMatrixToCSV(list, pointsFilename);

    // obliczamy macierz odległości
    const auto distanceMatrix = calculateDistanceMatrix(list);
    writeMatrixToCSV(distanceMatrix, datasetName + "_distanceMatrix.csv");

    // greedyNN
    experimentRunner(100, distanceMatrix, pointsFilename, greedyNN, datasetName, "GreedyNN");

    // greedyCycle
    experimentRunner(100, distanceMatrix, pointsFilename, greedyCycle, datasetName, "greedyCycle");

    // greedy regret
    auto regretFunc = [](const std::vector<std::vector<int>> &inMatrix,
                     std::vector<std::vector<int>> &outCycles,
                     long &score) {
        greedyCycle2Regret(inMatrix, outCycles, score);
    };
    experimentRunner(100, distanceMatrix, pointsFilename, regretFunc, datasetName, "greedyCycle2Regret");

    // regret weighted
    auto regretFunc2 = [](const std::vector<std::vector<int>> &inMatrix,
                     std::vector<std::vector<int>> &outCycles,
                     long &score,
                     const double regretWeight) {
        greedyCycle2Regret(inMatrix, outCycles, score, regretWeight);
    };
    experimentRunner<>(100, distanceMatrix, pointsFilename, regretFunc2, datasetName, "greedyCycleWeighted2Regret", 2.5);
}