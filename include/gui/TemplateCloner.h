#pragma once
/**
*Copyright (c) 2010 Fdastero LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
**/
#include "ZPrerequisites.h"
#include "gui/GuiPrerequisite.h"

namespace ZGame
{
    namespace Gui
    {
        class TemplateCloner
        {
        public:
            TemplateCloner();
            ~TemplateCloner();

            void
                setTemplateDoc(Rocket::Core::ElementDocument* doc)
            {
                _templateDoc = doc;
            }

            /** \brief This method will return the clone of an element given by id.**/
            Rocket::Core::Element*
                getCloneById(const Rocket::Core::String& id);
            
        private:
            Rocket::Core::ElementDocument* _templateDoc;
        };
    }
}