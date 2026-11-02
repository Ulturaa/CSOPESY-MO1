#include <iostream>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8); // or SetConsoleOutputCP(65001);

    // Always use string literals ("") rather than char literals ('') 
    // for non-ASCII characters, as they occupy multiple bytes.
    std::cout << "Extended characters: ñ, é, ß, ♠, €" << std::endl;
    std::cout << "    ╱|、\n";
    std::cout << "  (˚ˎ 。7     CSOPESY MO1\n";
    std::cout << "   |、˜〵     Process Scheduler\n";
    std::cout << "  じしˍ,)ノ\n\n";
    std::cout.flush();
    
    std::string temp;
    std::cin >> temp;

    return 0;
}