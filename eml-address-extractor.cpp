#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <regex>
#include <unordered_set>
namespace fs = std::filesystem;

std::string tolower(std::string const &input)
{
    std::string output;
    auto lambda = [](unsigned char c) { return std::tolower(c); };
    std::transform(input.begin(), input.end(), std::back_inserter(output), lambda);
    return output;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "path expected\n";
        return 0;
    }

    fs::path path{argv[1]};

    if (fs::is_directory(path))
    {
        std::regex pattern{R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})"};
        std::unordered_set<std::string> address;

        for (auto const &entry : fs::recursive_directory_iterator{path})
        {
            if (entry.is_regular_file() && entry.path().extension() == ".eml" && !entry.path().filename().string().starts_with("."))
            {
                if (std::ifstream ifs{entry.path()})
                {
                    bool flag = false;
                    for (std::string line; std::getline(ifs, line);)
                    {
                        if (tolower(line).starts_with("from:") || tolower(line).starts_with("sender:"))
                        {
                            // std::cout << line << '\n';
                            if (std::smatch matches; std::regex_search(line, matches, pattern))
                            {
                                for (auto const &match : matches)
                                {
                                    if (!address.contains(match.str()))
                                    {
                                        address.insert(match.str());
                                        std::cout << match.str() << '\n';
                                    }
                                }
                            }
                            flag = true;
                            break;
                        }
                    }

                    if (!flag)
                    {
                        // std::cout << entry.path().string() << '\n';
                    }
                }
                else
                {
                    // std::cout << entry.path() << " -> ifstream NOT open!\n";
                }
            }
        }
    }
    else if (fs::is_regular_file(path))
    {
        // regular file
    }

    return 0;
}