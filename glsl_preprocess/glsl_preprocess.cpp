#include <cstdio>
#include <filesystem>
#include <vector>
#include <cstdlib>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <cstring>
#include <regex>
#include <fstream>

namespace fs = std::filesystem;

void printUsage();
bool textFromFile(const char* const path, std::string& out);
bool preprocessGLSL(fs::path path, std::ofstream& out, std::unordered_set<fs::path> pastIncludes = {}, fs::path prevPath = {});
void addIncludeDir(const char* dir);

const char* const usage_msg = R"usage(
Usage: %s <source file path> <out file path>

Flags:
    -I <dir> -> Add a directory to the list of directories to be searched for included files.
)usage";

std::vector<fs::path> includeDirs;
const char* scriptName = nullptr;

using FlagHandler_p = void(*)(const char*);

const std::unordered_map<std::string, FlagHandler_p> flagMap{
    std::pair<std::string, FlagHandler_p>("-I", addIncludeDir)
};

int main(int argc, const char* argv[])
{
    // get data from args

    scriptName = argv[0];
    
    fs::path sourceFilePath, outFilePath;

    for(int i = 1; i < argc; i++)
    {
        if(flagMap.contains(argv[i]))
            if(i+1 < argc)
            {
                flagMap.at(argv[i])(argv[i+1]);
                ++i;
            }
            else
            {
                std::fputs("Directory not provided for -I flag!\n", stderr);
                std::printf(usage_msg, argv[0]);
                return EXIT_FAILURE;
            }
        else if(sourceFilePath.empty())
        {
            sourceFilePath = argv[i];
        }
        else if(outFilePath.empty())
        {
            outFilePath = argv[i];
        }
        else
        {
            std::fprintf(stderr, "Invalid argument provided: %s\n", argv[i]);
            std::puts(usage_msg);
            return EXIT_FAILURE;
        }
    }
    if(sourceFilePath.empty())
    {
        std::fputs("No source file directory provided!\n", stderr);
        printUsage();
        return EXIT_FAILURE;
    }
    if(outFilePath.empty())
    {
        std::fputs("No output file directory provided!\n", stderr);
        printUsage();
        return EXIT_FAILURE;
    }
    if(sourceFilePath == outFilePath)
    {
        std::fputs("Output file cannot be the same as source file\n", stderr);
        return EXIT_FAILURE;
    }

    // opening output file
    
    if(!outFilePath.parent_path().empty())
    {
        std::puts(outFilePath.parent_path().c_str());
        //fs::create_directories(outFilePath.parent_path());
    }
    
    std::ofstream outFile(outFilePath);
    if(!outFile.is_open())
    {
        std::fprintf(stderr, "Failed opening output file: %s\n", outFilePath.c_str());
        return EXIT_FAILURE;
    }

    // processing

    bool preprocessStatus = preprocessGLSL(sourceFilePath, outFile);
    
    outFile.close();

    if(!preprocessStatus)
    {
        fs::remove(outFilePath);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;    
}
void printUsage()
{
    std::printf(usage_msg, scriptName);
}
bool preprocessGLSL(fs::path path, std::ofstream& out, std::unordered_set<fs::path> pastIncludes, fs::path prevPath)
{
    fs::path filePath = path;

    // locating correct file

    fs::path foundFilePath;

    if(prevPath.empty()) // if this is the original file we want to preprocess, we skip some checks and don't look for it in include dirs
    {
        if(!fs::exists(filePath))
        {
            std::fprintf(stderr, "File \"%s\" does not exist\n", path.c_str());
            return false;
        }
        if(fs::is_directory(filePath))
        {
            std::fprintf(stderr, "File \"%s\" is a directory\n", path.c_str());
            return false;
        }
        foundFilePath = filePath;
    }
    else
    {
        std::vector dirsToCheck = {prevPath.parent_path()};
        dirsToCheck.insert(dirsToCheck.end(), includeDirs.begin(), includeDirs.end());
        dirsToCheck.push_back("./");
        for(fs::path dir : dirsToCheck)
        {
            fs::path checkPath = dir / filePath;
            if(fs::exists(checkPath) && !fs::is_directory(checkPath))
            {
                foundFilePath = checkPath;
                break;
            }
        }
        if(foundFilePath.empty())
        {
            std::fprintf(stderr, "File to include not found: \"%s\"\n", path.c_str());
            return false;
        }
        if(pastIncludes.contains(fs::canonical(foundFilePath)))
        {
            std::fprintf(stderr, "Cyclic include detected: %s!\n", path.c_str());
            return false;
        }
    }

    pastIncludes.insert(fs::canonical(foundFilePath));

    // opening a file

    std::ifstream file(foundFilePath);
    if (!file.is_open()) 
    {
        std::fprintf(stderr, "Could not open a file: \"%s\"\n", path.c_str());
        return false;
    }
    // processing a file

    std::regex include_formula(R"regex(^\s*#include\s+"(.+?)")regex");

    std::string line;

    uint lineCount = 1;
    while (std::getline(file, line)) {
        std::smatch match;
        if (std::regex_search(line, match, include_formula)) 
        {
            fs::path includePath = match[1].str();
            if(!preprocessGLSL(includePath, out, pastIncludes, foundFilePath))
            {
                std::fprintf(stderr, "Required at %s:%u\n", path.c_str(), lineCount);
                return false;
            }
        } 
        else 
        {
            out << line << "\n";
        }
        ++lineCount;
    }
    if(!file.eof())
    {
        std::fprintf(stderr, "Failed to read entire file: \"%s\"\n", path.c_str());
        return false;
    }
    file.close();
    return true;
}
void addIncludeDir(const char* dir)
{
    includeDirs.push_back(dir);
}