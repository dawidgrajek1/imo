#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

std::vector<std::vector<int>> distanceMatrix(const std::vector<std::vector<int>>& points) {
    std::vector<std::vector<int>> result(points.size(), std::vector<int>(points.size(), 0));

    for (int i = 0; i < points.size(); i++) {
        for (int j = i + 1; j < points.size(); j++) {
            result[i][j] =
        }
    }

    return result;
}

int main(int argc, char *argv[]) {
    const auto list = parseTspFile(argv[1]);
    std::cout << list.size() << std::endl;
}
