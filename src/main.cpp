#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

namespace fs = std::filesystem;

bool replaceInFile(const std::string &filename, const std::string &toReplace,
				   const std::string &replaceWith) {
	std::ifstream inFile(filename);
	if (!inFile) {
		std::cerr << "Error opening file for reading: " << filename
				  << std::endl;
		return false;
	}

	std::string content((std::istreambuf_iterator<char>(inFile)),
						std::istreambuf_iterator<char>());
	inFile.close();

	size_t pos = 0;
	bool replaced = false;

	while ((pos = content.find(toReplace, pos)) != std::string::npos) {
		content.replace(pos, toReplace.length(), replaceWith);
		pos += replaceWith.length();
		replaced = true;
	}

	if (!replaced) {
		std::cout << "Warning: No occurrences of \"" << toReplace
				  << "\" found in " << filename << std::endl;
	}

	std::ofstream outFile(filename, std::ios::trunc);
	if (!outFile) {
		std::cerr << "Error opening file for writing: " << filename
				  << std::endl;
		return false;
	}

	outFile << content;
	outFile.close();

	return true;
}

int runCommand(const std::string &command) {
	int result = std::system(command.c_str());
	if (result != 0) {
		std::cerr << "Command failed: " << command << std::endl;
	}
	return result;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: newp <project_name>" << std::endl;
		return 1;
	}

	std::string projectName = argv[1];

	if (!fs::is_empty(fs::current_path())) {
		std::cerr << "Error: Current directory is not empty." << std::endl;
		return 1;
	}

	std::cout << "Cloning template repository..." << std::endl;

	int cloneResult =
		runCommand("git clone --depth 1 "
				   "https://codeberg.org/woker/nix-cpp-template.git .");

	if (cloneResult != 0) {
		return 1;
	}

	std::cout << "Replacing project name..." << std::endl;

	if (!replaceInFile("CMakeLists.txt", "newp", projectName)) {
		return 1;
	}

	if (!replaceInFile("flake.nix", "newp", projectName)) {
		return 1;
	}

	std::cout << "Reinitializing git repository..." << std::endl;

	fs::remove_all(".git");
	fs::remove_all("result");
	fs::remove_all("flake.lock");
	fs::remove_all("src/main.cpp");

	if (runCommand("git init") != 0)
		return 1;
	if (runCommand("git add .") != 0)
		return 1;
	if (runCommand("git commit -m \"Initial commit\"") != 0)
		return 1;

	std::cout << "\nProject \"" << projectName << "\" created successfully!"
			  << std::endl;

	return 0;
}
