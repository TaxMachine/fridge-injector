//
// Created by TaxMachine on 2023-11-08.
//

#ifndef FRIDGE_INJECTOR_GUI_HPP
#define FRIDGE_INJECTOR_GUI_HPP

#include <Windows.h>

class GUI {
    public:
        void render();
    private:
        HWND createForm();
        HWND createLabel();

}

#endif //FRIDGE_INJECTOR_GUI_HPP
