/*
 *  This file is part of SCH Matrix.
 *
 *  SCH Matrix is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SCH Matrix is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SCH Matrix. If not, see <https://www.gnu.org/licenses/>.
 */

#include "aboutwindow.h"

/*
 * Licencek miatt is kell, meg szeretjük is a hírnevet.
 */

AboutWindow::AboutWindow(QWidget* parent/*=NULL*/)
    :QDialog(parent)
{
    setWindowTitle(tr("About the AnimEditor"));
    QStringList names;
    //tökmind1 a sorrend, meg lesz keverve
    names<<"CsLaci"<<"Hami"<<"Csirke"<<"Ubul";

    layout_=new QGridLayout(this);

    //kedves gyerekek, ÍGY KELL tömböt keverni.
    for(int i=0;i<names.size();++i)
    {
        std::swap(names[i],names[rand()%names.size()]);
    }
    mainText_=new QLabel((tr("<h1>Matrix AnimEditor 2012</h1>"
                            "<h3>By %1</h3>")+
                            //miért ne?
                            tr("<h4>Powered by <a href=\"http://z0r.de/1384\">"
                            "Tree-DAG™</a></h4>")+
                            //ezt kötelező betenni az FMOD licenc miatt
                            tr("Uses <a href=\"http://www.fmod.org/\">FMOD "
                            "Sound System</a>, copyright © Firelight "
                            "Technologies Pty, Ltd., 1994-2012.<br><br>")+
                            //ezt meg a Lua licenc miatt
                            tr("Uses <a href=\"http://www.lua.org/\">Lua</a>, "
                            "Copyright © 1994-2012 Lua.org, PUC-Rio.<br><br>")+
                            //ezt meg az Oxygen licenc miatt
                            tr("Uses <a href=\"http://www.oxygen-icons.org/\">"
                            "Oxygen Icons</a>.<br>")
                         ).arg(names.join(tr(", "))),this);
    aboutQt_=new QPushButton(tr("About Qt..."),this);
    aboutLua_=new QPushButton(tr("About Lua..."),this);
    ok_=new QPushButton(tr("OK"),this);
    layout_->addWidget(mainText_,0,0,1,3);
    layout_->addWidget(aboutQt_,1,0,1,1);
    layout_->addWidget(aboutLua_,1,1,1,1);
    layout_->addWidget(ok_,1,2,1,1);

    connect(mainText_,SIGNAL(linkActivated(const QString&)),
            this,SLOT(linkClicked(const QString&)));
    connect(aboutQt_,SIGNAL(clicked()),
            QApplication::instance(),SLOT(aboutQt()));
    connect(aboutLua_,SIGNAL(clicked()),this,SLOT(aboutLua()));
    connect(ok_,SIGNAL(clicked()),this,SLOT(accept()));
}

void AboutWindow::aboutLua()
{
    QDialog a;
    QGridLayout l(&a);
    QLabel main(tr("Copyright © 1994-2012 Lua.org, PUC-Rio.\n"
                   "Permission is hereby granted, free of charge, to any "
                   "person obtaining a copy of this software and associated "
                   "documentation files (the \"Software\"), to deal in the "
                   "Software without restriction, including without limitation "
                   "the rights to use, copy, modify, merge, publish, "
                   "distribute, sublicense, and/or sell copies of the "
                   "Software, and to permit persons to whom the Software is "
                   "furnished to do so, subject to the following conditions:\n"

                   "\nThe above copyright notice and this permission notice "
                   "shall be included in all copies or substantial portions of "
                   "the Software.\n\n"

                   "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF "
                   "ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED "
                   "TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A "
                   "PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL "
                   "THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, "
                   "DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF "
                   "CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN "
                   "CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS "
                   "IN THE SOFTWARE."),&a);
    main.setWordWrap(true);
    l.addWidget(&main,0,0,1,3);
    QPushButton ok(tr("OK"),&a);
    l.addWidget(&ok,1,1,1,1);
    connect(&ok,SIGNAL(clicked()),&a,SLOT(accept()));
    a.exec();
}

void AboutWindow::linkClicked(const QString& url)
{
    QDesktopServices::openUrl(url);
}
