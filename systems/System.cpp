//
// Created by insberr on 5/21/24.
//

#include "System.h"

App* System::app = nullptr; // Definition of the static member

void System::SetApp(App* a) {
    app = a;
}
