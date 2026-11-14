#include "TitleScreen.h"

#include <cassert>
#include <string>

int main() {
    MyButton button;
    setButtonText(button, "Play");
    assert(std::string(button.text.data()) == "Play");

    setButtonText(button, std::string(64, 'x'));
    assert(button.text.back() == '\0');
    assert(std::string(button.text.data()).size() == button.text.size() - 1);
    return 0;
}
